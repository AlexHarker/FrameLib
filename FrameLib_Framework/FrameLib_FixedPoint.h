
#ifndef FRAMELIB_FIXEDPOINT_H
#define FRAMELIB_FIXEDPOINT_H

#include "FrameLib_Types.h"
#include <cmath>
#include <limits>

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

// ************************************************************************************** //

class FL_SoftUInt64
{

public:
    
    FL_SoftUInt64()
    {
        mHi = 0;
        mLo = 0;
    }
    
    FL_SoftUInt64(FL_UInt32 hi, FL_UInt32 lo)
    {
        mHi = hi;
        mLo = lo;
    }
 
    friend bool operator < (const FL_SoftUInt64& lhs, const FL_SoftUInt64& rhs)
    {
        return ((lhs.mHi < rhs.mHi) || (lhs.mHi == rhs.mHi && lhs.mLo < rhs.mLo));
    }
    
    friend bool operator > (const FL_SoftUInt64& lhs, const FL_SoftUInt64& rhs)
    {
        return ((lhs.mHi > rhs.mHi) || (lhs.mHi == rhs.mHi && lhs.mLo > rhs.mLo));
    }

    friend bool operator == (const FL_SoftUInt64& lhs, const FL_SoftUInt64& rhs)
    {
        return (lhs.mHi == rhs.mHi && lhs.mLo == rhs.mLo);
    }
    
    friend bool operator ! ( const FL_SoftUInt64& rhs)
    {
        return !rhs.mHi && !rhs.mLo;
    }
    
    friend FL_SoftUInt64 operator + (const FL_SoftUInt64& lhs, const FL_SoftUInt64& rhs)
    {
        FL_UInt32 hi, lo;
        
        hi = lhs.mHi + rhs.mHi;
        lo = lhs.mLo + rhs.mHi;
        
        hi = (lo < lhs.mLo) ? ++hi : hi;
        
        return FL_SoftUInt64(hi, lo);
    }
    
    friend FL_SoftUInt64 operator - (const FL_SoftUInt64& lhs, const FL_SoftUInt64& rhs)
    {
        FL_UInt32 hi, lo;
        
        hi = lhs.mHi - rhs.mHi;
        lo = lhs.mLo - rhs.mHi;
        
        hi = (lo > lhs.mLo) ? --hi : hi;
        
        return FL_SoftUInt64(hi, lo);
    }
    
    operator double()
    {
        return ((double) mHi * 4294967296.0) + ((double) mLo);
    }
    
    FL_SoftUInt64& operator -- ()
    {
        mHi = (++mLo == 0) ? ++mHi : mHi;
        return *this;
    }

    FL_SoftUInt64& operator ++ (int)
    {
        FL_SoftUInt64& result = *this;
        operator++();
        return result;
    }
    
    FL_SoftUInt64& operator ++ ()
    {
        mHi = (mLo-- == 0) ? --mHi : mHi;
        return *this;
    }
    
    FL_SoftUInt64& operator -- (int)
    {
        FL_SoftUInt64& result = *this;
        operator--();
        return result;
    }
    
    // The double is aassumed to be unsigned here
    
    FL_SoftUInt64& operator = (const double& rhs)
    {
        mLo = (FL_UInt32) (rhs / 4294967296.0);
        mHi = (FL_UInt32) (rhs - (mLo * 4294967296));
        
        return *this;
    }
    
private:
    
    FL_UInt32 mHi;
    FL_UInt32 mLo;
};

// ************************************************************************************** //

template<> struct FL_Limits <FL_SoftUInt64>
{
    static FL_SoftUInt64 largest()
    {
        return FL_SoftUInt64(std::numeric_limits<FL_UInt32>::max(), std::numeric_limits<FL_UInt32>::max());
    }
};

// ************************************************************************************** //

// FrameLib_FixedPoint

// This class provides a very high precision fixed point format for dealing with time

// FIX - add division operator

#ifdef FL_64BIT
typedef FL_UInt64 FL_Internal_UInt64;
#else
typedef FL_SoftUInt64 FL_Internal_UInt64;
#endif

class FrameLib_FixedPoint
{

public:
    
    FrameLib_FixedPoint()
    {
        mInt = FL_Internal_UInt64();
        mFrac = FL_Internal_UInt64();
    }
    
    FrameLib_FixedPoint(FL_Internal_UInt64 intVal, FL_Internal_UInt64 fracVal, bool negFlag = FALSE)
    {
        mInt = intVal;
        mFrac = fracVal;
    }
    
