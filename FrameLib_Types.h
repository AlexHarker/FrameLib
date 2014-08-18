
#ifndef FRAMELIB_TYPES_H
#define FRAMELIB_TYPES_H

// This needs to be altered to cope with platforms other than windows/mac and compilers other than visual studio and GCC

#ifdef __APPLE__
#if __LP64__
#define FL_64BIT
#endif
#else
#ifdef _WIN64
#define FL_64BIT
#endif
#endif

// Pointer-sized Integer definitions

#ifdef FL_64BIT
typedef unsigned long long FL_UIntPtr;
typedef long long FL_SIntPtr;
#else
typedef unsigned long FL_UIntPtr;
typedef long FL_SIntPtr;
#endif

// Integer definitions for given bit sizes

typedef unsigned long long FL_UInt64;
typedef long long FL_SInt64;
typedef unsigned int FL_UInt32;
typedef int FL_SInt32;
typedef unsigned short FL_UInt16;
typedef short FL_SInt16;
typedef unsigned char FL_UInt8;
typedef char FL_SInt8;

// Bytes pointer

typedef char *BytePointer;

// Time formats

#include "FrameLib_FixedPoint.h"

typedef double FrameLib_TimeFormat;
//typedef FrameLib_FixedPoint FrameLib_TimeFormat;

#endif
