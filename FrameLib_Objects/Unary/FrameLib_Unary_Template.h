

#ifndef FRAMELIB_TEMPLATES_H
#define FRAMELIB_TEMPLATES_H

#include "FrameLib_DSP.h"
#include "FrameLib_Info.h"
#include <functional>

// OPT - vectorise where appropriate

// Unary (Operator Version)

template <typename Op> class FrameLib_UnaryOp : public FrameLib_Processor
{
    
public:
    
    // Constructor
    
    FrameLib_UnaryOp(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, NULL, 1, 1) {}
    
    // Info
    
    std::string objectInfo(bool verbose)
    {
        return getInfo("Calculates the # of each value in the input frame: The result is a frame of the same size as the input.",
                       "Calculates the # of each value in the input frame.", getOpString(), verbose);
    }

    std::string inputInfo(unsigned long idx, bool verbose)      { return "Input"; }
    std::string outputInfo(unsigned long idx, bool verbose)     { return "Result"; }

protected:
    
    // Process
    
    void process()
    {
        unsigned long size;
        double *input = getInput(0, &size);
        
        requestOutputSize(0, size);
        allocateOutputs();

        double *output = getOutput(0, &size);
        
        for (unsigned long i = 0; i < size; i++)
            output[i] = Op()(input[i]);
    }
    
    // Description (specialise/override to change description)

    virtual const char *getOpString() { return "<unary operation>"; }
};

// Unary Functor

template <double func(double)> struct Unary_Functor
{
    double operator()(double x) { return func(x); }
};

// Unary (Function Version)

template <double func(double)> class FrameLib_Unary : public FrameLib_UnaryOp<Unary_Functor<func> >
{
    
public:
    
    // Constructor
    
    FrameLib_Unary(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner)
    : FrameLib_UnaryOp < Unary_Functor<func> > (context, serialisedParameters, owner) {}

private:
    
    // Description (specialise/override to change description)

    virtual const char *getOpString() { return "<unary operation>"; }
};

#endif
