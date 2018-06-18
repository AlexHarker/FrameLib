
#ifndef FRAMELIB_TYPES_H
#define FRAMELIB_TYPES_H

// FrameLib_FixedPoint

#include "FrameLib_FixedPoint.h"

// Standard integer types

#include <stdint.h>

// STL includes

#include <memory>
#include <vector>

// Raw Bytes

typedef unsigned char Byte;
typedef unsigned char *BytePointer;


/**
 
 \struct FrameLib_TimeFormat
 
 \brief a type for representing time in fixed-point high-precision for scheduling purposes.
 
 */

struct FrameLib_TimeFormat : public FL_FP
{
    FrameLib_TimeFormat() : FL_FP() {}
    FrameLib_TimeFormat(const FL_FP& a) : FL_FP(a) {}
    FrameLib_TimeFormat(uint64_t a, uint64_t b) : FL_FP(a, b) {}
    FrameLib_TimeFormat(const double& val) : FL_FP(val) {}
    
    static FrameLib_TimeFormat smallest() { return FL_FP(0,1); }
    static FrameLib_TimeFormat largest()  { return FL_FP(std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::max()); }

    bool greaterThanZero()  { return intVal() | fracVal(); }
};

// Basic Enum Types

enum ObjectType { kOutput, kProcessor, kScheduler };
enum FrameType { kFrameAny, kFrameNormal, kFrameTagged };
enum DataType { kVector, kSingleString };
enum ConnectionResult { kConnectSuccess, kConnectWrongContext, kConnectSelfConnection, kConnectFeedbackDetected, kConnectNoOrderingSupport, kConnectAliased };

// Proxy - FrameLib_Proxy is simply a virtual struct allowing for extensible communication to/from the host environment 

struct FrameLib_Proxy
{
    virtual ~FrameLib_Proxy() {}
};

// OwnedList - FrameLib_OwnedList is a convenience wrapper for dealing with a vector of objects owned by pointer

template<class T>
struct FrameLib_OwnedList : public std::vector<std::unique_ptr<T>>
{
    void add(T *object)
    {
        std::vector<std::unique_ptr<T>>::push_back(std::unique_ptr<T>(object));
    }
};

#endif
