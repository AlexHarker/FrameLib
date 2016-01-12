
#ifndef FRAMELIB_SHIFT_H
#define FRAMELIB_SHIFT_H

#include <algorithm>
#include "FrameLib_DSP.h"
#include "FrameLib_Sort_Functions.h"

class FrameLib_MedianFilter : public FrameLib_Processor
{
    enum AttributeList {kWidth, kPadding, kMode};
    
    enum Modes {kPad, kWrap, kFold};
    
public:
    
    FrameLib_MedianFilter(DSPQueue *queue, FrameLib_Attributes::Serial *serialisedAttributes) : FrameLib_Processor(queue, 2, 1, 0, 0)
    {
        mAttributes.addDouble(kWidth, "width", 1.0, 0);
        mAttributes.setMin(0.0);
        mAttributes.addDouble(kPadding, "padding", 0.0, 1);
        
        mAttributes.addEnum(kMode, "mode", 3);
        mAttributes.addEnumItem(kPad, "pad");
        mAttributes.addEnumItem(kWrap, "wrap");
        mAttributes.addEnumItem(kFold, "fold");
        
        mAttributes.set(serialisedAttributes);
        
        inputMode(1, TRUE, FALSE, FALSE);
    }
    
private:
    
    double insertMedian(double *temp, unsigned long *indices, double value, long index, long width)
    {
        long current, insert, gap;
        
        // Insert actual value
        
        temp[index] = value;
        
        // Find sort position
        
        current = -10000000;
        
        for (long i = 0; i < width; i++)
        {
            if (indices[i] == index)
            {
                current = i;
                break;
            }
        }
        
        // Search right
        
        for (insert = current, std::max(1L, gap = (width - current) >> 1); gap; gap >>= 1)
        {
            for (long i = insert + gap; i < width; i += gap)
            {
                if (temp[indices[i]] > value)
                    break;
                else
                    insert = i;
            }
        }
        
        // Swaps
        
        for (long i = current; i < insert; i++)
            indices[i] = indices[i + 1];
        indices[insert] = index;
        
        // Search left
        
        for (current = insert, std::max(1L, gap = current >> 1); gap; gap >>= 1)
        {
            for (long i = insert - gap; i >= 0; i -= gap)
            {
                if (temp[indices[i]] < value)
                    break;
                else
                    insert = i;
            }
        }
        
        // Swaps
        
        for (long i = current; i > insert; i--)
            indices[i] = indices[i - 1];
        indices[insert] = index;
        
        return temp[indices[width >> 1]];
    }
    
    double median(double *temp, unsigned long *indices, long width)
    {
        sortAscending(indices, temp, width);
        return temp[indices[width >> 1]];
    }
    
    double getPad(double *input, long index, long sizeIn, long width, double padValue)
    {
        return (index >= 0 && index < sizeIn) ? input[index] : padValue;
    }
    
    double getWrap(double *input, long index, long sizeIn, long width)
    {
        index %= sizeIn;
        index = index < 0 ? index + sizeIn : index;
        
        return input[index];
    }
    
    double getFold(double *input, long index, long sizeIn, long width)
    {
        index = abs(index) % ((sizeIn - 1) * 2);
        index = index > (sizeIn - 1) ? ((sizeIn - 1) * 2) - sizeIn : index;
        
        return input[index];
    }
    
    
protected:
    
    void update()
    {
        FrameLib_Attributes::Serial *serialised = getInput(1);
        
        if (serialised)
            mAttributes.set(serialised);
    }
    
    void process ()
    {
        // Get Input
        
        unsigned long width = mAttributes.getInt(kWidth);
        unsigned long sizeIn, sizeOut;
        double *input = getInput(0, &sizeIn);
        double padValue = mAttributes.getValue(kPadding);
        Modes mode = (Modes) mAttributes.getInt(kMode);
        
        requestOutputSize(0, sizeIn);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        double *temp = (double *) mAllocator->alloc(sizeof(double) * width);
        unsigned long *indices = (unsigned long *) mAllocator->alloc(sizeof(unsigned long) * width);
        
        // Do filtering
        
        if (sizeOut)
        {
            switch (mode)
            {
                case kWrap:
                    for (long i = 0; i < width; i++)
                        temp[i] = getWrap(input, i - (width >> 1), sizeIn, width);
                    output[0] = median(temp, indices, width);
                    for (long i = 1; i < sizeIn; i++)
                    {
                        double newValue = getWrap(input, i + (width >> 1) - 1, sizeIn, width);
                        output[i] = insertMedian(temp, indices, newValue, (i - 1) % width, width);
                    }
                    break;
                    
                case kPad:
                    for (long i = 0; i < width; i++)
                        temp[i] = getPad(input, i - (width >> 1), sizeIn, width, padValue);
                    output[0] = median(temp, indices, width);
                    for (long i = 1; i < sizeIn; i++)
                    {
                        double newValue = getPad(input, i + (width >> 1) - 1, sizeIn, width, padValue);
                        output[i] = insertMedian(temp, indices, newValue, (i - 1) % width, width);
                    }
                    break;
                    
                case kFold:
                    for (long i = 0; i < width; i++)
                        temp[i] = getFold(input, i - (width >> 1), sizeIn, width);
                    output[0] = median(temp, indices, width);
                    for (long i = 1; i < sizeIn; i++)
                    {
                        double newValue = getFold(input, i + (width >> 1) - 1, sizeIn, width);
                        output[i] = insertMedian(temp, indices, newValue, (i - 1) % width, width);
                    }
                    break;
            }
        }
        
        mAllocator->dealloc(temp);
        mAllocator->dealloc(indices);
    }
};

#endif
