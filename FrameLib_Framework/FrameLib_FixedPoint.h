
#ifndef FRAMELIB_FIXEDPOINT_H
#define FRAMELIB_FIXEDPOINT_H

#include "FrameLib_Types.h"
#include <cmath>
#include <limits>

// FIX - This needs to be altered to cope with platforms other than windows/mac and compilers other than visual studio and GCC

#ifdef __APPLE__
#if __LP64__
#define FL_64BIT
#endif
#else
#ifdef _WIN64
#define FL_64BIT
#endif
#endif

// ************************************************************************************** //

class SUInt64
{

public:
    
    // Constructors
    
    SUInt64() : mHi(0), mLo(0) {}
    SUInt64(const uint32_t& lo)  : mHi(0), mLo(lo) {}
    SUInt64(const uint32_t& hi, const uint32_t& lo)  : mHi(hi), mLo(lo) {}
 
    SUInt64(const double& b)
    {
        double temp = fabs(b);
        mLo = (uint32_t) (temp / 4294967296.0);
        mHi = (uint32_t) (temp - (mLo * 4294967296));
    }
    
    // Comparisions

    friend bool operator == (const SUInt64& a, const SUInt64& b)    { return (a.mHi == b.mHi && a.mLo == b.mLo);}
    friend bool operator < (const SUInt64& a, const SUInt64& b)     { return ((a.mHi < b.mHi) || (a.mHi == b.mHi && a.mLo < b.mLo));}
    friend bool operator > (const SUInt64& a, const SUInt64& b)     { return ((a.mHi > b.mHi) || (a.mHi == b.mHi && a.mLo > b.mLo)); }
    
    // Not Zero
    
    friend bool operator !  ( const SUInt64& b)                     { return !b.mHi && !b.mLo; }
    
    // Bit Operations
    
    friend SUInt64 operator | (const SUInt64& a, const SUInt64& b)  { return SUInt64(a.mHi | b.mHi, a.mLo | b.mLo); }
    friend SUInt64 operator & (const SUInt64& a, const SUInt64& b)  { return SUInt64(a.mHi & b.mHi, a.mLo & b.mLo); }

    // Addition and Subtraction
    
    friend SUInt64 operator + (const SUInt64& a, const SUInt64& b)
    {
        uint32_t hi = a.mHi + b.mHi;
        uint32_t lo = a.mLo + b.mHi;
        
        return SUInt64((lo < a.mLo) ? ++hi : hi, lo);
    }
    
    friend SUInt64 operator - (const SUInt64& a, const SUInt64& b)
    {
        uint32_t hi = a.mHi - b.mHi;
        uint32_t lo = a.mLo - b.mHi;
        
        return SUInt64((lo > a.mLo) ? --hi : hi, lo);
    }
    
    // Multiplication
    
    friend SUInt64 operator * (const SUInt64& a, const SUInt64& b)
    {
        // FIX - requires implementation for software emulation of 64bit int
        
        return SUInt64();
    }
    // Addition with Assignment
    
    SUInt64& operator += (const SUInt64& b)
    {
        *this = *this + b;
        return *this;
    }
    
    // Increment/Decrement
    
    SUInt64& operator -- ()
    {
        mHi = (++mLo == 0) ? ++mHi : mHi;
        return *this;
    }

    SUInt64& operator ++ (int)
    {
        SUInt64& result = *this;
        operator++();
        return result;
    }
    
    SUInt64& operator ++ ()
    {
        mHi = (mLo-- == 0) ? --mHi : mHi;
        return *this;
    }
    
    SUInt64& operator -- (int)
    {
        SUInt64& result = *this;
        operator--();
        return result;
    }
    
    // Conversion

    operator double() const     { return ((double) mHi * 4294967296.0) + ((double) mLo); }
    
    SUInt64& operator = (const double& b)
    {
        *this = SUInt64(b);
        return *this;
    }
    
    // Bit Manipulation
    
    friend SUInt64 lo32Bits(SUInt64 a)                      { return SUInt64(0, a.mLo); }
    friend SUInt64 hi32Bits(SUInt64 a)                      { return SUInt64(0, a.mHi); }
    friend SUInt64 joinBits(SUInt64 hi, SUInt64 lo)         { return SUInt64(hi.mLo, lo.mHi); }
    friend SUInt64 highBits(SUInt64 a)                      { return SUInt64(a.mLo, 0);  }
    friend bool checkHighBit(SUInt64 a)                     { return a & SUInt64(0x80000000U, 0); }
    
private:
    
