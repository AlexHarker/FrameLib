
#ifndef FRAMELIB_SPATIAL_H
#define FRAMELIB_SPATIAL_H

#include "FrameLib_DSP.h"
#include <vector>

class FrameLib_Spatial : public FrameLib_Processor
{
    // Spatial Types
    
    struct Cartesian
    {
        Cartesian() {}
        Cartesian(double X, double Y, double Z) : x(X), y(Y), z(Z) {}
        
        double x, y, z;
    };
    
    struct Polar
    {
        Polar() {}
        Polar(double a, double e, double r) : azimuth(a), elevation(e), radius(r) {}
        
        double azimuth, elevation, radius;
    };
    
    // Parameter Enums and Info

    enum ParameterList {kInputMode, kSpeakers, kWeights, kRolloff, kBlur, kMaxSpeakers, kPoints};
    enum InputModes {kPolar, kCartesian};

public:
    
    // Constructor
    
    FrameLib_Spatial(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
private:

    // Conversion Helper
    
    Cartesian convertToCartesian(Polar position);
    
    // Process
    
    void process();
    
private:
    
    // Data
    
    std::vector <Cartesian> mSpeakers;
    
};

#endif
