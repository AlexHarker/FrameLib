
#ifndef FRAMELIB_SPATIAL_H
#define FRAMELIB_SPATIAL_H

#include "FrameLib_DSP.h"
#include "FrameLib_Poltocar.h"

#include <utility>
#include <vector>

class FrameLib_Spatial final : public FrameLib_Processor
{
    using Value3D = FrameLib_Spatial_Ops::Value3D;
    
    // Parameter Enums and Info

    enum ParameterList { kAngleUnits, kOrientation, kInputCoords, kSpeakerCoords, kConstrain, kSpeakers, kWeights, kRolloff, kBlur, kMaxSpeakers, kPointFactor };
    enum AngleUnits { kRadians, kDegrees };
    enum OrientationTypes { kXClockwise, kXAntiClockwise, kYClockwise, kYAntiClockwise };
    enum CoordinateTypes { kSpherical, kCartesian };
    enum ConstrainModes { kOff, kHemisphere, kSphere, kConvexHull };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
    // Spatial Types
    
    struct Vec3 : Value3D
    {
        Vec3(const Value3D &a) : Value3D(a) {}
        Vec3() : Value3D(0.0, 0.0, 0.0) {}
        Vec3(double X, double Y, double Z) : Value3D(X, Y, Z) {}
        
        double mag() const;
        Vec3& normalise();

        const double &x() const { return std::get<0>(*this); }
        const double &y() const { return std::get<1>(*this); }
        const double &z() const { return std::get<2>(*this); }
    };
        
    struct SphericalConversion
    {
        SphericalConversion(OrientationTypes orientation, AngleUnits units)
        : mOrientation(orientation)
        , mConvertor(units == kDegrees)
        {}
        
        Vec3 operator()(const Value3D& spherical)
        {
            Vec3 cartesian = mConvertor(spherical);
            
            switch (mOrientation)
            {
                case kXClockwise:           return Vec3(cartesian.x(), -cartesian.y(), cartesian.z());
                case kYClockwise:           return Vec3(cartesian.y(), cartesian.x(), cartesian.z());
                case kYAntiClockwise:       return Vec3(-cartesian.y(), cartesian.x(), cartesian.z());
                default:                    return cartesian;
            }
        }
        
        OrientationTypes mOrientation;
        FrameLib_Spatial_Ops::PolToCar mConvertor;
    };
    
    struct HullFace
    {
        HullFace() {}
        HullFace(const Vec3& A, const Vec3& B, const Vec3& C)
        : a(A), b(B), c(C), n(faceNormal(A, B, C)) {}
        
        static Vec3 faceNormal(const Vec3& A, const Vec3& B, const Vec3& C);

        double toPlane(const Vec3& p);
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

    // Constraints
    
    Vec3 constrain(Vec3 point);
    void calculateBounds();
   
    // Process
    
    void process() override;
    
    // Data
    
    CoordinateTypes mInputCoords;
    AutoArray<Vec3> mSpeakers;

    static ParameterInfo sParamInfo;
    
    // Constraint Bounds
    
    AutoArray<HullFace> mHull;
    double mRadius;
};

#endif
