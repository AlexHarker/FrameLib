
#ifndef FRAMELIB_NEUMAIERSUM_H
#define FRAMELIB_NEUMAIERSUM_H

class NeumaierSum
{
public:
    
    // Clear
    
    void clear()
    {
        mSum = 0.0;
        mCompensate = 0.0;
    }
    
    double value() const
    {
        return mSum;
    }
    
    // High Precision Sum

    void sum(double x)
    {
        double t = mSum + x;
        
        if (fabs(mSum) >= fabs(x))
            mCompensate += (mSum - t) + x;
        else
            mCompensate += (x - t) + mSum;
        
        mSum = t;
    }
    
private:
    
    double mSum;
    double mCompensate;
};

#endif
