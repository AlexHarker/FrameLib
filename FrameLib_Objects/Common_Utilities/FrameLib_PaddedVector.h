
#ifndef FRAMELIB_PADDEDVECTOR_H
#define FRAMELIB_PADDEDVECTOR_H

// A class to temporarily wrap a vector and access with end padding (extending the last value)
// A detault value will be used if the input vector is empty
// The class does not own the memory of the vector

class PaddedVector
{
    
public:
    
    PaddedVector(const double *input, unsigned long size, double defaultValue)
    : mVector(input ? input : &mDefault), mLimit(size ? size - 1 : 0), mDefault(defaultValue)
    {}
    
    double operator[](unsigned long pos) const
    {
        return mVector[std::min(pos, mLimit)];
    }
    
private:
    
    const double *mVector;
    unsigned long mLimit;
    double mDefault;
};

#endif
