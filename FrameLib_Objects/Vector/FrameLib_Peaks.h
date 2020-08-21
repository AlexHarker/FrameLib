
#ifndef FRAMELIB_PEAKS_H
#define FRAMELIB_PEAKS_H

#include "FrameLib_DSP.h"

class FrameLib_Peaks final : public FrameLib_Processor
{
    enum Parameters { kCriteria, kThreshold, kEdges, kPadding, kRefinement, kBoundary, kAlwaysDetect };
    enum Criteria { kOneNeighbour, kTwoNeighbours, kThreeNeighbours, kFourNeighbours };
    enum Edges { kPad, kExtend, kWrap, kFold, kMirror };
    enum Refinements { kNone, kParabolic, kParabolicLog };
    enum Boundaries { kMinimum, kMidpoint };

public:
    
    // Constructor
    
    FrameLib_Peaks(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    // Process
    
    void process() override;
};

#endif
