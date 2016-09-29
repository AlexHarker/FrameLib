
#ifndef FRAMELIB_COORDINATESYSTEM_H
#define FRAMELIB_COORDINATESYSTEM_H

#include "FrameLib_DSP.h"

#include <vector>

class FrameLib_CoordinateSystem : public FrameLib_Processor
{
    struct Cartesian
    {
        Cartesian(){}
        Cartesian(double X, double Y, double Z) : x(X), y(Y), z(Z) {}

        double x, y, z;
    };
    
    struct Polar
    {
        Polar(){}
        Polar(double a, double e, double r) : azimuth(a), elevation(e), radius(r) {}
        
        double azimuth, elevation, radius;
    };
    
    Cartesian convertToCartesian(Polar position)
    {
        double theta = (M_PI / 180.0) * (90 - position.elevation);
        double psi = (M_PI / 180.0) * position.azimuth;
        
        return Cartesian(position.radius * sin(theta) * sin(psi), position.radius * sin(theta) * cos(psi), position.radius * cos(theta));
    }
    
    Polar convertToPolar(Cartesian position)
    {
        // FIX - use degrees! - or have options
        
        double radius = sqrt(position.x * position.x + position.y * position.y + position.z * position.z);
        double azimuth = atan2(position.x, position.y) * (180.0 / M_PI);
        double elevation = asin(position.z / radius) * (180.0 / M_PI);
        
        return Polar(azimuth, elevation, radius);
    }
    
    enum AttributeList {kMode};
    enum InputModes {kPolarToCartesian, kCartesianToPolar};

public:
    
    FrameLib_CoordinateSystem (FrameLib_Context context, FrameLib_Attributes::Serial *serialisedAttributes, void *owner) : FrameLib_Processor(context, 1, 1)
    {
        mAttributes.addEnum(kMode, "mode");
        mAttributes.addEnumItem(kPolarToCartesian, "polar->cartesian");
        mAttributes.addEnumItem(kCartesianToPolar, "cartesian->polar");
        
        mAttributes.set(serialisedAttributes);
    }
    
protected:
    
    void process()
    {
        unsigned long sizeIn, sizeOut;
        double *input = getInput(0, &sizeIn);
        
        requestOutputSize(0, 3);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        
        if (((InputModes) mAttributes.getValue(kMode)) == kPolarToCartesian)
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
};

#endif
