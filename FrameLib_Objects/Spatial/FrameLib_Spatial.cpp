
#include "FrameLib_Spatial.h"
#include "FrameLib_Sorting_Functions.h"

#include <cmath>
#include <cstring>
#include <limits>

// Compile code for building the convex hull, binding to allocation functions below

#define ch_stateful_malloc FrameLib_Spatial::chMalloc
#define ch_stateful_calloc FrameLib_Spatial::chCalloc
#define ch_stateful_realloc FrameLib_Spatial::chRealloc
#define ch_stateful_resize FrameLib_Spatial::chResize
#define ch_stateful_free FrameLib_Spatial::chFree

#define CONVHULL_3D_ENABLE
#define CONVHULL_ALLOW_BUILD_IN_HIGHER_DIM

#include "../../FrameLib_Dependencies/convhull_3d/convhull_3d.h"

// Convex Hull Memory Allocation

FrameLib_Spatial *asSpatial(void *object)
{
    return static_cast<FrameLib_Spatial *>(object);
}

void *FrameLib_Spatial::chMalloc(void *object, size_t size)
{
    return asSpatial(object)->alloc<Byte>(size);
}

void *FrameLib_Spatial::chCalloc(void *object, size_t num, size_t size)
{
    void *ptr = chMalloc(object, num * size);
    std::memset(ptr, 0, num * size);
    return ptr;
}

void *FrameLib_Spatial::chRealloc(void *object, void *ptr, size_t size)
{
    return chResize(object, ptr, size, true);
}

void *FrameLib_Spatial::chResize(void *object, void *ptr, size_t size)
{
    return chResize(object, ptr, size, false);
}

void FrameLib_Spatial::chFree(void *object, void *ptr)
{
    asSpatial(object)->dealloc(ptr);
}

void *FrameLib_Spatial::chResize(void *object, void *ptr, size_t size, bool copy)
{
    size_t currentSize = asSpatial(object)->memorySize(static_cast<Byte *>(ptr));
        
    // No need to reallocate

    if (currentSize >= size)
        return ptr;
        
    // Make a new allocation (overallocate when growing)
    
    void *newPtr = chMalloc(object, std::max(static_cast<size_t>(512), size * 2));
    
    // Copy and free if required
    
    if (copy)
        std::memcpy(newPtr, ptr, std::min(currentSize, size));
    if (ptr)
        chFree(object, ptr);
    
    return newPtr;
}

// Vec3 Type

double FrameLib_Spatial::Vec3::mag() const
{
    return sqrt(dot(*this, *this));
}

FrameLib_Spatial::Vec3& FrameLib_Spatial::Vec3::normalise()
{
    const double m = mag();
    
    return *this = m ? (*this * (1.0 / m)) : Vec3(0, 0, 1);
}

FrameLib_Spatial::Vec3 operator * (const FrameLib_Spatial::Vec3& a, double b)
{
    return { a.x * b, a.y * b,  a.z * b };
}

