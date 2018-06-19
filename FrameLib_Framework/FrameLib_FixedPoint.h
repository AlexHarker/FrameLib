
#ifndef FRAMELIB_FIXEDPOINT_H
#define FRAMELIB_FIXEDPOINT_H

#include <stdint.h>
#include <cmath>
#include <limits>

/**
 
 @defgroup FixedPoint Fixed-Point
 
 */

/**
 
 @struct FL_SP
 
 @ingroup FixedPoint
 
 @brief a minimal class for "super precision" fixed-point calculations where required.
 
 This unsigned type allows for 64 bits of integer precision and 128 bits of fractional precision.
 
 */

struct FL_SP
{
    // Constructors
    
    FL_SP() {}
    FL_SP(uint64_t a, uint64_t b, uint64_t c) : mInt(a), mFracHi(b), mFracLo(c) {}
    
    // Get Components
    
    uint64_t intVal() const       { return mInt; }
    uint64_t fracHiVal() const    { return mFracHi; }
    uint64_t fracLoVal() const    { return mFracLo; }
    
    // Necessary Operations
    
    friend FL_SP qMul(const FL_SP& a, const uint64_t& intVal, const uint64_t& fracVal);
    friend FL_SP operator * (const FL_SP& a, const FL_SP& b);
    friend FL_SP twoMinus(const FL_SP& b);
    
private:
    
    uint64_t mInt;
    uint64_t mFracHi;
    uint64_t mFracLo;
};


/**
 
 @class FL_FP
 
 @ingroup FixedPoint

 @brief  high-precision unsigned fixed-point numerical format.
 
 This unsigned type allows for 64 bits of integer precision and 64 bits of fractional precision. Basic arithmetic and comparison operators are supported, for this type, and when used in conjunction with double-precision floating-point numbers. The primary purpose of this type is to precisely represent the time in samples in FrameLib.
 
 */

class FL_FP
{

public:
    
    // Constructors
    
    FL_FP() : mInt(0U), mFrac(0U) {}
    FL_FP(uint64_t a, uint64_t b) : mInt(a), mFrac(b) {}
    FL_FP(const FL_SP& val);
    FL_FP(const double& val);
    
    // Int and Fract
    
    uint64_t intVal() const   { return mInt; }
    uint64_t fracVal() const  { return mFrac; }
    
    // Comparison operators (N.B. - it is faster to avoid branching using bit rather logical operators)

    friend bool operator == (const FL_FP& a, const FL_FP& b)    { return (a.mInt == b.mInt & a.mFrac == b.mFrac); }
    friend bool operator != (const FL_FP& a, const FL_FP& b)    { return !(a == b); }

    friend bool operator < (const FL_FP& a, const FL_FP& b)     { return ((a.mInt < b.mInt) | (a.mInt == b.mInt & a.mFrac < b.mFrac)); }
    friend bool operator > (const FL_FP& a, const FL_FP& b)     { return ((a.mInt > b.mInt) | (a.mInt == b.mInt & a.mFrac > b.mFrac)); }
    friend bool operator <= (const FL_FP& a, const FL_FP& b)    { return !(a > b); }
    friend bool operator >= (const FL_FP& a, const FL_FP& b)    { return !(a < b); }
    
    // Zero Test
    
    friend bool operator ! ( const FL_FP& b)                    { return !b.mInt && !b.mFrac; }
    
    // Arithmetic Operators
    
    friend FL_FP operator + (const FL_FP& a, const FL_FP& b);
    friend FL_FP operator - (const FL_FP& a, const FL_FP& b);
    friend FL_FP operator * (const FL_FP& a, const FL_FP& b);
    friend FL_FP operator / (const FL_FP& a, const FL_FP& b);
    
    // Arithmetic Operators with Assignment
    
    FL_FP& operator += (const FL_FP& b)
    {
        *this = *this + b;
        return *this;
    }
    
    FL_FP& operator -= (const FL_FP& b)
    {
        *this = *this - b;
        return *this;
    }
    
    FL_FP& operator *= (const FL_FP& b)
    {
        *this = *this * b;
        return *this;
    }
    
    FL_FP& operator /= (const FL_FP& b)
    {
        *this = *this / b;
        return *this;
    }
    
    // Increment / Decrement Operatiors
    
    FL_FP& operator ++ ()
    {
        if (++mFrac == uint64_t(0U))
            ++mInt;
        
        return *this;
    }
   
    FL_FP operator ++ (int)
    {
        FL_FP result = *this;
        operator++();
        return result;
    }
    
    FL_FP& operator -- ()
    {
        if (mFrac-- == uint64_t(0U))
            --mInt;
        
        return *this;
    }
    
    FL_FP& operator -- (int)
    {
        FL_FP& result = *this;
        operator--();
        return result;
    }
    
    // Support for operators using doubles
    
    // Conversion
    
    operator double() const     { return ((double) mInt) + ((double) mFrac / 18446744073709551616.0); }

    FL_FP& operator = (const double& a)
    {
        *this = FL_FP(a);
        return *this;
    }
    
    // Comparison
   
    friend bool operator == (const FL_FP& a, const double& b)   { return a == FL_FP(b); }
    friend bool operator == (const double& a, const FL_FP& b)   { return FL_FP(a) == b; }
    friend bool operator != (const FL_FP& a, const double& b)   { return a != FL_FP(b); }
    friend bool operator != (const double& a, const FL_FP& b)   { return FL_FP(a) != b; }
    
    friend bool operator < (const FL_FP& a, const double& b)    { return a < FL_FP(b); }
    friend bool operator < (const double& a, const FL_FP& b)    { return FL_FP(a) < b; }
    friend bool operator > (const FL_FP& a, const double& b)    { return a > FL_FP(b); }
    friend bool operator > (const double& a, const FL_FP& b)    { return FL_FP(a) > b; }
    friend bool operator <= (const FL_FP& a, const double& b)   { return a <= FL_FP(b); }
    friend bool operator <= (const double& a, const FL_FP& b)   { return FL_FP(a) <= b; }
    friend bool operator >= (const FL_FP& a, const double& b)   { return a >= FL_FP(b); }
    friend bool operator >= (const double& a, const FL_FP& b)   { return FL_FP(a) >= b; }
    
    // Arithmetic Operators
    
    friend FL_FP operator + (const FL_FP& a, const double& b)   { return a + FL_FP(b); }
    friend FL_FP operator + (const double& a, const FL_FP& b)   { return FL_FP(a) + b; }
    friend FL_FP operator - (const FL_FP& a, const double& b)   { return a - FL_FP(b); }
    friend FL_FP operator - (const double& a, const FL_FP& b)   { return FL_FP(a) - b; }
    friend FL_FP operator * (const FL_FP& a, const double& b)   { return a * FL_FP(b); }
    friend FL_FP operator * (const double& a, const FL_FP& b)   { return FL_FP(a) * b; }
    friend FL_FP operator / (const FL_FP& a, const double& b)   { return a / FL_FP(b); }
    friend FL_FP operator / (const double& a, const FL_FP& b)   { return FL_FP(a) / b; }

    // Arithmetic Operators with Assignment

    FL_FP& operator += (const double& b)                        { return operator +=(FL_FP(b)); }
    FL_FP& operator -= (const double& b)                        { return operator -=(FL_FP(b)); }
    FL_FP& operator *= (const double& b)                        { return operator *=(FL_FP(b)); }
    FL_FP& operator /= (const double& b)                        { return operator /=(FL_FP(b)); }
    
private:

    uint64_t mInt;
    uint64_t mFrac;
};

#endif
