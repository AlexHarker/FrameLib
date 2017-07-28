
#ifndef FRAMELIB_PEAKS_H
#define FRAMELIB_PEAKS_H

#include "FrameLib_DSP.h"

class FrameLib_Peaks : public FrameLib_Processor
{
    
public:
    
    FrameLib_Peaks(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 1, 3)
    {
        mParameters.set(serialisedParameters);
    }
    
private:
    
    double logValue(double val)
    {
        val = log(val);
        
        return val < -200.0 ? -200.0 : val;
    }
    
    void refinePeak(double& pos, double& amp, double posUncorrected, double vm1, double v0, double vp1)
    {
        // Take log values (avoiding values that are too low)
        
        vm1 = logValue(vm1);
        v0 = logValue(v0);
        vp1 = logValue(vp1);
        
        // Parabolic interpolation
        
        double divisor = vm1 + vp1 - (2.0 * v0);
        double correction = divisor ? (0.5 * (vm1 - vp1)) / divisor : 0.0;
        
        // N.B - Leave amplitude in a log format
        
        pos = posUncorrected + correction;
        amp = exp(v0 - (0.25 * (vm1 - vp1) * correction));
    }
        
    void process ()
    {
        // Get Input
        
        unsigned long sizeIn, sizeOut1, sizeOut2, sizeOut3;
        unsigned long nPeaks = 0;
        
        double *input = getInput(0, &sizeIn);
      
        if (!sizeIn)
            return;
        
        // Calculate number of peaks (for now ignore peaks in the top 2 positions)
      
        if (sizeIn > 2 && (input[0] > input[1]) && (input[0] > input[2]))
            nPeaks++;
        else
        {
            if (sizeIn > 3 && (input[1] > input[2]) && (input[1] > input[3]) && (input[1] > input[0]))
                nPeaks++;
        }
        for (long i = 2; i < (sizeIn - 2); i++)
            if ((input[i] > input[i - 2]) && (input[i] > input[i - 1]) && (input[i] > input[i + 1]) && (input[i] > input[i + 2]))
                nPeaks++;
        
        // Allocate outputs
        
        requestOutputSize(0, sizeIn);
        requestOutputSize(1, nPeaks);
        requestOutputSize(2, nPeaks);
        allocateOutputs();
        
        double *output1 = getOutput(0, &sizeOut1);
        double *output2 = getOutput(1, &sizeOut2);
        double *output3 = getOutput(2, &sizeOut3);
        
        // Find and Refine Peaks (for now ignore peaks in the top 2 positions)
        
        nPeaks = 0;
        
        if (sizeOut1 && sizeOut2 && sizeOut3)
        {
            if (sizeIn > 2 && (input[0] > input[1]) && (input[0] > input[2]))
            {
                refinePeak(output2[0], output3[0], 0, input[1], input[0], input[1]);
                nPeaks++;
            }
            else
            {
                if (sizeIn > 3 && (input[1] > input[2]) && (input[1] > input[3]) && (input[1] > input[0]))
                {
                    refinePeak(output2[0], output3[0], 1, input[0], input[1], input[2]);
                    nPeaks++;
                }
            }
            
            for (long i = 2; i < (sizeIn - 2); i++)
            {
                if ((input[i] > input[i - 2]) && (input[i] > input[i - 1]) && (input[i] > input[i + 1]) && (input[i] > input[i + 2]))
                {
                    refinePeak(output2[nPeaks], output3[nPeaks], i, input[i - 1], input[i], input[i + 1]);
                    nPeaks++;
                }
            }
        }
        
        // Set indices

        long binsFilled = 0;
        long peak = 0;
        long minPoint = 0;

        if (nPeaks)
        {
            for (; peak < (nPeaks - 1); peak++)
            {
                long beg = output2[peak];
                long end = output2[peak + 1];
                
                double minValue = input[beg];
                minPoint = beg;
                
                for (long i = beg; i < end; i++)
                {
                    if (input[i] < minValue)
                    {
                        minValue = input[i];
                        minPoint = i;
                    }
                }
            
                for (; binsFilled < minPoint; binsFilled++)
                    output1[binsFilled] = peak;
            }
        }
        
        // Fill to the end
        
        for (; binsFilled < sizeOut1; binsFilled++)
            output1[binsFilled] = peak;
    }
};

#endif