FrameLib_Spatial::Vec3 operator - (const FrameLib_Spatial::Vec3& a, const FrameLib_Spatial::Vec3& b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

FrameLib_Spatial::Vec3 operator + (const FrameLib_Spatial::Vec3& a, const FrameLib_Spatial::Vec3& b)
{
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}
    
double dot(const FrameLib_Spatial::Vec3& a, const FrameLib_Spatial::Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

FrameLib_Spatial::Vec3 cross(const FrameLib_Spatial::Vec3& v1, const FrameLib_Spatial::Vec3& v2)
{
    FrameLib_Spatial::Vec3 c;
    
    c.x = v1.y * v2.z - v1.z * v2.y;
    c.y = v1.z * v2.x - v1.x * v2.z;
    c.z = v1.x * v2.y - v1.y * v2.x;
    
    return c;
}

// Comparisons with epsilon

static constexpr double epsilon = 1E-8;

static inline bool compareGT(double a, double b)
{
    return a > (b - epsilon);
}

static inline bool compareLT(double a, double b)
{
    return a < (b + epsilon);
}

// Hull Face

FrameLib_Spatial::Vec3 FrameLib_Spatial::HullFace::faceNormal(const Vec3 &a, const Vec3 &b, const Vec3 &c)
{
    Vec3 v1 = b - a;
    Vec3 v2 = c - a;
    
    return cross(v1, v2).normalise();
}

double FrameLib_Spatial::HullFace::toPlane(const Vec3& p)
{
    return dot(p - a, n);
}

std::pair<FrameLib_Spatial::Vec3, double> FrameLib_Spatial::HullFace::closestPoint(const Vec3& p)
{
    auto pointWithDistance = [](const Vec3& x, const Vec3& p)
    {
        return std::pair<Vec3, double>(x, (p - x).mag());
    };
    
    // Based on https://github.com/LLNL/axom/blob/develop/src/axom/primal/operators/closest_point.hpp
    
    const Vec3 ab = b - a;
    const Vec3 ac = c - a;
    const Vec3 ap = p - a;
    const double d1 = dot(ab, ap);
    const double d2 = dot(ac, ap);
    
    // Closest point is a
    
    if (compareLT(d1, 0.0) && compareLT(d2, 0.0))
        return pointWithDistance(a, p);

    const Vec3 bp = p - b;
    const double d3 = dot(ab, bp);
    const double d4 = dot(ac, bp);
    
    // Closest point is b

    if (compareGT(d3, 0.0) && compareLT(d4, d3))
        return pointWithDistance(b, p);

    const double vc = d1 * d4 - d3 * d2;
  
    // Closest point is on ab
    
    if (compareLT(vc, 0.0) && compareGT(d1, 0.0) && compareLT(d3, 0.0))
        return pointWithDistance(a + ab * (d1 / (d1 - d3)), p);

    const Vec3 cp = p - c;
    const double d5 = dot(ab, cp);
    const double d6 = dot(ac, cp);
    
    // Closest point is c
  
    if (compareGT(d6, 0.0) && compareLT(d5, d6))
        return pointWithDistance(c, p);

    const double vb = d5 * d2 - d1 * d6;
  
    // Closest point is on ac
    
    if (compareLT(vb, 0.0) && compareGT(d2, 0.0) && compareLT(d6, 0.0))
        return pointWithDistance(a + ac * (d2 / (d2 - d6)), p);

    const double va = d3 * d6 - d5 * d4;
    
    // Closest point is on bc
    
    if (compareLT(va, 0.0) && compareGT(d4 - d3, 0.0) && compareGT(d5 - d6, 0.0))
        return pointWithDistance(b + (c - b) * ((d4 - d3) / ((d4 - d3) + (d5 - d6))), p);
    
    // Closest point is the projection onto the plane
  
    const double tp = toPlane(p);
    
    return { p - (n * tp), fabs(tp) };
};

// Spatial Constraints

FrameLib_Spatial::Cartesian FrameLib_Spatial::constrain(Cartesian point)
{
    ConstrainModes mode = mParameters.getEnum<ConstrainModes>(kConstrain);
    
    // No Spatial Constraint
    
    if (mode == kNone)
        return point;
        
    // Hemispherical / Spherical Constraint
    
    if (mode == kHemisphere || mode == kSphere)
    {
        if (mode == kHemisphere)
            point.z = std::max(point.z, 0.0);
                
        if (point.mag() <= mRadius)
            return point;
        
        return point.normalise() * mRadius;
    }
    
    // Convex Hull Constraint
    
    // For a single speaker the hull is a single point
    
    if (mSpeakers.size() == 1)
        return mSpeakers[0];
    
    double minDistance = std::numeric_limits<double>::infinity();
    Vec3 constrained = point;
    
    for (unsigned long i = 0; i < mHull.size(); i ++)
    {
        const double toPlane = mHull[i].toPlane(point);
        const double planeDistance = fabs(toPlane);
        const bool inFront = compareGT(toPlane, 0.0);
        
        // If we are not in front of the face, or the distance to the plane is greater than min_distance this face doesn't have the closest point
        
        if (inFront && planeDistance < minDistance)
        {
            auto closest = mHull[i].closestPoint(point);
            
            if (closest.second < minDistance)
            {
                minDistance = closest.second;
                constrained = closest.first;
            }
        }
    }
    
    return constrained;
}

void FrameLib_Spatial::calculateBounds()
{
    auto vertex = [](double x, double y, double z)
    {
        ch_vertex v;
        
        v.x = x;
        v.y = y;
        v.z = z;
        
        return v;
    };
    
    int numSpeakers = static_cast<int>(mSpeakers.size());
    int numVertices = std::max(numSpeakers, 4);
    int *faces = nullptr;
    int numFaces = 0;

    if (numSpeakers > 1)
    {
        // Copy the array into a suitable format

        auto vertices = allocAutoArray<ch_vertex>(numVertices);

        for (size_t i = 0; i < mSpeakers.size(); i++)
            vertices[i] = vertex(mSpeakers[i].x, mSpeakers[i].y, mSpeakers[i].z);
        
        // If we have less than 4 speaers then add synthetic vertices to ensure we can build the hull
        
        if (numSpeakers == 2)
        {
            // Add two equally space points along the line
            
            Vec3 p1 = mSpeakers[0] * (2.0/3.0) + mSpeakers[1] * (1.0/3.0);
            Vec3 p2 = mSpeakers[1] * (2.0/3.0) + mSpeakers[0] * (1.0/3.0);

            vertices[2] = vertex(p1.x, p1.y, p1.z);
            vertices[3] = vertex(p2.x, p2.y, p2.z);
        }
        else if (numSpeakers == 3)
        {
            // Add the centroid of the triangle
            
            Vec3 centroid;
            
            for (size_t i = 0; i < mSpeakers.size(); i++)
                centroid = centroid + (mSpeakers[i] * (1.0/3.0));
            
            vertices[3] = vertex(centroid.x, centroid.y, centroid.z);
        }
        
        // Build the hull and keep a copy with calculated / stored normals
        
        convhull_3d_build_alloc(vertices.data(), numVertices, &faces, &numFaces, this);

        mHull = allocAutoArray<HullFace>(numFaces);
        
        for (int i = 0; i < numFaces; i++)
        {
            auto& va = vertices[faces[i * 3 + 0]];
            auto& vb = vertices[faces[i * 3 + 1]];
            auto& vc = vertices[faces[i * 3 + 2]];
            
            mHull[i] = HullFace(Vec3(va.x, va.y, va.z), Vec3(vb.x, vb.y, vb.z), Vec3(vc.x, vc.y, vc.z));
        }

        // Free the faces returned from convhull
        
        chFree(this, faces);
    }
    else
        mHull = allocAutoArray<HullFace>(0);

    // Calculate the array sphere radius from the origin
    
    mRadius = 0.0;

    for (int i = 0; i < numSpeakers; i++)
        mRadius = std::max(mRadius, mSpeakers[i].mag());
}

// Constructor

FrameLib_Spatial::FrameLib_Spatial(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 1, 1)
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
        
    mParameters.addEnum(kConstrain, "constrain");
    mParameters.addEnumItem(kNone, "none");
    mParameters.addEnumItem(kHemisphere, "hemisphere");
    mParameters.addEnumItem(kSphere, "sphere");
    mParameters.addEnumItem(kConvexHull, "hull", true);
    
    mParameters.set(serialisedParameters);
    
    unsigned long speakerSize;
    const double *speakers = mParameters.getArray(kSpeakers, &speakerSize);
    unsigned long numSpeakers = (speakerSize + 2) / 3;
    
    mSpeakers = allocAutoArray<Cartesian>(numSpeakers);
    
    for (unsigned long i = 0; i < numSpeakers; i++)
    {
        double azimuth = speakerSize > (i * 3 + 0) ? speakers[i * 3 + 0] : 0.0;
        double elevation = speakerSize > (i * 3 + 1) ? speakers[i * 3 + 1] : 0.0;
        double radius = speakerSize > (i * 3 + 2) ? speakers[i * 3 + 2] : 1.0;
        
        mSpeakers[i] = convertToCartesian(Polar(azimuth, elevation, radius));
    }

    calculateBounds();
}

// Info

std::string FrameLib_Spatial::objectInfo(bool verbose)
{
    return formatInfo("Generates multiplication factors for a number of speakers from an input coordinate triple: "
                   "The algorithm used is a modified version of DBAP, with extra features. "
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
    add("Sets the maximum number of speakers to be used (the nearest N speakers will be used only). "
        "If zero all speakers are used.");
    add("Interpolate to point source panning (0 is modified DBAP - 1 is point source).");
    add("Sets the method for constraining positions outside of the speaker array.");
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
    const double *input = getInput(0, &sizeIn);
    
    const double *weights = mParameters.getArray(kWeights, &weightsSize);
    unsigned long numSpeakers = static_cast<unsigned long>(mSpeakers.size());
    unsigned long maxSpeakers = mParameters.getInt(kMaxSpeakers);
    
    double blur = mParameters.getValue(kBlur);
    double rolloff = mParameters.getValue(kRolloff);
    double pointFactor = mParameters.getValue(kPoints);
    
    double rolloffFactor =  rolloff / (20 * log10(2.0));
    double blur2 = blur * blur;
    maxSpeakers = maxSpeakers == 0 ? numSpeakers : maxSpeakers;
    
    requestOutputSize(0, numSpeakers);
    allocateOutputs();
    
    double *output = getOutput(0, &numSpeakers);
    
    double norm = 0.0;
    double minDistance = 0.0;
    unsigned long nearestIdx = 0;
    
    Cartesian panPosition;
    
    if (mParameters.getEnum<InputModes>(kInputMode) == kPolar)
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
        
        panPosition = Cartesian(x, y, z);
    }
    
    // Constrain Position
    
    panPosition = constrain(panPosition);
    
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
        auto indices = allocAutoArray<unsigned long>(numSpeakers);
        sortIndicesDescending(indices, output, numSpeakers);
        
        for (unsigned long i = maxSpeakers; i < numSpeakers; i++)
            output[indices[i]] = 0.0;
    }
    
    // Interpolate to points
    
    if (pointFactor > 0.0)
    {
        for (unsigned long i = 0; i < numSpeakers; i++)
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
    
    norm = sqrt(1.0 / norm);
    
    for (unsigned long i = 0; i < numSpeakers; i++)
        output[i] *= norm;
}
