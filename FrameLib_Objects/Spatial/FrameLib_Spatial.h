
#ifndef FRAMELIB_SPATIAL_H
#define FRAMELIB_SPATIAL_H

#include "FrameLib_DSP.h"
#include <vector>

class FrameLib_Spatial final : public FrameLib_Processor
{
    // Spatial Types
    
    struct Vec3
    {
        Vec3() : x(0.0), y(0.0), z(0.0) {}
        Vec3(double X, double Y, double Z) : x(X), y(Y), z(Z) {}
        
        double mag() const;
        Vec3& normalise();

        double x, y, z;
    };

    friend Vec3 operator *(const Vec3& a, double b);
    friend Vec3 operator -(const Vec3& a, const Vec3& b);
    friend Vec3 operator +(const Vec3& a, const Vec3& b);
        
    friend double dot(const Vec3& a, const Vec3& b);
    friend Vec3 cross(const Vec3& v1, const Vec3& v2);
    
    using Cartesian = Vec3;
    
    struct Polar
    {
        Polar() {}
        Polar(double a, double e, double r) : azimuth(a), elevation(e), radius(r) {}
        
        double azimuth, elevation, radius;
    };
    
    // Parameter Enums and Info

    enum ParameterList { kInputMode, kSpeakers, kWeights, kRolloff, kBlur, kMaxSpeakers, kPoints, kConstrain };
    enum InputModes { kPolar, kCartesian };
    enum ConstrainModes { kNone, kHemisphere, kSphere, kConvexHull };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

    class ConstrainPoint
    {
        struct HullFace
        {
            HullFace() {}
            HullFace(const Vec3& a, const Vec3& b, const Vec3& c)
            : p1(a), p2(b), p3(c), n(faceNormal(a, b, c)) {}
            
            static Vec3 faceNormal(const Vec3& a, const Vec3& b, const Vec3& c);
            
            Vec3 p1;
            Vec3 p2;
            Vec3 p3;
            Vec3 n;
        };

        struct Hull
        {
            AutoArray<HullFace> mFaces;
            Vec3 mCentroid;
        };

    public:
        
        Cartesian operator()(Cartesian point);
        
        void setArray(FrameLib_Spatial& object, const std::vector<Cartesian>& array);
        void setMode(ConstrainModes mode) { mMode = mode; }

    private:
        
        bool triangleTest(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& n);
        bool pointProjectsInTriangle(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& n);
        
        Hull mHull;
        Cartesian mOrigin;
        double mRadius;
        
        ConstrainModes mMode;
    };
    
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
    
    ConstrainPoint mConstainer;
};

#endif
