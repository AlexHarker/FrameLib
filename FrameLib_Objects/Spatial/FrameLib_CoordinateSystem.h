
#ifndef FRAMELIB_COORDINATESYSTEM_H
#define FRAMELIB_COORDINATESYSTEM_H

#include "FrameLib_DSP.h"

class FrameLib_CoordinateSystem final : public FrameLib_Processor
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

    enum ParameterList { kMode };
    enum InputModes { kPolarToCartesian, kCartesianToPolar };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    FrameLib_CoordinateSystem(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:
    
    // Conversion Helpers
    
    Cartesian convertToCartesian(Polar position);
    Polar convertToPolar(Cartesian position);

    // Process
    
    void process() override;
    
    static ParameterInfo sParamInfo;
};

#endif