    uint32_t mHi;
    uint32_t mLo;
};

// ************************************************************************************** //

//#ifdef FL_64BIT
typedef uint64_t FUInt64;
//#else
//typedef SUInt64 FUInt64;
//#endif

// ************************************************************************************** //

struct FL_SP
{
    // Constructors
    
    FL_SP() {}
    FL_SP(FUInt64 a, FUInt64 b, FUInt64 c) : mInt(a), mFracHi(b), mFracLo(c) {}
    
    // Get Components
    
    FUInt64 intVal() const       { return mInt; }
    FUInt64 fracHiVal() const    { return mFracHi; }
    FUInt64 fracLoVal() const    { return mFracLo; }
    
    // Necessary Operations
    
    friend FL_SP qMul (const FL_SP& a, const FUInt64 &intVal, const FUInt64 &fracVal);
    friend FL_SP operator * (const FL_SP& a, const FL_SP& b);
    friend FL_SP operator - (const FL_SP& a, const FL_SP& b);
    friend FL_SP twoMinus(const FL_SP& b);
    
    FL_SP& operator *= (const FL_SP& b)
    {
        *this = *this * b;
        return *this;
    }
    
    friend bool operator > (const FL_SP& a, const FL_SP& b)
    {
        return ((a.mInt > b.mInt) || (a.mInt == b.mInt && a.mFracHi > b.mFracHi) || (a.mInt == b.mInt && a.mFracHi == b.mFracHi && a.mFracLo > b.mFracLo));
    }
    
private:
    
    FUInt64 mInt;
    FUInt64 mFracHi;
    FUInt64 mFracLo;
};

// ************************************************************************************** //

// FL_FP

// This class provides a very high precision fixed point format for dealing with time

class FL_FP
{

public:
    
    // Constructors
    
    FL_FP() {}
    FL_FP(FUInt64 a, FUInt64 b) : mInt(a), mFrac(b) {}
    FL_FP(double val);
    
    // Int and Fract
    
    FUInt64 intVal()   { return mInt; }
    FUInt64 fracVal()  { return mFrac; }
    
    // Absolute value
    
    friend FL_FP clipToPositive(const FL_FP& arg)               { return arg; }
    
    // Comparison operators

    friend bool operator == (const FL_FP& a, const FL_FP& b)    { return (a.mInt == b.mInt && a.mFrac == b.mFrac); }
    
    friend bool operator < (const FL_FP& a, const FL_FP& b)     { return ((a.mInt < b.mInt) || (a.mInt == b.mInt && a.mFrac < b.mFrac)); }
    friend bool operator > (const FL_FP& a, const FL_FP& b)     { return ((a.mInt > b.mInt) || (a.mInt == b.mInt && a.mFrac > b.mFrac)); }
    friend bool operator <= (const FL_FP& a, const FL_FP& b)    { return !(a > b); }
    friend bool operator >= (const FL_FP& a, const FL_FP& b)    { return !(a < b); }
    
    // Not zero
    
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
        if (++mFrac == FUInt64())
            ++mInt;
        
        return *this;
    }
   
    FL_FP& operator ++ (int)
    {
        FL_FP& result = *this;
        operator++();
        return result;
    }
    
    FL_FP& operator -- ()
    {
        if (mFrac-- == FUInt64())
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

    // Arithmetic Operators with Assignmnet

    FL_FP& operator += (const double& b)                        { return operator +=(FL_FP(b)); }
    FL_FP& operator -= (const double& b)                        { return operator -=(FL_FP(b)); }
    FL_FP& operator *= (const double& b)                        { return operator *=(FL_FP(b)); }
    FL_FP& operator /= (const double& b)                        { return operator /=(FL_FP(b)); }
    
private:

    FUInt64 mInt;
    FUInt64 mFrac;
};

// ************************************************************************************** //

// Numeric Limits

template <class T> struct FL_Limits
{
    static T largest()
    {
        if (std::numeric_limits<T>::has_infinity)
            return std::numeric_limits<T>::infinity();
        else
            return std::numeric_limits<T>::max();
    }
};

template<> struct FL_Limits <SUInt64>
{
    static SUInt64 largest() { return SUInt64(std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max()); }
};

template<> struct FL_Limits <FL_FP>
{
    static FL_FP largest() { return FL_FP(FL_Limits<FUInt64>::largest(), FL_Limits<FUInt64>::largest()); }
};

// Double Helper Utility

inline double clipToPositive (double &arg)  {return arg < 0 ? 0 : arg; }

#endif
