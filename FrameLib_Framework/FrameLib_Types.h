
#ifndef FRAMELIB_TYPES_H
#define FRAMELIB_TYPES_H

/**
 
 @defgroup Utility
 
 */

// FrameLib_FixedPoint

#include "FrameLib_FixedPoint.h"

// STL includes - including standard types

#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>

/**
  
 @brief A constexpr value representing the framework version.
 
 This can be used to ensure that wersions that might be compiled into different dynamic libraries are compatible at runtime.
 
 The version is stored as a decimal with major version changes as multiples of 1000
 
 */

static constexpr uint32_t FrameLib_FrameworkVersion = 1000;

// Raw Bytes

using Byte = unsigned char;
using BytePointer = Byte *;

/**
 
 @struct FrameLib_TimeFormat
 
 @brief a type for representing time in fixed-point high-precision for scheduling purposes.
 
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

enum class ObjectType { Output, Processor, Scheduler };
enum class FrameType { Any, Vector, Tagged };
enum class DataType { Vector, String };
enum class ConnectionResult { Success, WrongContext, SelfConnection, FeedbackDetected, NoOrderingSupport, Aliased };
enum class ErrorSource { Object, Parameter, Memory, DSP  };

/**
 
 @struct FrameLib_Proxy
 
 @brief a class allowing for extensible communication to/from the host environment.
 
 This class is used to facilitate host environment communication with FrameLib objects. On construction each FrameLib_Object takes a pointer to a proxy which may be a nullptr or a valid pointer to a type inheriting from FrameLib_Proxy. By default the object simply stores a void pointer which can be used by inheriting types to store pointers to owning objects (thus allowing the owning object to be retrieved from FrameLib classes). This is stored without type in order to allow usage across different dynamic libraries if needed (where dynamic_cast is not 100% guaranteed to match across independent but identical type definitions).
 
 More importantly, specific objects (such as FrameLib_ToHost, FrameLib_FromHost and FrameLib_Read) make use of FrameLib_Proxy to allow custom host-specific functionality. In these cases such classes extend FrameLib_Proxy into abstract interfaces which a host can later implement. Pointers to the proxy are dynamically cast to the desired type in order to ensure that host support has been provided. FrameLib_Proxy should always be inherited as a virtual in order to allow a single type to multiply inherit different functionaility.
 */

class FrameLib_Proxy
{
public:
    
    /** A virtual destructor */
    
    virtual ~FrameLib_Proxy() {}
    
    /** A method for setting the owner
     
     @param owner a pointer to the owning object.

     */
    
    void setOwner(void *owner) { mOwner = owner; }
    
    /** A template method for retrieving the owner with a given type */

    template <class T>
    T *getOwner() const { return static_cast<T *>(mOwner); }
    
private:
    
    void *mOwner;
};

/**
 
 @class FrameLib_OwnedList

 @ingroup Utility
 
 @brief a convenience wrapper for dealing with a vector of objects owned by pointer.
 
 This minimal template class inherits from a std::vector of std::unique_ptr<T> objects. The owned list of objects is thus memory-managed as-per std::unique_ptr. For convenience the add() method allows a raw pointer to be added to the end of the vector (and thus ownership transferred) in a compact manner.

 */

template<class T>
struct FrameLib_OwnedList : public std::vector<std::unique_ptr<T>>
{    
    /** Add a pointer to the list of managed pointers, transferring ownership.
     
     @param object the pointer to add to the owned list.
     
     */
    
    void add(T *object)
    {
        std::vector<std::unique_ptr<T>>::push_back(std::unique_ptr<T>(object));
    }
};

#endif
