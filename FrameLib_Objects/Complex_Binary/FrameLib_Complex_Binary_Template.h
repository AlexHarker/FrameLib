
#ifndef FRAMELIB_COMPLEX_BINARY_TEMPLATE_H
#define FRAMELIB_COMPLEX_BINARY_TEMPLATE_H

#include <complex>
#include "FrameLib_DSP.h"

// OPT - vectorise where appropriate

// Complex Binary Operator

template <typename Op> class FrameLib_Complex_BinaryOp : public FrameLib_Processor
{
    class PaddedInput
    {

    public:
        
        PaddedInput(FrameLib_Complex_BinaryOp *owner, const double *input, unsigned long size, unsigned long paddedSize)
        : mOwner(owner), mAllocated(NULL)
        {
            if (paddedSize > size)
            {
                mAllocated = owner->alloc<double>(paddedSize);
                if (mAllocated)
                {
                    copyVector(mAllocated, input, size);
                    zeroVector(mAllocated + size, paddedSize - size);
                }
                mPtr = mAllocated;
            }
            else
                mPtr = input;
        }
        
        ~PaddedInput()
        {
            mOwner->dealloc(mAllocated);
        }
        
        const double &operator [](size_t idx) { return mPtr[idx]; }
        
    private:
        
        // Deleted
        
        PaddedInput(const PaddedInput&);
        PaddedInput& operator=(const PaddedInput&);
        
        // Data
        
        FrameLib_Complex_BinaryOp *mOwner;
        double *mAllocated;
        const double *mPtr;
    };
    
    // Parameter Enums and Info
    
    struct ParameterInfo : public FrameLib_Parameters::Info
    {
        ParameterInfo()
        {
            add("Sets the mode used when dealing with mismatched input lengths: "
                "wrap - the smaller input is read modulo against the larger input. "
                "shrink - the output length is set to the size of the smaller input. "
                "pad_in - the smaller input is padded prior to calculation to match the size of the larger input. "
                "pad_out - the output is padded to match the size of the larger input.");
            add("Sets which inputs trigger output.");
            add("Sets the complex value used for padding (for either pad_in or pad_out modes).");
        }
    };
    
    enum ParameterList { kMismatchMode, kTriggers, kPadding };
    enum MismatchModes { kWrap, kShrink, kPadIn, kPadOut };
    enum TriggerModes { kBoth, kLeft, kRight };
    
public:
    
    // Constructor
    
    FrameLib_Complex_BinaryOp(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, owner, getParameterInfo(), 4, 2)
    {
        mParameters.addEnum(kMismatchMode, "mismatch");
        mParameters.addEnumItem(kWrap, "wrap");
        mParameters.addEnumItem(kShrink, "shrink");
        mParameters.addEnumItem(kPadIn, "pad_in");
        mParameters.addEnumItem(kPadOut, "pad_out");
        
        mParameters.addEnum(kTriggers, "trigger_ins");
        mParameters.addEnumItem(kBoth, "both");
        mParameters.addEnumItem(kLeft, "left");
        mParameters.addEnumItem(kRight, "right");

        mParameters.addDoubleArray(kPadding, "pad", 0.0, 2);
        
        mParameters.set(serialisedParameters);
                                    
        mMismatchMode = static_cast<MismatchModes>(mParameters.getInt(kMismatchMode));
        mPadValue = mParameters.getValue(kPadding);
        
        TriggerModes triggers = (TriggerModes) mParameters.getInt(kTriggers);
        
        if (triggers == kLeft)
        {
            setInputMode(2, false, false, false);
            setInputMode(3, false, false, false);
        }
        if (triggers == kRight)
        {
            setInputMode(0, false, false, false);
            setInputMode(1, false, false, false);
        }
    }
    
    // Info
    
    std::string objectInfo(bool verbose)
    {
        return formatInfo("#: Calculation is performed on pairs of complex values in turn (with each complex value split across two inputs). If there is a mismatch between sizes within each complex pair (within a single operand) the shorter input is padded with zeros before calculation and the longer size used). The result is two output frames at least as long as the smaller of the two input pairs. "
                       "When complex pairs of frames (left and right operands) mismatch in size the result depends on the setting of the mismatch parameter. Either or both pairs of inputs may be set to trigger output.",
                       "#.", getDescriptionString(), verbose);
    }
        
    std::string inputInfo(unsigned long idx, bool verbose)
    {
        if (idx == 0)
            return formatInfo("Left Real Input", "Left Real Input", verbose);
        else if (idx == 1)
            return formatInfo("Left Imaginary Input", "Left Imag Input", verbose);
        else if (idx == 2)
            return formatInfo("Right Real Input", "Right Real Input", verbose);
        else
            return formatInfo("Right Imaginary Input", "Right Imag Input", verbose);
    }
    
    std::string outputInfo(unsigned long idx, bool verbose)
    {
        if (idx == 0)
            return formatInfo("Real Result", "Real Result", verbose);
        else
            return formatInfo("Imaginary Result", "Imag Result", verbose);
    }

private:
    
    // Calculate Split
    
    void calculate(double &rOut, double &iOut, const double &rIn1, const double &iIn1, const double &rIn2, const double &iIn2, Op &op)
    {
        std::complex<double> result = op(std::complex<double>(rIn1, iIn1), std::complex<double>(rIn2, iIn2));
        rOut = result.real();
        iOut = result.imag();
    }
    
    // Process
    
    void process()
    {
        MismatchModes mode = mMismatchMode;
        Op op;
        
        unsigned long sizeIn1R, sizeIn1I, sizeIn2R, sizeIn2I, sizeOut;
        
        const double *input1R = getInput(0, &sizeIn1R);
        const double *input1I = getInput(1, &sizeIn1I);
        const double *input2R = getInput(2, &sizeIn2R);
        const double *input2I = getInput(3, &sizeIn2I);
        
        double defaultValueR = mPadValue;
        double defaultValueI = mPadValue;
        
        unsigned long sizeIn1 = std::max(sizeIn1R, sizeIn1I);
        unsigned long sizeIn2 = std::max(sizeIn2R, sizeIn2I);
        
        // Get common size
        
        unsigned long sizeCommon = sizeIn1 < sizeIn2 ? sizeIn1 : sizeIn2;
        
        // Calculate output size by mode
        
        switch (mode)
        {
            case kShrink:
                sizeOut = sizeCommon;
                break;
            default:
                sizeOut = sizeIn1 > sizeIn2 ? sizeIn1 : sizeIn2;
                if (mode == kWrap)
                    sizeOut = sizeIn1 && sizeIn2 ? sizeOut : 0;
                break;
        }
        
        // Allocate output
        
        requestOutputSize(0, sizeOut);
        requestOutputSize(1, sizeOut);
        allocateOutputs();
        double *outputR = getOutput(0, &sizeOut);
        double *outputI = getOutput(1, &sizeOut);
        sizeCommon = sizeCommon > sizeOut ? sizeOut : sizeCommon;
        
        PaddedInput in1R(this, input1R, sizeIn1R, sizeIn1);
        PaddedInput in1I(this, input1I, sizeIn1I, sizeIn1);
        PaddedInput in2R(this, input2R, sizeIn2R, sizeIn2);
        PaddedInput in2I(this, input2I, sizeIn2I, sizeIn2);
        
        if (!sizeOut)
            return;
        
        // Do first part
        
        for (unsigned long i = 0; i < sizeCommon; i++)
            calculate(outputR[i], outputI[i], in1R[i], in1I[i], in2R[i], in2I[i], op);

        // Clean up if sizes don't match
        
        if (sizeIn1 != sizeIn2)
        {
            switch (mode)
            {
                case kShrink:
                    break;
                
                case kWrap:
                
                    if (sizeIn1 > sizeIn2)
                    {
                       if (sizeIn2 == 1)
                       {
                           double valueR = in2R[0];
                           double valueI = in2I[0];
                           for (unsigned long i = 1; i < sizeOut; i++)
                               calculate(outputR[i], outputI[i], in1R[i], in1I[i], valueR, valueI, op);
                       }
                       else
                       {
                           for (unsigned long i = sizeCommon; i < sizeOut;)
                               for (unsigned long j = 0; j < sizeIn2 && i < sizeOut; i++, j++)
                                   calculate(outputR[i], outputI[i], in1R[i], in1I[i], in2R[j], in2I[j], op);
                       }
                    }
                    else
                    {
                        if (sizeIn1 == 1)
                        {
                            double valueR = in1R[0];
                            double valueI = in1I[0];
                            for (unsigned long i = 1; i < sizeOut; i++)
                                calculate(outputR[i], outputI[i], valueR, valueI, in2R[i], in2I[i], op);
                        }
                        else
                        {
                            for (unsigned long i = sizeCommon; i < sizeOut;)
                                for (unsigned long j = 0; j < sizeIn1 && i < sizeOut; i++, j++)
                                    calculate(outputR[i], outputI[i], in1R[j], in1I[j], in2R[i], in2I[i], op);
                        }
                    }
                    break;
                    
                case kPadIn:
                    
                    if (sizeIn1 > sizeIn2)
                    {
                        for (unsigned long i = sizeCommon; i < sizeOut; i++)
                            calculate(outputR[i], outputI[i], in1R[i], in1I[i], defaultValueR, defaultValueI, op);
                    }
                    else
                    {
                        for (unsigned long i = sizeCommon; i < sizeOut; i++)
                            calculate(outputR[i], outputI[i], defaultValueR, defaultValueI, in2R[i], in2I[i], op);
                    }
                    break;
                    
                case kPadOut:
                    
                    for (unsigned long i = sizeCommon; i < sizeOut; i++)
                    {
                        outputR[i] = defaultValueR;
                        outputI[i] = defaultValueI;
                    }

                    break;
            }
        }
    }
    
private:
    
    // Description (specialise/override to change description)
    
    virtual const char *getDescriptionString() { return "Binary Operator - No operator info available"; }

    ParameterInfo *getParameterInfo()
    {
        static ParameterInfo info;
        return &info;
    }
    
    // Data
    
    double mPadValue;
    MismatchModes mMismatchMode;
};

// Complex Binary (Function Version)

// Complex Binary Functor

template <std::complex<double> func(const std::complex<double>&, const std::complex<double>&)> struct Complex_Binary_Functor
{
    std::complex<double> operator()(const std::complex<double> &x, const std::complex<double> &y) { return func(x, y); }
};

template <std::complex<double> func(const std::complex<double>&, const std::complex<double>&)> class FrameLib_Complex_Binary : public FrameLib_Complex_BinaryOp<Complex_Binary_Functor<func> >
{
    
public:
    
    // Constructor
    
    FrameLib_Complex_Binary(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner)
    : FrameLib_Complex_BinaryOp<Complex_Binary_Functor<func> > (context, serialisedParameters, owner) {}

private:
    
    // Description (specialise/override to change description)

    virtual const char *getDescriptionString() { return "Binary Operator - No operator info available"; }
};

#endif
