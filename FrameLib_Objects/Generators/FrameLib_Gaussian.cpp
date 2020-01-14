
#include "FrameLib_Gaussian.h"

// Constructor

FrameLib_Gaussian::FrameLib_Gaussian(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 3, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kRequestedLength, "requested");
    mParameters.addEnumItem(kInLength, "input");
    
    mParameters.addInt(kLength, "length", 1, 1);
    mParameters.setMin(0);
    
    mParameters.addEnum(kUnits, "units", 2);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    
    mParameters.addDouble(kMean, "mean", 0.0, 3);

    mParameters.addDouble(kStdDev, "stddev", 1.0, 4);

    mParameters.set(serialisedParameters);
        
    setParameterInput(2);
}

// Info

std::string FrameLib_Gaussian::objectInfo(bool verbose)
{
    return formatInfo("Generates frames of gaussian distributed random values: "
                      "The size of the output is dependent on the mode. "
                      "The mean and standard deviation of the values are determined by the input values. "
                      "The output size may either be set as a parameter, or be set to match that of the triggering input.",
                      "Generates frames of gaussian distributed random values.", verbose);
}

std::string FrameLib_Gaussian::inputInfo(unsigned long idx, bool verbose)
{
    if (idx == 0)
            return formatInfo("Means Per Sample", "Means Per Sample", verbose);
    else if (idx == 1)
            return formatInfo("Standard Deviations Per Sample", "Standard Deviations Per Sample", verbose);
    else
            return parameterInputInfo(verbose);
}

std::string FrameLib_Gaussian::outputInfo(unsigned long idx, bool verbose)
{
    return "Frame of Gaussian Random Values";
}

// Parameter Info

FrameLib_Gaussian::ParameterInfo FrameLib_Gaussian::sParamInfo;

FrameLib_Gaussian::ParameterInfo::ParameterInfo()
{
    add("Controls how the output length is determined: "
        "requested - the output frame size is set by the length parameter. "
        "input - the output frame size will match the input size.");
    add("Sets the length of the output when the mode is set to requested. Set in the units specified by the units parameter.");
    add("Sets the units for specified output lengths.");
    add("Sets the default mean.");
    add("Sets the default standard deviation.");
}

// Helpers

unsigned long FrameLib_Gaussian::getLength()
{
    double time = mParameters.getValue(kLength);
    
    switch (static_cast<Units>(mParameters.getInt(kUnits)))
    {
        case kSamples:  break;
        case kMS:       time = msToSamples(time);       break;
        case kSeconds:  time = secondsToSamples(time);  break;
    }
    
    return roundToUInt(time);
}

// Generation helpers

double get(double value, unsigned long i)           { return value; }
double get(const double* values, unsigned long i)   { return values[i]; }

template<typename T, typename U, typename V>
void generate(FrameLib_RandGen& gen, double *output, T mean, U stddev, V& i, V loop)
{
    for ( ; (i + 1) < loop; i += 2)
    {
        double x, y;
        
        gen.randGaussians(x, y);
        
        output[i + 0] = x * get(stddev, i + 0) + get(mean, i + 0);
        output[i + 1] = y * get(stddev, i + 1) + get(mean, i + 1);
    }
        
    for ( ; i < loop; i++)
        output[i] = gen.randGaussian(get(mean, i), get(stddev, i));
}

// Process

void FrameLib_Gaussian::process()
{
    unsigned long sizeIn1, sizeIn2, sizeOut;
        
    const double *means = getInput(0, &sizeIn1);
    const double *stddevs = getInput(1, &sizeIn2);
    
    sizeOut = ((Modes) mParameters.getInt(kMode)) == kInLength ? std::max(sizeIn1, sizeIn2) : getLength();
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    unsigned long loop1 = std::min(std::min(sizeIn1, sizeIn2), sizeOut);
    unsigned long loop2 = std::min(sizeIn1, sizeOut);
    unsigned long loop3 = std::min(sizeIn2, sizeOut);
    unsigned long i = 0;
    
    double fmean = mParameters.getValue(kMean);
    double fstddev = mParameters.getValue(kStdDev);
    
    // Use inputs / fixed standard deviation / fixed mean / both fixed
    
    generate(mRandom, output, means, stddevs, i, loop1);
    generate(mRandom, output, means, fstddev, i, loop2);
    generate(mRandom, output, fmean, stddevs, i, loop3);
    generate(mRandom, output, fmean, fstddev, i, sizeOut);
}

