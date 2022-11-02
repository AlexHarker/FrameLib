
#include "FrameLib_Gaussian.h"

// Constructor

FrameLib_Gaussian::FrameLib_Gaussian(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 3, 1)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kRequestedLength, "requested");
    mParameters.addEnumItem(kInLength, "input");
    
    mParameters.addDouble(kLength, "length", 1, 1);
    mParameters.setMin(0);
    
    mParameters.addEnum(kUnits, "units", 2);
    mParameters.addEnumItem(kSamples, "samples");
    mParameters.addEnumItem(kMS, "ms");
    mParameters.addEnumItem(kSeconds, "seconds");
    
    mParameters.addDouble(kMean, "mean", 0.0, 3);

    mParameters.addDouble(kDeviation, "deviation", 1.0, 4);

    mParameters.addEnum(kMismatchMode, "mismatch", 5);
    mParameters.addEnumItem(kDefault, "default");
    mParameters.addEnumItem(kExtend, "extend");
    
    mParameters.set(serialisedParameters);
        
    setParameterInput(2);
}

// Info

std::string FrameLib_Gaussian::objectInfo(bool verbose)
{
    return formatInfo("Generates frames of normally distributed random values: "
                      "Mean and standard deviation can be set per sample or by parameter. "
                      "Per-sample mean and standard deviations are passed in by input. "
                      "The length of the output is dependent on the mode parameter. "
                      "Output length may be set by parameter or based on that of the trigger input. "
                      "When input and output lengths are mismatched the result depends on the mismatch parameter.",
                      "Generates frames of gaussian distributed random values.", verbose);
}

std::string FrameLib_Gaussian::inputInfo(unsigned long idx, bool verbose)
{
    if (idx == 0)
            return formatInfo("Means - values used to generate the output", "Means", verbose);
    else if (idx == 1)
            return formatInfo("Standard Deviations - values used to generate the output", "Standard Deviations", verbose);
    else
            return parameterInputInfo(verbose);
}

std::string FrameLib_Gaussian::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_Gaussian::ParameterInfo FrameLib_Gaussian::sParamInfo;

FrameLib_Gaussian::ParameterInfo::ParameterInfo()
{
    add("Controls how the output length is determined: "
        "requested - the output length is set by the length parameter. "
        "input - the output length follows the length of the trigger input.");
    add("Sets the requested output length in the units specified by the units parameter.");
    add("Sets the units used for the length parameter.");
    add("Sets the default mean.");
    add("Sets the default standard deviation.");
    add("Sets the mode used when dealing with mismatches between input and output lengths: "
        "default - missing input values are substitued using the default values. "
        "extend - inputs are extended by repeating their final value. "
        "Note that default values are always used when an input is empty.");
}

// Helpers

unsigned long FrameLib_Gaussian::getLength()
{
    double time = mParameters.getValue(kLength);
    
    switch (mParameters.getEnum<Units>(kUnits))
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
void generate(random_generator<>& gen, double *output, T mean, U stddev, V& i, V loop)
{
    for ( ; (i + 1) < loop; i += 2)
    {
        double x, y;
        
        gen.rand_gaussians(x, y);
        
        output[i + 0] = x * get(stddev, i + 0) + get(mean, i + 0);
        output[i + 1] = y * get(stddev, i + 1) + get(mean, i + 1);
    }
    
    for ( ; i < loop; i++)
        output[i] = gen.rand_gaussian(get(mean, i), get(stddev, i));
}

// Process

void FrameLib_Gaussian::process()
{
    unsigned long sizeIn1, sizeIn2, sizeOut;
    bool extend = mParameters.getEnum<MismatchModes>(kMismatchMode) == kExtend;

    const double *means = getInput(0, &sizeIn1);
    const double *stddevs = getInput(1, &sizeIn2);
    
    sizeOut = mParameters.getEnum<Modes>(kMode) == kInLength ? std::max(sizeIn1, sizeIn2) : getLength();
    requestOutputSize(0, sizeOut);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    unsigned long loop1 = std::min(std::min(sizeIn1, sizeIn2), sizeOut);
    unsigned long loop2 = std::min(sizeIn1, sizeOut);
    unsigned long loop3 = std::min(sizeIn2, sizeOut);
    unsigned long i = 0;
    
    double fmean = extend && sizeIn1 ? means[sizeIn1 - 1] : mParameters.getValue(kMean);
    double fstddev = extend && sizeIn2 ? stddevs[sizeIn2 - 1] : mParameters.getValue(kDeviation);
    
    // Use inputs / fixed standard deviation / fixed mean / both fixed
    
    generate(mRandom, output, means, stddevs, i, loop1);
    generate(mRandom, output, means, fstddev, i, loop2);
    generate(mRandom, output, fmean, stddevs, i, loop3);
    generate(mRandom, output, fmean, fstddev, i, sizeOut);
}
