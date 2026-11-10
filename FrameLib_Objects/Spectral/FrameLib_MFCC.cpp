
#include "FrameLib_MFCC.h"
#include "../Common_Utilities/FrameLib_FluidAllocator.h"
#include "../Common_Utilities/FrameLib_Scaling_Functions.h"

#define EIGEN_MPL2_ONLY 1
#include <flucoma/algorithms/public/DCT.hpp>
#include <flucoma/algorithms/public/MelBands.hpp>
#include <algorithm>
#include <cmath>
#include <new>

struct FrameLib_MFCC::Implementation
{
    Implementation(FrameLib_DSP::Allocator allocator, unsigned long maxBands, unsigned long maxFFTSize)
    : mRawAllocator(allocator), mAllocator(mRawAllocator)
    , mMelBands(static_cast<fluid::index>(maxBands), static_cast<fluid::index>(maxFFTSize), mAllocator)
    , mDCT(static_cast<fluid::index>(maxBands), static_cast<fluid::index>(maxBands), mAllocator) {}

    FrameLib_FluidAllocator mRawAllocator;
    fluid::Allocator mAllocator;
    fluid::algorithm::MelBands mMelBands;
    fluid::algorithm::DCT mDCT;
};

FrameLib_MFCC::FrameLib_MFCC(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 1, 1), mMaxFFTSize(16)
{
    mParameters.addInt(kMaxFFTSize, "max_fft_size", 16384, 0);
    mParameters.setClip(16, 65536);
    mParameters.setInstantiation();
    mParameters.addInt(kMaxBands, "max_bands", 128, 1);
    mParameters.setClip(2, 1024);
    mParameters.setInstantiation();
    mParameters.addInt(kCoefficients, "coefficients", 13, 2);
    mParameters.setMin(1);
    mParameters.addInt(kBands, "bands", 40, 3);
    mParameters.setMin(2);
    mParameters.addDouble(kMinFrequency, "min_frequency", 20.0, 4);
    mParameters.setMin(0.0);
    mParameters.addDouble(kMaxFrequency, "max_frequency", 20000.0, 5);
    mParameters.setMin(0.0);
    mParameters.addBool(kDropZero, "drop_zero", false, 6);
    mParameters.addBool(kNormalise, "normalise", true, 7);
    mParameters.set(serialisedParameters);
    addParameterInput();

    while (mMaxFFTSize < static_cast<unsigned long>(mParameters.getInt(kMaxFFTSize)))
        mMaxFFTSize <<= 1;
    
    mImplementation.reset(new Implementation(Allocator(*this), mParameters.getInt(kMaxBands), mMaxFFTSize));
    
    objectReset();
}

FrameLib_MFCC::~FrameLib_MFCC() = default;

std::string FrameLib_MFCC::objectInfo(bool verbose)
{
    return formatInfo("Calculates mel-frequency cepstral coefficients (MFCCs) from a magnitude spectrum using FluCoMa's algorithms. "
                      "The input is a magnitude spectrum including DC and Nyquist. "
                      "The output is a frame of coefficients. The sampling rate is taken from the host.",
                      "Calculates mel-frequency cepstral coefficients from a magnitude spectrum.", verbose);
}

std::string FrameLib_MFCC::inputInfo(unsigned long idx, bool verbose)
{
    return idx ? parameterInputInfo(verbose) : formatInfo("Magnitude spectrum including DC and Nyquist.", "Magnitude spectrum", verbose);
}

std::string FrameLib_MFCC::outputInfo(unsigned long, bool)
{
    return "MFCCs";
}

// Parameter Info

FrameLib_MFCC::ParameterInfo FrameLib_MFCC::sParamInfo;

FrameLib_MFCC::ParameterInfo::ParameterInfo()
{
    add("Sets the maximum FFT size, rounded up to a power of two (16 to 65536).");
    add("Sets the maximum number of mel bands.");
    add("Sets the number of coefficients output (limited by the number of bands).");
    add("Sets the number of mel bands (limited by the maximum number of bands).");
    add("Sets the lowest frequency in Hz.");
    add("Sets the highest frequency in Hz.");
    add("Drops the zeroth coefficient from the output.");
    add("Sets whether the input is normalised (as by fl.fft~), such that values are scaled to match unnormalised FFT magnitudes.");
}

