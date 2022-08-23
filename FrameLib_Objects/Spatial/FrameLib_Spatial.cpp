
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
    return { a.x() * b, a.y() * b,  a.z() * b };
}

FrameLib_Spatial::Vec3 operator - (const FrameLib_Spatial::Vec3& a, const FrameLib_Spatial::Vec3& b)
{
    return { a.x() - b.x(), a.y() - b.y(), a.z() - b.z() };
}

FrameLib_Spatial::Vec3 operator + (const FrameLib_Spatial::Vec3& a, const FrameLib_Spatial::Vec3& b)
{
    return { a.x() + b.x(), a.y() + b.y(), a.z() + b.z() };
}
    
double dot(const FrameLib_Spatial::Vec3& a, const FrameLib_Spatial::Vec3& b)
{
    return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

FrameLib_Spatial::Vec3 cross(const FrameLib_Spatial::Vec3& v1, const FrameLib_Spatial::Vec3& v2)
{
    const double x = v1.y() * v2.z() - v1.z() * v2.y();
    const double y = v1.z() * v2.x() - v1.x() * v2.z();
    const double z = v1.x() * v2.y() - v1.y() * v2.x();
    
    return FrameLib_Spatial::Vec3(x, y, z);
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

FrameLib_Spatial::Vec3 FrameLib_Spatial::constrain(Vec3 point)
{
    ConstrainModes mode = mParameters.getEnum<ConstrainModes>(kConstrain);
    
    // No Spatial Constraint
    
    if (mode == kOff)
        return point;
        
    // Hemispherical / Spherical Constraint
    
    if (mode == kHemisphere || mode == kSphere)
    {
        if (mode == kHemisphere)
            point = Vec3(point.x(), point.y(), std::max(point.z(), 0.0));
                
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
            vertices[i] = vertex(mSpeakers[i].x(), mSpeakers[i].y(), mSpeakers[i].z());
        
        // If we have less than 4 speakers then add synthetic vertices to ensure we can build the hull
        
        if (numSpeakers == 2)
        {
            // Add two equally space points along the line
            
            Vec3 p1 = mSpeakers[0] * (2.0/3.0) + mSpeakers[1] * (1.0/3.0);
            Vec3 p2 = mSpeakers[1] * (2.0/3.0) + mSpeakers[0] * (1.0/3.0);

            vertices[2] = vertex(p1.x(), p1.y(), p1.z());
            vertices[3] = vertex(p2.x(), p2.y(), p2.z());
        }
        else if (numSpeakers == 3)
        {
            // Add the centroid of the triangle
            
            Vec3 centroid;
            
            for (size_t i = 0; i < mSpeakers.size(); i++)
                centroid = centroid + (mSpeakers[i] * (1.0/3.0));
            
            vertices[3] = vertex(centroid.x(), centroid.y(), centroid.z());
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
: FrameLib_Processor(context, proxy, &sParamInfo, 3, 0)
{
    mParameters.addEnum(kAngleUnits, "angles", 0);
    mParameters.addEnumItem(kRadians, "radians");
    mParameters.addEnumItem(kDegrees, "degrees", true);
    
    mParameters.addEnum(kOrientation, "orientation", 1);
    mParameters.addEnumItem(kXClockwise, "x_clockwise");
    mParameters.addEnumItem(kXAntiClockwise, "x_anticlockwise");
    mParameters.addEnumItem(kYClockwise, "y_clockwise", true);
    mParameters.addEnumItem(kYAntiClockwise, "y_anticlockwise");
    
    mParameters.addEnum(kInputCoords, "input_coords", 2);
    mParameters.addEnumItem(kSpherical, "spherical");
    mParameters.addEnumItem(kCartesian, "cartesian");
    mParameters.setInstantiation();

    mParameters.addEnum(kSpeakerCoords, "speaker_coords", 3);
    mParameters.addEnumItem(kSpherical, "spherical");
    mParameters.addEnumItem(kCartesian, "cartesian");
    mParameters.setInstantiation();

    mParameters.addEnum(kConstrain, "constrain", 4);
    mParameters.addEnumItem(kOff, "off");
    mParameters.addEnumItem(kHemisphere, "hemisphere");
    mParameters.addEnumItem(kSphere, "sphere");
    mParameters.addEnumItem(kConvexHull, "hull", true);
    
    mParameters.addVariableDoubleArray(kSpeakers, "speakers", 0.0, 360, 0);
    mParameters.setInstantiation();
    
    mParameters.addVariableDoubleArray(kWeights, "weights", 1.0, 120, 0);
    mParameters.setMin(0.0);

    mParameters.addDouble(kRolloff, "rolloff", 6.0);
    mParameters.setMin(0.0000001);
    mParameters.addDouble(kBlur, "blur", 0.001);
    mParameters.setMin(0.0);
    mParameters.addInt(kMaxSpeakers, "max_speakers", 0);
    mParameters.setMin(0);
    mParameters.addDouble(kPointFactor, "point_factor", 0.0);
    mParameters.setClip(0.0, 1.0);
    
    mParameters.set(serialisedParameters);
    
    unsigned long speakerSize;
    const double *speakers = mParameters.getArray(kSpeakers, &speakerSize);
    unsigned long numSpeakers = std::max(2UL, (speakerSize + 2) / 3);
    
    mSpeakers = allocAutoArray<Vec3>(numSpeakers);
    
    if (mParameters.getEnum<CoordinateTypes>(kSpeakerCoords) == kSpherical)
    {
        SphericalConversion convertor(mParameters.getEnum<OrientationTypes>(kOrientation), mParameters.getEnum<AngleUnits>(kAngleUnits));

        for (unsigned long i = 0; i < numSpeakers; i++)
        {
            double r = speakerSize > (i * 3 + 0) ? speakers[i * 3 + 0] : 0.0;
            double a = speakerSize > (i * 3 + 1) ? speakers[i * 3 + 1] : 0.0;
            double e = speakerSize > (i * 3 + 2) ? speakers[i * 3 + 2] : 0.0;
        
            
            mSpeakers[i] = convertor( { r, a, e } );
        }
    }
    else
    {
        for (unsigned long i = 0; i < numSpeakers; i++)
        {
            double x = speakerSize > (i * 3 + 0) ? speakers[i * 3 + 0] : 0.0;
            double y = speakerSize > (i * 3 + 1) ? speakers[i * 3 + 1] : 0.0;
            double z = speakerSize > (i * 3 + 2) ? speakers[i * 3 + 2] : 0.0;
            
            mSpeakers[i] = { x, y, z };
        }
    }

    calculateBounds();
    
    setIO(3, mSpeakers.size());
    
    mInputCoords = mParameters.getEnum<CoordinateTypes>(kInputCoords);
    
    addParameterInput();
}

// Info

std::string FrameLib_Spatial::objectInfo(bool verbose)
{
    return formatInfo("Generates multiplication factors for a set of speakers positioned in 3D: "
                      "The core underlying algorithm is DBAP (distance-based amplitude panning). "
                      "Input may use cartesian [x, y, z] or spherical [radius, azimuth, elevation] coordinates. "
                      "Angles may be set using degrees or radians. "
                      "2D operation can be achieved by setting all z or elevation values to zero. "
                      "Missing values at any input are assumed to be zero. "
                      "Note that the orientation of the spherical coordinates can be adjusted as required. "
                      "The output size is equal to that of the longest input.",
                      "Generates multiplication factors for a set of speakers positioned in 3D.", verbose);
}

std::string FrameLib_Spatial::inputInfo(unsigned long idx, bool verbose)
{
    bool cartesian = mInputCoords == kCartesian;
    
    switch (idx)
    {
        case 0:     return cartesian ? "X Values" : "Radius Values";
        case 1:     return cartesian ? "Y Values" : "Azimuth Values";
        case 2:     return cartesian ? "Z Values" : "Elevation Values";
    }
        
    return parameterInputInfo(verbose);
}

std::string FrameLib_Spatial::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Speaker # Coefficients - multiplication factor.", "Speaker # Coefficients", idx, verbose);
}

// Parameter Info

FrameLib_Spatial::ParameterInfo FrameLib_Spatial::sParamInfo;

FrameLib_Spatial::ParameterInfo::ParameterInfo()
{
    add("Sets the units used for angles.");
    add("Sets the orientation for spherical coordinates. "
        "This determines how azimuth is interpreted: "
        "x_clockwise - zero is the positive x axis / positive angles move clockwise. "
        "x_anticlockwise - zero is the positive x axis / positive angles move anticlockwise. "
        "y_clockwise - zero is the positive y axis / positive angles move clockwise. "
        "y_anticlockwise - zero is the positive y axis / positive angles move anticlockwise.");
    add("Sets the coordinate system used for input.");
    add("Sets the coordinate system used for speaker positions.");
    add("Sets the method for constraining positions outside of the speaker array: "
        "off - suitable when the input is already within the speaker array (most efficient). "
        "hemisphere - restrict to the upper hemisphere of the sphere described below. "
        "sphere - restrict to the smallest sphere centred at the origin containing all speakers. "
        "hull - restrict to the convex hull of the speaker array (least efficient / always correct).");
    add("Sets the speaker positions in triples (one triple per speaker). "
        "For cartesian coordinates the values are [x, z, y]. "
        "For spherical coordinates the values are [radius, azimuth, elevation].");
    add("Sets the speaker weightings (one value per speaker). "
        "By default all speakers are weighted equally. "
        "Using weighting values allows the emphasis or exclusion of specific speakers.");
    add("Sets the rolloff in positive dB for a doubling of distance between source and speakers. "
        "The default (6dB) equates to the inverse distance law for sound propagating in a free field. "
        "Lower values may be suitable for simulating more closed environments.");
    add("Sets the blur factor. "
        "This should be set in relation to the dimensions of the speaker array. "
        "Larger numbers result in more blurring of the spatial image.");
    add("Sets the number of speakers that can be non-zero (only the nearest N speakers are used). "
        "If this is set to zero then all speakers are used. "
        "Reducing the number of speakers used can improve localisation.");
    add("Interpolation factor between DBAP (at 0) and point source panning (at 1).");
    
}

// Process

void FrameLib_Spatial::process()
{
    unsigned long sizeIn1, sizeIn2, sizeIn3, sizeOut, weightsSize;
    
    const double *weights = mParameters.getArray(kWeights, &weightsSize);
    unsigned long numSpeakers = static_cast<unsigned long>(mSpeakers.size());
    unsigned long maxSpeakers = mParameters.getInt(kMaxSpeakers);
    
    const double blur = mParameters.getValue(kBlur);
    const double rolloff = mParameters.getValue(kRolloff);
    const double pointFactor = mParameters.getValue(kPointFactor);
    
    const double rolloffFactor = rolloff / (20 * std::log10(2.0));
    const double blur2 = blur * blur;
    
    maxSpeakers = maxSpeakers == 0 ? numSpeakers : maxSpeakers;

    const double *input1 = getInput(0, &sizeIn1);
    const double *input2 = getInput(1, &sizeIn2);
    const double *input3 = getInput(2, &sizeIn3);
    
    unsigned long sizeIn = std::max(sizeIn1, std::max(sizeIn2, sizeIn3));
    
    // Take max of input lengths
    
    for (unsigned long i = 0; i < numSpeakers; i++)
        requestOutputSize(i, sizeIn);
    
    if (!allocateOutputs())
        return;
    
    // Allocate temporary vector of size
    
    auto speakerCoeffs = allocAutoArray<double>(numSpeakers);
    
    auto spatialise = [&](double *coefficients, Vec3 position)
    {
        double minDistance = 0.0;
        double norm = 0.0;
        
        unsigned long nearestIdx = 0;
        
        // Constrain Position
        
        position = constrain(position);
        
        // DBAP
        
        for (unsigned long i = 0; i < numSpeakers; i++)
        {
            double xDelta = position.x() - mSpeakers[i].x();
            double yDelta = position.y() - mSpeakers[i].y();
            double zDelta = position.z() - mSpeakers[i].z();
            
            double sqDistance = xDelta * xDelta + yDelta * yDelta + zDelta * zDelta;
            double weight = i < weightsSize ? weights[i] : 1.0;
            
            coefficients[i] = weight / std::pow(sqDistance + blur2, 0.5 * rolloffFactor);
            
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
            sortIndicesDescending(indices, coefficients, numSpeakers);
            
            for (unsigned long i = maxSpeakers; i < numSpeakers; i++)
                coefficients[indices[i]] = 0.0;
        }
        
        // Interpolate to points
        
        if (pointFactor > 0.0)
        {
            for (unsigned long i = 0; i < numSpeakers; i++)
            {
                if (i == nearestIdx)
                    coefficients[i] += pointFactor * (1.0 - coefficients[i]);
                else
                    coefficients[i] -= pointFactor * coefficients[i];
            }
        }
        
        // Normalise
        
        for (unsigned long i = 0; i < numSpeakers; i++)
            norm += coefficients[i] * coefficients[i];
        
        norm = sqrt(1.0 / norm);
        
        for (unsigned long i = 0; i < numSpeakers; i++)
            coefficients[i] *= norm;
    };
            
    if (mInputCoords == kSpherical)
    {
        SphericalConversion convertor(mParameters.getEnum<OrientationTypes>(kOrientation), mParameters.getEnum<AngleUnits>(kAngleUnits));

        for (unsigned long i = 0; i < sizeIn; i++)
        {
            double r = sizeIn1 > i ? input1[i] : 0.0;
            double a = sizeIn2 > i ? input2[i] : 0.0;
            double e = sizeIn3 > i ? input3[i] : 0.0;
        
            spatialise(speakerCoeffs, convertor( { r, a, e } ));
            
            for (unsigned long j = 0; j < numSpeakers; j++)
                getOutput(j, &sizeOut)[i] = speakerCoeffs[j];
        }
    }
    else
    {
        for (unsigned long i = 0; i < sizeIn; i++)
        {
            double x = sizeIn1 > i ? input1[i] : 0.0;
            double y = sizeIn2 > i ? input2[i] : 0.0;
            double z = sizeIn3 > i ? input3[i] : 0.0;
        
            spatialise(speakerCoeffs, Vec3(x, y, z));
            
            for (unsigned long j = 0; j < numSpeakers; j++)
                getOutput(j, &sizeOut)[i] = speakerCoeffs[j];
        }
    }
}
