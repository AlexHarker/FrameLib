
#ifndef FRAMELIB_FIXEDPOINT_H
#define FRAMELIB_FIXEDPOINT_H

#include <stdint.h>
#include <cmath>
#include <limits>

// ************************************************************************************** //

// FL_SP

// This is a minimal class for super precision that aids the calculation of division in fixed point (giving added precision where needed)

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

// ************************************************************************************** //

// FL_FP

// This class provides a very high precision fixed point format for dealing with time

class FL_FP
{

public:
    
    // Constructors
    
    FL_FP() : mInt(0U), mFrac(0U) {}
    FL_FP(uint64_t a, uint64_t b) : mInt(a), mFrac(b) {}
    FL_FP(const FL_SP& val);
    FL_FP(const double& val);
    
    // Int and Fract
    
    uint64_t intVal()   { return mInt; }
    uint64_t fracVal()  { return mFrac; }
    
    // Absolute value
    
    friend FL_FP clipToPositive(const FL_FP& arg)               { return arg; }
    
    // Comparison operators (N.B. - it is faster to avoid branching using bit rather logical operators)

    friend bool operator == (const FL_FP& a, const FL_FP& b)    { return (a.mInt == b.mInt & a.mFrac == b.mFrac); }
    
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
   
    FL_FP& operator ++ (int)
    {
        FL_FP& result = *this;
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

    uint64_t mInt;
    uint64_t mFrac;
};

// ************************************************************************************** //

// Numeric Limits

template <class T> struct FL_Limits
{
    // N.B. there is basically no good value for smallest for a floating point unit, because all values will fail at some point before total overflow
    
    static T smallest()    { return std::numeric_limits<T>::epsilon() * 65536.0; }
    
    static T largest()
    {
        if (std::numeric_limits<T>::has_infinity)
            return std::numeric_limits<T>::infinity();
        else
            return std::numeric_limits<T>::max();
    }
};

template<> struct FL_Limits <FL_FP>
{
    static FL_FP smallest() { return FL_FP(0,1); }
    static FL_FP largest() { return FL_FP(FL_Limits<uint64_t>::largest(), FL_Limits<uint64_t>::largest()); }
};

// Double Helper Utility

inline double clipToPositive(double &arg)  {return arg < 0 ? 0 : arg; }

#endif
