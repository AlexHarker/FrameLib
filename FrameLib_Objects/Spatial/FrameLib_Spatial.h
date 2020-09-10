
#ifndef FRAMELIB_SPATIAL_H
#define FRAMELIB_SPATIAL_H

#include "FrameLib_DSP.h"
#include <vector>

class FrameLib_Spatial final : public FrameLib_Processor
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

    enum ParameterList { kInputMode, kSpeakers, kWeights, kRolloff, kBlur, kMaxSpeakers, kPoints };
    enum InputModes { kPolar, kCartesian };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_Spatial(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:

    // Conversion Helper
    
    Cartesian convertToCartesian(Polar position);
    
    // Process
    
    void process() override;
    
    // Data
    
    std::vector<Cartesian> mSpeakers;

    static ParameterInfo sParamInfo;
};

#endif
