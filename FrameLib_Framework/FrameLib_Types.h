
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

// Time Format

typedef FL_FP FrameLib_TimeFormat;

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
