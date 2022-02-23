
#include "FrameLib_Spatial.h"
#include "FrameLib_Sorting_Functions.h"

#include <cmath>
#include <cstring>
#include <limits>

#define ch_stateful_malloc FrameLib_Spatial::chMalloc
#define ch_stateful_calloc FrameLib_Spatial::chCalloc
#define ch_stateful_realloc FrameLib_Spatial::chRealloc
#define ch_stateful_resize FrameLib_Spatial::chResize
#define ch_stateful_free FrameLib_Spatial::chFree
#define CONVHULL_3D_ENABLE
#include "../../FrameLib_Dependencies/convhull_3d.h"

// Memory Allocation

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
    
    void *newPtr = chMalloc(object, std::max(512UL, size * 2));
    
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

// Hull Face

FrameLib_Spatial::Vec3 FrameLib_Spatial::ConstrainPoint::HullFace::faceNormal(const Vec3 &a, const Vec3 &b, const Vec3 &c)
{
    Vec3 v1 = b - a;
    Vec3 v2 = c - a;
    
    return cross(v1, v2).normalise();
}

// Spatial Constraints

bool FrameLib_Spatial::ConstrainPoint::triangleTest(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& n)
{
    return dot(cross(b - a, p - a), n) < 0;
}

bool FrameLib_Spatial::ConstrainPoint::vertexTest(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c)
{
    const Vec3 ab(a - b);
    const Vec3 ac(a - c);
    const Vec3 ap(a - p);
    
    const double d1 = dot(ab, ap);
    const double d2 = dot(ac, ap);
   
    return d1 < 0.0 && d2 < 0.0;
}

bool FrameLib_Spatial::ConstrainPoint::pointProjectsInTriangle(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& n)
{
    return !(triangleTest(p, a, b, n) || triangleTest(p, b, c, n) || triangleTest(p, c, a, n));
}

FrameLib_Spatial::Cartesian FrameLib_Spatial::ConstrainPoint::operator()(Cartesian point, ConstrainModes mode)
{
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
    
    double min_distance = std::numeric_limits<double>::infinity();
    Vec3 constrained = point;
    
    for (unsigned long i = 0; i < mHull.size(); i ++)
    {
        const Vec3& a = mHull[i].p1;
        const Vec3& b = mHull[i].p2;
        const Vec3& c = mHull[i].p3;
        const Vec3& n = mHull[i].n;
        
        const double distance = dot(point - a, n);
        const bool inFront = distance > 0.0;
        
        // If we are not in front of the face, or the distance to the plane is greater than min_distance this face doesn't have the closest point
        
        if (inFront && distance < min_distance)
        {
            // If we are above the triangle then use that value, otherwise the nearest point is an edge or vertex
            
            if (pointProjectsInTriangle(point, a, b, c, n))
            {
                min_distance = distance;
                constrained = point - (n * distance);
            }
            else
            {
                Vec3 closest = Vec3(10000,10000,10000);
                
                // HERE WE NEED TO FIND CLOSEST POINT ON TRIANGLE EDGE...
                
                // Test to see if we are neareset to a vertex
                
                if (vertexTest(point, a, b, c))
                    closest = a;
                else if (vertexTest(point, b, c, a))
                    closest = b;
                else if (vertexTest(point, c, a, b))
                    closest = c;
                else
                {
                    // Edge tests...
                }
                
                const double point_distance = (point - closest).mag();
                
                if (point_distance < min_distance)
                {
                    min_distance = point_distance;
                    constrained = closest;
                }
            }
        }
    }
    
    return constrained;
}

void FrameLib_Spatial::ConstrainPoint::setArray(FrameLib_Spatial& object, const AutoArray<Cartesian>& array)
{
    int numSpeakers = static_cast<int>(array.size());
    int *faces = nullptr;
    int numFaces = 0;
    
    // Copy the array into a suitable format
    
    auto vertices = object.allocAutoArray<ch_vertex>(array.size() + 1);

    for (size_t i = 0; i < array.size(); i++)
        vertices[i] = { array[i].x, array[i].y, array[i].z };
    
    // Add a vertex at the origin
                 
    vertices[array.size()] = { 0.0, 0.0, 0.0 };
    
    // Build the hull and keep a copy with calculated / stored normals
    
    convhull_3d_build_alloc(vertices.data(), numSpeakers, &faces, &numFaces, &object);

    mHull = object.allocAutoArray<HullFace>(numFaces);
    
    for (int i = 0; i < numFaces; i++)
        mHull[i] = HullFace(array[faces[i * 3 + 0]], array[faces[i * 3 + 1]], array[faces[i * 3 + 2]]);

    // Free the faces returned from convhull
    
    chFree(&object, faces);

    // Calculate the array sphere radius from the origin
    
    mRadius = 0.0;

    for (int i = 0; i < numSpeakers; i++)
        mRadius = std::max(mRadius, array[i].mag());
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

    mConstainer.setArray(*this, mSpeakers);
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
    add("Sets the maximum number of speakers to be used (the neaarest N speakers will be used only). "
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
    
    panPosition = mConstainer(panPosition, mParameters.getEnum<ConstrainModes>(kConstrain));
    
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
