
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

// Object Types

enum ObjectType { kOutput, kProcessor, kScheduler };

#endif
