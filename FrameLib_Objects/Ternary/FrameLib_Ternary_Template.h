
#ifndef FRAMELIB_TERNARY_TEMPLATE_H
#define FRAMELIB_TERNARY_TEMPLATE_H

#include "FrameLib_DSP.h"

template <typename Op> class FrameLib_TernaryOp : public FrameLib_Processor
{
    enum ParameterList { kMismatchMode };
    enum MismatchModes { kWrap, kShrink, kExtend};
    
    class EnlargedInput
    {
    public:
        EnlargedInput(FrameLib_TernaryOp *owner, const double *input, unsigned long size, unsigned long extendedSize,
                      MismatchModes mode) : mOwner(owner),mAllocated(NULL)
        {
            if (extendedSize > size)
            {
                mAllocated = owner->alloc<double>(extendedSize);
                if (mAllocated)
                {
                    switch (mode)
                    {
                        case kWrap:
                        {
                            enlargeModulo(mAllocated, input, size, extendedSize);
                            break;
                        }
                            
                        case kShrink:
                            break;
                            
                        case kExtend:
                            enlargeExtend(mAllocated, input, size, extendedSize);
                            break;
                    }
                    mPtr = mAllocated;
                }
            }
            else
                mPtr=input;
        }
        
        ~EnlargedInput()
        {
            mOwner->dealloc(mAllocated);
        }
        const double &operator [] (size_t idx) { return mPtr[idx]; }

    private:
        void enlargeExtend(double* output, const double *input, unsigned long size, unsigned long extendedSize)
        {
            copyVector(output, input, size);
            std::fill_n(output + size, extendedSize - size, input[size-1]);
        }
        
        void enlargeModulo(double* output, const double *input, unsigned long size, unsigned long extendedSize)
        {
            unsigned long leftover = extendedSize % size;
            
            for (unsigned long i = 0; i < (extendedSize-leftover); i+=size)
                copyVector(output + i, input, size);
            
            copyVector(output + (extendedSize-leftover), input, leftover);
        }
        
        
        // Deleted
        
        EnlargedInput(const EnlargedInput&);
        EnlargedInput& operator=(const EnlargedInput&);
        
        // Data
        
        FrameLib_TernaryOp *mOwner;
        double *mAllocated;
        const double *mPtr;
    };
    
    struct ParameterInfo : public FrameLib_Parameters::Info
    {
        ParameterInfo()
        {
            add("Sets the mode used when dealing with mismatched input lengths: "
                "wrap -   smaller right inputs are read modulo against larger left input. "
                "shrink - the output length is set to the size of the smaller input. "
                "extend - smaller right inputs are extended by repeating their final value");
        }
    };
    

public:
    FrameLib_TernaryOp(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, getParameterInfo(), 3, 1)
    {
        mParameters.addEnum(kMismatchMode, "mismatch");
        mParameters.addEnumItem(kWrap, "wrap");
        mParameters.addEnumItem(kShrink, "shrink");
        mParameters.addEnumItem(kExtend, "extend");
        
        mParameters.set(serialisedParameters);
        
        mMismatchMode = static_cast<MismatchModes>(mParameters.getInt(kMismatchMode));
    }
    
    
    std::string objectInfo(bool verbose)
    {
        return formatInfo("#: Calculation is performed on triplets of values in turn. The result is an output frame as long as the left most frame, presumed to be a signal input."
                          "When frames mismatch in size the result depends on the setting of the mismatch parameter.",
                          "#.", getDescriptionString(), verbose);
    }


    std::string inputInfo(unsigned long idx, bool verbose)
    {
        switch (idx)
        {
            case 0: return "Input";
            case 1: return "Parameter 1";
            case 2: return "Parameter 2";
            default: return "Unknown input";
        }
    }

    std::string outputInfo(unsigned long idx, bool verbose)     { return "Result"; }
private:
    void process()
    {
        MismatchModes mode = mMismatchMode;
        Op op;
        
        unsigned long sizeIn[3];
        unsigned long sizeOut;
        
        const double *input1 = getInput(0, &sizeIn[0]);
        const double *input2 = getInput(1, &sizeIn[1]);
        const double *input3 = getInput(2, &sizeIn[2]);

        unsigned long sizeMax = *std::max_element(sizeIn, sizeIn+3);
        unsigned long sizeMin = *std::min_element(sizeIn, sizeIn+3);

        for (int i = 0; i < 3; i++)
            if (sizeIn[i] == 0)
                return;
            
        
        // Not a real ternary op: sizeOut always = sizeIn1;
        
        switch (mode)
        {
            case kShrink:
            {
                sizeOut = sizeMin;
                break;
            }
            case kWrap:
            case kExtend:
                sizeOut = sizeMax;
        }
        
        requestOutputSize(0,sizeOut);
        allocateOutputs();
        double *output = getOutput(0, &sizeOut);

        if (!sizeOut)
            return;
        
        for (unsigned long i = 0; i < sizeMin; i++)
        {
            output[i] = op(input1[i],input2[i],input3[i]);
        }
        

        if (mode == kShrink)
            return;
        
        EnlargedInput in1(this, input1, sizeIn[0], sizeMax, mode);
        EnlargedInput in2(this, input2, sizeIn[1], sizeMax, mode);
        EnlargedInput in3(this, input3, sizeIn[2], sizeMax, mode);
        
        for (unsigned long i = sizeMin; i < sizeMax; i++)
        {
            output[i] = op(in1[i], in2[i],in3[i]);
        }
    }
    
    
    virtual const char *getDescriptionString() { return "Ternary Operator - No operator info available"; }
    
    ParameterInfo *getParameterInfo()
    {
        static ParameterInfo info;
        return &info;
    }
    
    MismatchModes mMismatchMode;
};


template <double func(double, double, double)> struct Ternary_Functor
{
    double operator()(double x, double y, double z) { return func(x, y, z); }
};

template <double func(double, double, double)> class FrameLib_Ternary : public FrameLib_TernaryOp<Ternary_Functor<func> >
{
    
public:
    
    // Constructor
    
    FrameLib_Ternary(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner)
    : FrameLib_TernaryOp<Ternary_Functor<func> > (context, serialisedParameters, owner) {}
    
private:
    
    // Description (specialise/override to change description)
    
    virtual const char *getDescriptionString() { return "Ternary Operator - No operator info available"; }
};

#endif