    FrameLib_FixedPoint(double val)
    {
        double absVal = fabs(val);
        
        mInt = absVal;
        mFrac = ((absVal - floor(absVal)) * 18446744073709551616.0);
    }
    
    // Absolute value
    
    friend FrameLib_FixedPoint clipToPositive (const FrameLib_FixedPoint& arg)
    {
        return arg;
    }
    
    // Casting to double
    
    operator double()
    {
        double result = ((double) mInt) + ((double) mFrac / 18446744073709551616.0);
        
        return result;
    }
    
    // Comparison operators
    
public:
    
    friend bool operator ! ( const FrameLib_FixedPoint& rhs)
    {
        return !rhs.mInt && !rhs.mFrac;
    }
    
    friend bool operator < (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        return ((lhs.mInt < rhs.mInt) || (lhs.mInt == rhs.mInt && lhs.mFrac < rhs.mFrac));
    }
    
    friend bool operator > (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        return ((lhs.mInt > rhs.mInt) || (lhs.mInt == rhs.mInt && lhs.mFrac > rhs.mFrac));
    }
    
    friend bool operator <= (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        return !(lhs > rhs);
    }
    
    friend bool operator >= (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        return !(lhs < rhs);
    }
    
    friend bool operator == (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        return (lhs.mInt == rhs.mInt && lhs.mFrac == rhs.mFrac); //&& (lhs.mNeg == rhs.mNeg));
    }
    
private:

    // Single 64 bit add/subtract with carry bit

    static inline bool addWithCarry(FL_Internal_UInt64 *result, const FL_Internal_UInt64& a, const FL_Internal_UInt64& b)
    {
        *result = a + b;
        return (*result < a);
    }
    
    static inline bool subWithCarry(FL_Internal_UInt64 *result, const FL_Internal_UInt64& a, const FL_Internal_UInt64& b)
    {
        *result = a - b;
        return (*result > a);
    }

    // Arithmetic operators (with assignment)

public:
    
    // Addition
    
    friend FrameLib_FixedPoint operator + (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        FL_Internal_UInt64 hi, lo;
        
        hi = lhs.mInt + rhs.mInt;
        hi = addWithCarry(&lo, lhs.mFrac, rhs.mFrac) ? ++hi : hi;
        
        return FrameLib_FixedPoint(hi, lo);
    }

    FrameLib_FixedPoint& operator += (const FrameLib_FixedPoint& rhs)
    {
        *this = *this + rhs;
        return *this;
    }
    
    // Subtraction
    
    friend FrameLib_FixedPoint operator - (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        FL_Internal_UInt64 hi, lo;
      
        hi = lhs.mInt - rhs.mInt;
        hi = subWithCarry(&lo, lhs.mFrac, rhs.mFrac) ? --hi : hi;
        
        return FrameLib_FixedPoint(hi, lo);
    }
    
    FrameLib_FixedPoint& operator -= (const FrameLib_FixedPoint& rhs)
    {
        *this = *this - rhs;
        return *this;
    }
    
    // Multiplication
    /*
    friend FrameLib_FixedPoint operator * (const FrameLib_FixedPoint& lhs, const FrameLib_FixedPoint& rhs)
    {
        // N.B. Overflow behaviour is undefined for multiplication (it is assumed you are using values well within range for the purpose)
        
        FL_UInt64 hi, lo;
        FL_UInt64 w, x, y, z;
        FL_UInt64 c1, c2, c3;
    
        // Split both into 4 x 32 bits stored within 64 bit integers
        
        FL_UInt64 a1 = lhs.mInt >> 0x20;
        FL_UInt64 a2 = lhs.mInt & 0xFFFFFFFF;
        FL_UInt64 a3 = lhs.mFrac >> 0x20;
        FL_UInt64 a4 = lhs.mFrac & 0xFFFFFFFF;
        
        FL_UInt64 b1 = rhs.mInt >> 0x20;
        FL_UInt64 b2 = rhs.mInt & 0xFFFFFFFF;
        FL_UInt64 b3 = rhs.mFrac >> 0x20;
        FL_UInt64 b4 = rhs.mFrac & 0xFFFFFFFF;
        
        // Calculate sums of parallel 64 bit results and associated carry and rounding information
        
        c1 = addWithCarry(&x, a3 * b4, a4 * b3) ? 0x20ULL : 0LL;
        c1 = addWithCarry(&w, a4 * b4, x << 0x20) ? c1++ : c1;
        c1 = (w & 0x8000000000000000) ? c1++ : c1;
        
        c2 = addWithCarry(&y, a3 * b3, a2 * b4) ? 0x1ULL : 0LL;
        c2 = addWithCarry(&y, y, a4 * b2) ? c2++ : c2;
        
        c3 = addWithCarry(&z, a2 * b3, a3 * b2) ? 0x1ULL : 0LL;
        c3 = addWithCarry(&z, y, a1 * b4) ? c3++ : c3;
        c3 = addWithCarry(&z, y, a4 * b1) ? c3++ : c3;
        
        // Calculate fractional part with carry
        
        c2 = addWithCarry(&lo, y, ((z << 0x20) & (x >> 0x20))) ? c2++ : c2;
        c2 = addWithCarry(&lo, lo, c1) ? c2++ : c2;
        
        // Calculate integer part including carry
        
        hi = (lhs.mInt * rhs.mInt) + (a1 * b3) + (a3 * b1) + c2 + ((c3 << 0x20) & (z >> 0x20));
        
        return FrameLib_FixedPoint(hi, lo);//, lhs.mNeg != rhs.mNeg);
    }
    
    FrameLib_FixedPoint& operator *= (const FrameLib_FixedPoint& rhs)
    {
        *this = *this * rhs;
        return *this;
    }*/
    
