
#include "FrameLib_Multistream.h"
#include <vector>

class ScheduleTest1
{
public:

    ScheduleTest1(FrameLib_Proxy *proxy = new FrameLib_Proxy(), FrameLib_Thread::Priorities priorities = FrameLib_Thread::defaultPriorities());
    ~ScheduleTest1();

    void reset(double samplerate, unsigned long maxvectorsize);
    void process(double **inputs, double **output, unsigned long blockSize);

    unsigned long getNumAudioIns() const { return mNumAudioIns; }
    unsigned long getNumAudioOuts() const  { return mNumAudioOuts; }

    void setMultithreading(bool multithread)  { mProcessingQueue->setMultithreading(multithread); }
    void setTimeOuts(double relative = 0.0, double absolute = 0.0) { mProcessingQueue->setTimeOuts(relative, absolute); }

private:

    FrameLib_Context initialise(FrameLib_Thread::Priorities priorities);

    FrameLib_Global *mGlobal;
    FrameLib_Context::ProcessingQueue mProcessingQueue;

    std::vector<FrameLib_Multistream *> mObjects;
    std::vector<FrameLib_Multistream *> mAudioInObjects;
    std::vector<FrameLib_Multistream *> mAudioOutObjects;

    unsigned long mNumAudioIns;
    unsigned long mNumAudioOuts;

    FrameLib_Proxy *mProxy;
};
