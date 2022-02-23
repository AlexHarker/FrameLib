
#ifndef FRAMELIB_SPATIAL_H
#define FRAMELIB_SPATIAL_H

#include "FrameLib_DSP.h"

#include <utility>
#include <vector>

class FrameLib_Spatial final : public FrameLib_Processor
{
    // Parameter Enums and Info

    enum ParameterList { kInputMode, kSpeakers, kWeights, kRolloff, kBlur, kMaxSpeakers, kPoints, kConstrain };
    enum InputModes { kPolar, kCartesian };
    enum ConstrainModes { kNone, kHemisphere, kSphere, kConvexHull };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
    // Spatial Types
    
    struct Vec3
    {
        Vec3() : x(0.0), y(0.0), z(0.0) {}
        Vec3(double X, double Y, double Z) : x(X), y(Y), z(Z) {}
        
        double mag() const;
        Vec3& normalise();

        double x, y, z;
    };
    
    using Cartesian = Vec3;
    
    struct Polar
    {
        Polar() {}
        Polar(double a, double e, double r) : azimuth(a), elevation(e), radius(r) {}
        
        double azimuth, elevation, radius;
    };
    
    struct HullFace
    {
        HullFace() {}
        HullFace(const Vec3& A, const Vec3& B, const Vec3& C)
        : a(A), b(B), c(C), n(faceNormal(A, B, C)) {}
        
        static Vec3 faceNormal(const Vec3& A, const Vec3& B, const Vec3& C);

        double distance(const Vec3& p);
        std::pair<Vec3, double> closestPoint(const Vec3& p);

        Vec3 a;
        Vec3 b;
        Vec3 c;
        Vec3 n;
    };
    
    // Vec 3 Functions
    
    friend Vec3 operator *(const Vec3& a, double b);
    friend Vec3 operator -(const Vec3& a, const Vec3& b);
    friend Vec3 operator +(const Vec3& a, const Vec3& b);
        
    friend double dot(const Vec3& a, const Vec3& b);
    friend Vec3 cross(const Vec3& v1, const Vec3& v2);
    
public:

    // Public static methods for memory allocation for convhull_3d
    
    static void *chMalloc(void *object, size_t size);
    static void *chCalloc(void *object, size_t num, size_t size);
    static void *chRealloc(void *object, void *ptr, size_t size);
    static void *chResize(void *object, void *ptr, size_t size);
    static void chFree(void *object, void *ptr);

    // Joint resize/realloc method
    
    static void *chResize(void *object, void *ptr, size_t size, bool copy);
    
    // Constructor
    
    FrameLib_Spatial(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;
    
private:

    // Conversion Helper
    
    Cartesian convertToCartesian(Polar position);
    
    // Constraints
    
    Cartesian constrain(Cartesian point);
    void calculateBounds();
   
    // Process
    
    void process() override;
    
    // Data
    
    AutoArray<Cartesian> mSpeakers;

    static ParameterInfo sParamInfo;
    
    // Constraint Bounds
    
    AutoArray<HullFace> mHull;
    double mRadius;
};

#endif
