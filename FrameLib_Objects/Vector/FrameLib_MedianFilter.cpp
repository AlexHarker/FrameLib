
#include "FrameLib_MedianFilter.h"
#include "FrameLib_Sort_Functions.h"
#include <algorithm>

// Constructor

FrameLib_MedianFilter::FrameLib_MedianFilter(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner) : FrameLib_Processor(context, 2, 1)
{
    mParameters.addInt(kWidth, "width", 1, 0);
    mParameters.setMin(0);
    mParameters.addDouble(kPadding, "padding", 0.0, 1);
    
    mParameters.addEnum(kMode, "mode", 3);
    mParameters.addEnumItem(kPad, "pad");
    mParameters.addEnumItem(kWrap, "wrap");
    mParameters.addEnumItem(kFold, "fold");
    
    mParameters.set(serialisedParameters);
    
    inputMode(1, true, false, false, kFrameTagged);
}

// Helpers

double FrameLib_MedianFilter::insertMedian(double *temp, unsigned long *indices, double value, long index, long width)
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

double FrameLib_MedianFilter::median(double *temp, unsigned long *indices, long width)
{
    sortIndicesAscending(indices, temp, width);
    return temp[indices[width >> 1]];
}

double FrameLib_MedianFilter::getPad(double *input, long index, long sizeIn, long width, double padValue)
{
    return (index >= 0 && index < sizeIn) ? input[index] : padValue;
}

double FrameLib_MedianFilter::getWrap(double *input, long index, long sizeIn, long width)
{
    index %= sizeIn;
    index = index < 0 ? index + sizeIn : index;
    
    return input[index];
}

double FrameLib_MedianFilter::getFold(double *input, long index, long sizeIn, long width)
{
    index = std::abs(index) % ((sizeIn - 1) * 2);
    index = index > (sizeIn - 1) ? ((sizeIn - 1) * 2) - sizeIn : index;
    
    return input[index];
}

// Update and Process

void FrameLib_MedianFilter::update()
{
    FrameLib_Parameters::Serial *serialised = getInput(1);
    
    if (serialised)
        mParameters.set(serialised);
}

void FrameLib_MedianFilter::process()
{
    // Get Input
    
    unsigned long width = mParameters.getInt(kWidth);
    unsigned long sizeIn, sizeOut;
    double *input = getInput(0, &sizeIn);
    double padValue = mParameters.getValue(kPadding);
    Modes mode = (Modes) mParameters.getInt(kMode);
    
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