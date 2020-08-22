
#include "FrameLib_MedianFilter.h"
#include "FrameLib_Sort_Functions.h"
#include "FrameLib_Edges.h"
#include <algorithm>

// Constructor

FrameLib_MedianFilter::FrameLib_MedianFilter(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 2, 1)
{
    mParameters.addInt(kWidth, "width", 1, 0);
    mParameters.setMin(1);
    
    mParameters.addDouble(kPadding, "pad", 0.0, 1);
    
    mParameters.addEnum(kEdges, "mode", 2);
    mParameters.addEnumItem(kEdgePad, "pad");
    mParameters.addEnumItem(kEdgeExtend, "extend");
    mParameters.addEnumItem(kEdgeWrap, "wrap");
    mParameters.addEnumItem(kEdgeFold, "fold");
    mParameters.addEnumItem(kEdgeMirror, "mirror");

    mParameters.addDouble(kPercentile, "percentile", 50.0, 3);
    mParameters.setClip(0.0, 100.0);
    
    mParameters.set(serialisedParameters);
    
    setParameterInput(1);
}

// Info

std::string FrameLib_MedianFilter::objectInfo(bool verbose)
{
    return formatInfo("Apply a median filter to the input: "
                      "The output is the same size as the input. "
                      "Each output value is the median of the area surrounding the input value. "
                      "The width of the area, and the edge behaviours are controllable.",
                      "Apply a median filter to the input.", verbose);
}

std::string FrameLib_MedianFilter::inputInfo(unsigned long idx, bool verbose)
{
    if (idx)
        return parameterInputInfo(verbose);
    else
        return "Input";
}

std::string FrameLib_MedianFilter::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_MedianFilter::ParameterInfo FrameLib_MedianFilter::sParamInfo;

FrameLib_MedianFilter::ParameterInfo::ParameterInfo()
{
    add("Sets the width of the median filtering in samples.");
    add("Sets the padding value.");
    add("Sets the mode that controls the edge behaviour: "
        "pad - edges are treated as though infinitely padded with the padding value. "
        "wrap - edges are treated as though the frame is wrapped cyclically. "
        "fold - edges are treated as though they fold over (suitable for spectral purposes).");
}

// Helpers

double insertMedian(double *data, unsigned long *indices, double value, long index, long width, unsigned long pos)
{
    long current = -1, insert = 0, gap = 0;
    
    // Insert actual data
    
    data[index] = value;
    
    // Find sort position for the index
    
    for (current = 0; (current < width) && (indices[current] != index); current++);
    
    // Search right
    
    for (insert = current, gap = std::max(1L, (width - current) >> 1); gap; gap >>= 1)
        for (long i = insert + gap; (i < width) && (value > data[indices[i]]); i += gap)
            insert = i;
    
    // Swaps
    
    std::copy(indices + current + 1, indices + insert + 1, indices + current);
    indices[insert] = index;
    
    // Search left
    
    for (current = insert, gap = std::max(1L, current >> 1); gap; gap >>= 1)
        for (long i = insert - gap; (i >= 0) && (value < data[indices[i]]); i -= gap)
            insert = i;
    
    // Swaps

    std::copy_backward(indices + insert, indices + current, indices + current + 1);
    indices[insert] = index;
    
    return data[indices[pos]];
}

template <class T>
void filter(T in, double *out, double *data, unsigned long* indices, long width, long size, unsigned long pos)
{
    long o1 = width >> 1;
    long o2 = width - o1;
    
    // Calculate the first median
    
    for (long i = 0; i < width; i++)
        data[i] = in(i - o1);
    
    sortIndicesAscending(indices, data, width);
    out[0] = data[indices[pos]];
    
    // Do other values using insertion

    for (long i = 1; i < size; i++)
        out[i] = insertMedian(data, indices, in(i + o2), (i - 1) % width, width, pos);
}

// Process

void FrameLib_MedianFilter::process()
{
    unsigned long sizeIn, sizeOut;
    
    // Parameters
    
    long width = mParameters.getInt(kWidth);
    double pad = mParameters.getValue(kPadding);
    Edges edges = static_cast<Edges>(mParameters.getInt(kEdges));
    unsigned long pos = roundToUInt(mParameters.getValue(kPercentile) * (width - 1) / 100.0);
    
    pos = std::min(pos, static_cast<unsigned long>(width - 1));
    
    // Get Input

    const double *input = getInput(0, &sizeIn);
    long size = static_cast<long>(sizeIn);

    requestOutputSize(0, sizeIn);
    allocateOutputs();
    
    double *output = getOutput(0, &sizeOut);    
    double *data = alloc<double>(width);
    unsigned long *indices = alloc<unsigned long>(width);
    
    // Do filtering
    
    if (sizeOut && data && indices)
    {
        switch (edges)
        {
            case kEdgePad:      filter(EdgesPad(input, size, pad), output, data, indices, width, size, pos);    break;
            case kEdgeExtend:   filter(EdgesExtend(input, size), output, data, indices, width, size, pos);      break;
            case kEdgeWrap:     filter(EdgesWrap(input, size), output, data, indices, width, size, pos);        break;
            case kEdgeFold:     filter(EdgesFold(input, size), output, data, indices, width, size, pos);        break;
            case kEdgeMirror:   filter(EdgesMirror(input, size), output, data, indices, width, size, pos);      break;
        }
    }
    else
    {
        if (sizeOut && (!data || !indices))
        {
            zeroVector(output, sizeOut);
            getReporter()(kErrorObject, getProxy(), "couldn't allocate temporary memory");
        }
    }
    
    dealloc(data);
    dealloc(indices);
}