    // Increment and decrement operatiors
    
    FrameLib_FixedPoint& operator ++ ()
    {
        if (++mFrac == FL_Internal_UInt64())
            ++mInt;
        
        return *this;
    }
   
    FrameLib_FixedPoint& operator ++ (int)
    {
        FrameLib_FixedPoint& result = *this;
        operator++();
        return result;
    }
    
    FrameLib_FixedPoint& operator -- ()
    {
        if (mFrac-- == FL_Internal_UInt64())
            --mInt;
        
        return *this;
    }
    
    FrameLib_FixedPoint& operator -- (int)
    {
        FrameLib_FixedPoint& result = *this;
        operator--();
        return result;
    }
    
    // Support for operators using doubles
    
    FrameLib_FixedPoint& operator = (const double& rhs)
    {
        *this = FrameLib_FixedPoint(rhs);
        return *this;
    }
    
    friend bool operator < (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs < FrameLib_FixedPoint(rhs);
    }
    
    friend bool operator < (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) < rhs;
    }
    
    friend bool operator > (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs > FrameLib_FixedPoint(rhs);
    }
    
    friend bool operator > (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) > rhs;
    }
    
    friend bool operator == (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs == FrameLib_FixedPoint(rhs);
    }
    
    friend bool operator == (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) == rhs;
    }
    
    friend bool operator <= (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs <= FrameLib_FixedPoint(rhs);
    }
    
    friend bool operator <= (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) <= rhs;
    }
    
    friend bool operator >= (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs >= FrameLib_FixedPoint(rhs);
    }
    
    friend bool operator >= (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) >= rhs;
    }
    
    friend FrameLib_FixedPoint operator + (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs + FrameLib_FixedPoint(rhs);
    }
    
    friend FrameLib_FixedPoint operator + (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) + rhs;
    }
    
    FrameLib_FixedPoint& operator += (const double& rhs)
    {
        return operator +=(FrameLib_FixedPoint(rhs));
    }
    
    friend FrameLib_FixedPoint operator - (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs - FrameLib_FixedPoint(rhs);
    }
    
    friend FrameLib_FixedPoint operator - (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) - rhs;
    }
    
    FrameLib_FixedPoint& operator -= (const double& rhs)
    {
        return operator -=(FrameLib_FixedPoint(rhs));
    }
    /*
    friend FrameLib_FixedPoint operator * (const FrameLib_FixedPoint& lhs, const double& rhs)
    {
        return lhs * FrameLib_FixedPoint(rhs);
    }
    
    friend FrameLib_FixedPoint operator * (const double& lhs, const FrameLib_FixedPoint& rhs)
    {
        return FrameLib_FixedPoint(lhs) * rhs;
    }
    
    FrameLib_FixedPoint& operator *= (const double& rhs)
    {
        return operator *=(FrameLib_FixedPoint(rhs));
    }*/
    
private:
    
    FL_Internal_UInt64 mInt;
    FL_Internal_UInt64 mFrac;
};

// ************************************************************************************** //

inline double clipToPositive (double &arg)
{
    return arg < 0 ? 0 : arg;
}

// ************************************************************************************** //

template<> struct FL_Limits <FrameLib_FixedPoint>
{
    static FrameLib_FixedPoint largest()
    {
        return FrameLib_FixedPoint(FL_Limits<FL_Internal_UInt64>::largest(), FL_Limits<FL_Internal_UInt64>::largest());
    }
};

#endif