// Process

void FrameLib_MFCC::objectReset()
{
    // Values that can never match, so that the next frame always initialises

    mInitBins = -1;
    mInitBands = -1;
    mInitCoefficients = -1;
    mInitSamplingRate = -1.0;
}

void FrameLib_MFCC::process()
{
    auto offsetAmount = [](unsigned long binFactor)
    {
        return ampToDb(1.0 / static_cast<double>(std::max(binFactor, 1UL)));
    };
    
    auto scalingFactor = [](unsigned long dctSize)
    {
        return 1.0 / static_cast<double>(dctSize);
    };
    
    unsigned long size;
    const double *input = getInput(0, &size);
    const double minFrequency = mParameters.getValue(kMinFrequency);
    const double maxFrequency = mParameters.getValue(kMaxFrequency);
    const long dropZero = mParameters.getBool(kDropZero) ? 1 : 0;
    const long bands = std::min(mParameters.getInt(kBands), mParameters.getInt(kMaxBands));
    const long dctSize = std::min(mParameters.getInt(kCoefficients) + dropZero, bands);
    const bool valid = input && size >= 2 && size <= mMaxFFTSize / 2 + 1 && minFrequency < maxFrequency;

    if (!valid)
        getReporter()(ErrorSource::Object, getProxy(), "MFCC requires a magnitude spectrum no larger than max_fft_size, and min_frequency below max_frequency");

    requestOutputSize(0, valid ? dctSize - dropZero : 0);
    AutoArray<double> temp = allocAutoArray<double>(valid ? bands + dctSize : 0);
    if (!allocateOutputs() || !valid || !temp)
        return;
    
    unsigned long outputSize;
    double *output = getOutput(0, &outputSize);
    const long bins = static_cast<long>(size);
    
    // Read all the flags, as they must all be cleared
    
    const bool changedMinFrequency = mParameters.changed(kMinFrequency);
    const bool changedMaxFrequency = mParameters.changed(kMaxFrequency);
    const bool initMelBands = changedMinFrequency || changedMaxFrequency || bins != mInitBins || bands != mInitBands || mSamplingRate != mInitSamplingRate;
    const bool initDCT = bands != mInitBands || dctSize != mInitCoefficients;
    
    const double offset = offsetAmount(mParameters.getBool(kNormalise) ? 1 : bins - 1);
    
    // FluCoMa's views require mutable pointers, but the input is only read.
    
    fluid::RealVectorView in(const_cast<double *>(input), 0, bins);
    fluid::RealVectorView melBands(temp.data(), 0, bands);
    fluid::RealVectorView coefficients(temp.data() + bands, 0, dctSize);
    
    try
    {
        if (initMelBands)
            mImplementation->mMelBands.init(minFrequency, maxFrequency, bands, bins, mSamplingRate, 2 * (bins - 1), mImplementation->mAllocator);
        if (initDCT)
            mImplementation->mDCT.init(bands, dctSize, mImplementation->mAllocator);
        
        mInitBins = bins;
        mInitBands = bands;
        mInitCoefficients = dctSize;
        mInitSamplingRate = mSamplingRate;
        
        mImplementation->mMelBands.processFrame(in, melBands, false, false, true, mImplementation->mAllocator);
        
        if (offset)
        {
            for (long i = 0; i < bands; i++)
                melBands[i] += offset;
        }
        
        mImplementation->mDCT.processFrame(melBands, coefficients);
        
        // Undoing normalisation scales the magnitudes, which is an offset in dB
        
        if (!dropZero)
            coefficients[0] *= scalingFactor(dctSize);
        
        copyVector(output, temp.data() + bands + dropZero, dctSize - dropZero);
    }
    catch (const std::bad_alloc&)
    {
        objectReset();
        std::fill(output, output + outputSize, 0.0);
        getReporter()(ErrorSource::Object, getProxy(), "MFCC could not allocate processing memory");
    }
}
