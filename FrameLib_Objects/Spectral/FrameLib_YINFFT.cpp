#include "FrameLib_YINFFT.h"
#include "../Common_Utilities/FrameLib_FluidAllocator.h"

#define EIGEN_MPL2_ONLY 1
#include <flucoma/algorithms/public/YINFFT.hpp>
#include <cmath>
#include <new>

struct FrameLib_YINFFT::Implementation
{
    Implementation(FrameLib_DSP::Allocator allocator, unsigned long maxLength)
    : mRawAllocator(allocator), mAllocator(mRawAllocator),
      mYin(static_cast<fluid::index>(maxLength / 2 + 1), mAllocator) {}

    FrameLib_FluidAllocator mRawAllocator;
    fluid::Allocator mAllocator;
    fluid::algorithm::YINFFT mYin;
};

FrameLib_YINFFT::FrameLib_YINFFT(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 1, 2), mMaxLength(16)
{
    mParameters.addInt(kMaxLength, "max_length", 16384, 0);
    mParameters.setClip(16, 65536);
    mParameters.setInstantiation();
    mParameters.addDouble(kMinFrequency, "min_frequency", 20.0, 1);
    mParameters.setMin(0.0);
    mParameters.addDouble(kMaxFrequency, "max_frequency", 10000.0, 2);
    mParameters.setMin(0.0);
    mParameters.set(serialisedParameters);
    addParameterInput();

    while (mMaxLength < static_cast<unsigned long>(mParameters.getInt(kMaxLength)))
        mMaxLength <<= 1;
    mImplementation.reset(new Implementation(Allocator(*this), mMaxLength));
}

FrameLib_YINFFT::~FrameLib_YINFFT() = default;

std::string FrameLib_YINFFT::objectInfo(bool verbose)
{
    return formatInfo("Estimate pitch using FluCoMa's YINFFT algorithm. "
                      "Input is a magnitude spectrum including DC and Nyquist, with a power-of-two FFT size. "
                      "Outputs are pitch in Hz and confidence.", "Estimate pitch from a magnitude spectrum.", verbose);
}

std::string FrameLib_YINFFT::inputInfo(unsigned long idx, bool verbose)
{
    return idx ? parameterInputInfo(verbose) : formatInfo("Magnitude spectrum including DC and Nyquist.", "Magnitude spectrum", verbose);
}

std::string FrameLib_YINFFT::outputInfo(unsigned long idx, bool)
{
    return idx ? "Pitch confidence" : "Pitch in Hz";
}

FrameLib_YINFFT::ParameterInfo FrameLib_YINFFT::sParamInfo;

FrameLib_YINFFT::ParameterInfo::ParameterInfo()
{
    add("Maximum FFT size, rounded up to a power of two (16 to 65536).");
    add("Lowest pitch frequency in Hz.");
    add("Highest pitch frequency in Hz.");
}

void FrameLib_YINFFT::process()
{
    unsigned long size;
    const double *input = getInput(0, &size);
    const double minFrequency = mParameters.getValue(kMinFrequency);
    const double maxFrequency = mParameters.getValue(kMaxFrequency);
    const unsigned long fftSize = size > 1 ? 2 * (size - 1) : 0;
    const bool valid = input && size >= 9 && size <= mMaxLength / 2 + 1
        && fftSize && !(fftSize & (fftSize - 1))
        && std::isfinite(minFrequency) && std::isfinite(maxFrequency)
        && minFrequency < maxFrequency && maxFrequency <= mSamplingRate / 2;

    if (!valid)
        getReporter()(ErrorSource::Object, getProxy(), "YINFFT requires a valid magnitude spectrum, and ordered frequency bounds below Nyquist");

    requestOutputSize(0, valid ? 1 : 0);
    requestOutputSize(1, valid ? 1 : 0);
    if (!allocateOutputs() || !valid)
        return;

    unsigned long outputSize;
    double *pitch = getOutput(0, &outputSize);
    double *confidence = getOutput(1, &outputSize);
    if (!pitch || !confidence)
        return;

    *pitch = *confidence = 0;

    double result[2] = { 0, 0 };

    // FluCoMa's input view requires a mutable pointer, but YINFFT only reads it.
    fluid::RealVectorView in(const_cast<double *>(input), 0, static_cast<fluid::index>(size));
    fluid::RealVectorView out(result, 0, 2);

    try
    {
        mImplementation->mYin.processFrame(in, out, minFrequency, maxFrequency, mSamplingRate, mImplementation->mAllocator);
        *pitch = result[0];
        *confidence = result[1];
    }
    catch (const std::bad_alloc&)
    {
        getReporter()(ErrorSource::Object, getProxy(), "YINFFT could not allocate processing memory");
    }
}
