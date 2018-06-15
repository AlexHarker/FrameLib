
#ifndef FRAMELIB_TYPES_H
#define FRAMELIB_TYPES_H

// Standard integer types

#include <stdint.h>

// FrameLib_FixedPoint

#include "FrameLib_FixedPoint.h"

// Raw Bytes

typedef unsigned char Byte;
typedef unsigned char *BytePointer;

// Time format

typedef FL_FP FrameLib_TimeFormat;

// Basic Types

enum ObjectType { kOutput, kProcessor, kScheduler };
enum FrameType { kFrameAny, kFrameNormal, kFrameTagged };
enum DataType { kVector, kSingleString };
enum ConnectionResult { kConnectSuccess, kConnectWrongContext, kConnectSelfConnection, kConnectFeedbackDetected, kConnectNoOrderingSupport, kConnectAliased };

// Proxy - FrameLib_Proxy is simply a virtual struct allowing for extensible communication to/from the host environment 

struct FrameLib_Proxy
{
    virtual ~FrameLib_Proxy() {}
};

#endif
