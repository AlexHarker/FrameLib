
#include "FrameLib_Spatial.h"
#include "FrameLib_Sort_Functions.h"

// Constructor

FrameLib_Spatial::FrameLib_Spatial(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, &sParamInfo, 1, 1)
{
    mParameters.addEnum(kInputMode, "inputmode");
    mParameters.addEnumItem(kPolar, "polar");
    mParameters.addEnumItem(kCartesian, "cartesian");
    
    mParameters.addVariableDoubleArray(kSpeakers, "speakers", 0.0, 360, 0);
    mParameters.addVariableDoubleArray(kWeights, "weights", 1.0, 360, 0);
    
    mParameters.addDouble(kRolloff, "rolloff", 6.0);
    mParameters.setMin(0.0);
    mParameters.addDouble(kBlur, "blur", 0.4);
    mParameters.setMin(0.0000001);
    mParameters.addInt(kMaxSpeakers, "maxspeakers", 0);
    mParameters.setMin(0);
    mParameters.addDouble(kPoints, "points", 0.0);
    mParameters.setClip(0.0, 1.0);
        
    mParameters.set(serialisedParameters);
    
    unsigned long speakerSize;
    const double *speakers = mParameters.getArray(kSpeakers, &speakerSize);
    
    for (unsigned long i = 0; i < (speakerSize + 2) / 3; i++)
    {
        double azimuth = speakerSize > (i * 3 + 0) ? speakers[i * 3 + 0] : 0.0;
        double elevation = speakerSize > (i * 3 + 1) ? speakers[i * 3 + 1] : 0.0;
        double radius = speakerSize > (i * 3 + 2) ? speakers[i * 3 + 2] : 1.0;
        
        mSpeakers.push_back(convertToCartesian(Polar(azimuth, elevation, radius)));
    }
}

// Info

std::string FrameLib_Spatial::objectInfo(bool verbose)
{
    return formatInfo("Generates multiplication factors for a number of speakers from an input coordinate triple: "
                   "The alogirthm used is a modified version of DBAP, with extra features. "
                   "Input may be in cartesian coordinates (x, y, z) or polar ones (azimuth, elevation, radius. "
                   "Missing values at the input are assumed zero. Extra values are ignored. "
                   "The output size is equal to the number of loudspeakers.",
                   "Generates multiplication factors for a number of speakers from an input coordinate triple.", verbose);
}

std::string FrameLib_Spatial::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Input Triple - cartesian or polar values for which to generate factors.", "Input Triple", verbose);

}

std::string FrameLib_Spatial::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Multiplication Factors - one per loudspeaker.", "Multiplication Factors", verbose);
}

// Parameter Info

FrameLib_Spatial::ParameterInfo FrameLib_Spatial::sParamInfo;

FrameLib_Spatial::ParameterInfo::ParameterInfo()
{
    add("Sets the input coordinate mode.");
    add("Sets the speaker positions in polar triples (one triple per speaker).");
    add("Sets the speaker weightings (one value per speaker).");
    add("Sets the rolloff in dB.");
    add("Sets the blur factor.");
    add("Sets the maximum number of speakers to be used (the neaarest N speakers will be used only). "
        "If zero all speakers are used.");
    add("Interpolate to point source panning (0 is modified DBAP - 1 is point source).");
}

// Conversion Helper

FrameLib_Spatial::Cartesian FrameLib_Spatial::convertToCartesian(Polar position)
{
    double theta = (M_PI / 180.0) * (90 - position.elevation);
    double psi = (M_PI / 180.0) * position.azimuth;
    
    return Cartesian(position.radius * sin(theta) * sin(psi), position.radius * sin(theta) * cos(psi), position.radius * cos(theta));
}

// Process

void FrameLib_Spatial::process()
{
    unsigned long sizeIn, weightsSize;
    double *input = getInput(0, &sizeIn);
    
    const double *weights = mParameters.getArray(kWeights, &weightsSize);
    unsigned long numSpeakers = mSpeakers.size();
    unsigned long maxSpeakers = mParameters.getValue(kMaxSpeakers);
    
    double blur = mParameters.getValue(kBlur);
    double rolloff = mParameters.getValue(kRolloff);
    double pointFactor = mParameters.getValue(kPoints);
    
    double rolloffFactor =  rolloff / (20 * log10(2.0));;
    double blur2 = blur * blur;
    maxSpeakers = maxSpeakers == 0 ? numSpeakers : maxSpeakers;
    
    requestOutputSize(0, numSpeakers);
    allocateOutputs();
    
    double *output = getOutput(0, &numSpeakers);
    
    double norm = 0.0;
    double minDistance;
    int nearestIdx = 0;
    
    Cartesian panPosition;
    
    if (((InputModes) mParameters.getValue(kInputMode)) == kPolar)
    {
        double azimuth = sizeIn > 0 ? input[0] : 0.0;
        double elevation = sizeIn > 1 ? input[1] : 0.0;
        double radius = sizeIn > 2 ? input[2] : 1.0;
        
        panPosition = convertToCartesian(Polar(azimuth, elevation, radius));
    }
    else
    {
        double x = sizeIn > 0 ? input[0] : 0.0;
        double y = sizeIn > 1 ? input[1] : 0.0;
        double z = sizeIn > 2 ? input[2] : 0.0;
        
        panPosition = Cartesian(x, y, z);;
    }
    
    for (unsigned long i = 0; i < numSpeakers; i++)
    {
        double xDelta = panPosition.x - mSpeakers[i].x;
        double yDelta = panPosition.y - mSpeakers[i].y;
        double zDelta = panPosition.z - mSpeakers[i].z;
        
        double sqDistance = xDelta * xDelta + yDelta * yDelta + zDelta * zDelta;
        double weight = i < weightsSize ? weights[i] : 1.0;
        output[i] = weight / pow(sqDistance + blur2, 0.5 * rolloffFactor);
        
        if (i == 0 || (sqDistance < minDistance))
        {
            nearestIdx = i;
            minDistance = sqDistance;
        }
    }
    
    // Reduce number of speakers
    
    if (maxSpeakers < numSpeakers)
    {
        unsigned long *indices = (unsigned long *) mAllocator->alloc(numSpeakers * sizeof(unsigned long));
        sortIndicesDescending(indices, output, numSpeakers);
        
        for (unsigned long i = maxSpeakers; i < numSpeakers; i++)
            output[indices[i]] = 0.0;
        
        mAllocator->dealloc(indices);
    }
    
    // Interpolate to points
    
    if (pointFactor > 0.0)
    {
        for (int i = 0; i < numSpeakers; i++)
        {
            if (i == nearestIdx)
                output[i] += pointFactor * (1.0 - output[i]);
            else
                output[i] -= pointFactor * output[i];
        }
    }
    
    // Normalise
    
    for (unsigned long i = 0; i < numSpeakers; i++)
        norm += output[i] * output[i];
    
    norm = sqrt(1 / norm);
    
    for (unsigned long i = 0; i < numSpeakers; i++)
        output[i] *= norm;
}

