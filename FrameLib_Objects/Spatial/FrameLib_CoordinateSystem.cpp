
#include "FrameLib_CoordinateSystem.h"

// Constructor

FrameLib_CoordinateSystem::FrameLib_CoordinateSystem(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 1)
{
    mParameters.addEnum(kMode, "mode");
    mParameters.addEnumItem(kPolarToCartesian, "polar->cartesian");
    mParameters.addEnumItem(kCartesianToPolar, "cartesian->polar");
    mParameters.setInstantiation();
    
    mParameters.setInfo(&sParamInfo);

    mParameters.set(serialisedParameters);
}

// Info

const char *FrameLib_CoordinateSystem::objectInfo(bool verbose)
{
    return getInfo("Converts vectors of triples between cartesian and polar values: "
                   "Inputs should either be a cartesian triple (x, y, x) or polar tripe (azimuth, elevation, radius). "
                   "Missing values are assumed to be zeroes. Extra values are ignored and the output is also a triple.",
                   "Converts vectors of triples between cartesian and polar values.", verbose);
}

const char *FrameLib_CoordinateSystem::inputInfo(unsigned long idx, bool verbose)
{
    return getInfo("Input Triple - cartesian or polar values to be converted.", "Input Triple", verbose);
}

const char *FrameLib_CoordinateSystem::outputInfo(unsigned long idx, bool verbose)
{
    return "Converted values";
}

// Parameter Info

FrameLib_CoordinateSystem::ParameterInfo FrameLib_CoordinateSystem::sParamInfo;

FrameLib_CoordinateSystem::ParameterInfo::ParameterInfo()
{
    add("Sets the conversion mode.");
}

// Conversion Helpers

FrameLib_CoordinateSystem::Cartesian FrameLib_CoordinateSystem::convertToCartesian(Polar position)
{
    double theta = (M_PI / 180.0) * (90 - position.elevation);
    double psi = (M_PI / 180.0) * position.azimuth;
    
    return Cartesian(position.radius * sin(theta) * sin(psi), position.radius * sin(theta) * cos(psi), position.radius * cos(theta));
}

FrameLib_CoordinateSystem::Polar FrameLib_CoordinateSystem::convertToPolar(Cartesian position)
{
    // FIX - use degrees! - or have options
    
    double radius = sqrt(position.x * position.x + position.y * position.y + position.z * position.z);
    double azimuth = atan2(position.x, position.y) * (180.0 / M_PI);
    double elevation = asin(position.z / radius) * (180.0 / M_PI);
    
    return Polar(azimuth, elevation, radius);
}

// Process

void FrameLib_CoordinateSystem::process()
{
    unsigned long sizeIn, sizeOut;
    double *input = getInput(0, &sizeIn);
    
    requestOutputSize(0, 3);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);
    
    if (((InputModes) mParameters.getValue(kMode)) == kPolarToCartesian)
    {
        double azimuth = sizeIn > 0 ? input[0] : 0.0;
        double elevation = sizeIn > 1 ? input[1] : 0.0;
        double radius = sizeIn > 2 ? input[2] : 1.0;
        
        Cartesian panPosition = convertToCartesian(Polar(azimuth, elevation, radius));
        
        if (sizeOut)
        {
            output[0] = panPosition.x;
            output[1] = panPosition.y;
            output[2] = panPosition.z;
        }
    }
    else
    {
        double x = sizeIn > 0 ? input[0] : 0.0;
        double y = sizeIn > 1 ? input[1] : 0.0;
        double z = sizeIn > 2 ? input[2] : 0.0;
        
        Polar panPosition = convertToPolar(Cartesian(x, y, z));
        
        if (sizeOut)
        {
            output[0] = panPosition.azimuth;
            output[1] = panPosition.elevation;
            output[2] = panPosition.radius;
        }
    }
}