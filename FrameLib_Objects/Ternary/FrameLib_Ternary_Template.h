
#ifndef FRAMELIB_TERNARY_TEMPLATE_H
#define FRAMELIB_TERNARY_TEMPLATE_H

#include "FrameLib_DSP.h"

template <typename Op> class FrameLib_TernaryOp final : public FrameLib_Processor
{
    enum ParameterList { kMismatchMode };
    enum MismatchModes { kWrap, kShrink, kExtend};
    
    class EnlargedInput
    {
        
    public:
        
        EnlargedInput(FrameLib_TernaryOp *owner, const double *input, unsigned long size, unsigned long extendedSize, MismatchModes mode)
            : mOwner(owner), mAllocated(nullptr)
        {
            if (extendedSize > size)
            {
                if ((mPtr = mAllocated = owner->alloc<double>(extendedSize)))
                {
                    switch (mode)
                    {
                        case kWrap:     copyVectorWrap(mAllocated, input, extendedSize, size);      break;
                        case kShrink:                                                               break;
                        case kExtend:   copyVectorExtend(mAllocated, input, extendedSize, size);    break;
                    }
                }
            }
            else
                mPtr = input;
        }
        
        ~EnlargedInput()
        {
            mOwner->dealloc(mAllocated);
        }
        
        bool isValid() const { return mPtr; }
        
        const double &operator [] (size_t idx) const { return mPtr[idx]; }

    private:
        
        // Deleted
        
        EnlargedInput(const EnlargedInput&) = delete;
        EnlargedInput& operator=(const EnlargedInput&) = delete;
        
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
    FrameLib_TernaryOp(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, getParameterInfo(), 3, 1)
    {
        mParameters.addEnum(kMismatchMode, "mismatch");
        mParameters.addEnumItem(kWrap, "wrap");
        mParameters.addEnumItem(kShrink, "shrink");
        mParameters.addEnumItem(kExtend, "extend");
        mParameters.setInstantiation();

        mParameters.set(serialisedParameters);
        
        mMismatchMode = static_cast<MismatchModes>(mParameters.getInt(kMismatchMode));
    }
    
    std::string objectInfo(bool verbose) override
    {
        return formatInfo("#: Calculation is performed on triplets of values in turn. The output is a frame at least as long as the left most frame."
                          "When frames mismatch in size the result depends on the setting of the mismatch parameter.",
                          "#.", getDescriptionString(), verbose);
    }

    std::string inputInfo(unsigned long idx, bool verbose) override
    {
        switch (idx)
        {
            case 0: return "Input";
            case 1: return "Parameter 1";
            case 2: return "Parameter 2";
            default: return "Unknown input";
        }
    }

    std::string outputInfo(unsigned long idx, bool verbose) override
    {
        return "Result";
    }
    
private:
    
    void process() override
    {
        MismatchModes mode = mMismatchMode;
        Op op;
        
        unsigned long sizeIn[3];
        unsigned long sizeOut;
        
        const double *input1 = getInput(0, &sizeIn[0]);
        const double *input2 = getInput(1, &sizeIn[1]);
        const double *input3 = getInput(2, &sizeIn[2]);

        unsigned long sizeMax = *std::max_element(sizeIn, sizeIn + 3);
        unsigned long sizeMin = *std::min_element(sizeIn, sizeIn + 3);

        if (sizeMin == 0)
            return;
                
        switch (mode)
        {
            case kShrink:
                sizeOut = sizeMin;
                break;

            case kWrap:
            case kExtend:
                sizeOut = sizeMax;
        }
        
        requestOutputSize(0,sizeOut);
        allocateOutputs();
        double *output = getOutput(0, &sizeOut);

        if (!sizeOut)
            return;
        
        if (mode == kShrink || sizeMin == sizeMax)
        {
            for (unsigned long i = 0; i < sizeMin; i++)
                output[i] = op(input1[i], input2[i], input3[i]);
        }
        else
        {
            EnlargedInput in1(this, input1, sizeIn[0], sizeMax, mode);
            EnlargedInput in2(this, input2, sizeIn[1], sizeMax, mode);
            EnlargedInput in3(this, input3, sizeIn[2], sizeMax, mode);
        
            if (in1.isValid() && in2.isValid() && in3.isValid())
            {
                for (unsigned long i = 0; i < sizeMax; i++)
                    output[i] = op(in1[i], in2[i], in3[i]);
            }
            else
                zeroVector(output, sizeMax);
        }
    }
    
    const char *getDescriptionString() { return "Ternary Operator - No operator info available"; }
    
    ParameterInfo *getParameterInfo()
    {
        static ParameterInfo info;
        return &info;
    }
    
    MismatchModes mMismatchMode;
};

// Ternary Functor

template<double func(double, double, double)>
struct Ternary_Functor
{
    double operator()(double x, double y, double z) { return func(x, y, z); }
};

// Ternary (Function Version)

template<double func(double, double, double)>
using  FrameLib_Ternary = FrameLib_TernaryOp<Ternary_Functor<func>>;

#endif
