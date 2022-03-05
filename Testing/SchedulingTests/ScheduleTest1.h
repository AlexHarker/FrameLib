
#include "FrameLib_Multistream.h"
#include <vector>

class ScheduleTest1
{

public:

    ScheduleTest1(FrameLib_Proxy *proxy = new FrameLib_Proxy());
    ~ScheduleTest1();

    void reset(double samplerate, unsigned long maxvectorsize);
    void process(double **inputs, double **output, unsigned long blockSize);

    unsigned long getNumAudioIns() const { return mNumAudioIns; }
    unsigned long getNumAudioOuts() const  { return mNumAudioOuts; }

private:

    FrameLib_Global *mGlobal;
    std::vector<FrameLib_Multistream *> mObjects;
    std::vector<FrameLib_Multistream *> mAudioInObjects;
    std::vector<FrameLib_Multistream *> mAudioOutObjects;
    unsigned long mNumAudioIns;
    unsigned long mNumAudioOuts;
    FrameLib_Proxy *mProxy;
};
