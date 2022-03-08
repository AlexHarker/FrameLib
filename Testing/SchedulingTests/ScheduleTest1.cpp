
#include "ScheduleTest1.h"
#include "FrameLib_Objects.h"

ScheduleTest1::ScheduleTest1(FrameLib_Proxy *proxy, FrameLib_Thread::Priorities priorities)
: mGlobal(nullptr)
, mProcessingQueue(initialise(priorities))
, mNumAudioIns(0)
, mNumAudioOuts(0)
, mProxy(proxy)
{
    using Connection = FrameLib_Object<FrameLib_Multistream>::Connection;

    FrameLib_Context context(mGlobal, this);
    FrameLib_Parameters::AutoSerial parameters;

    mProcessingQueue->setTimeOuts();

    mObjects.resize(7);

    double fl_0_vector_0[] = { 8 };
    parameters.clear();
    parameters.write("interval", fl_0_vector_0, 1);
    mObjects[0] = new FrameLib_Expand<FrameLib_Interval>(context, &parameters, mProxy, 1);

    parameters.clear();
    mObjects[1] = new FrameLib_Expand<FrameLib_Register>(context, &parameters, mProxy, 1);
    double fl_1_inputs_1[] = { 512 };
    mObjects[1]->setFixedInput(1, fl_1_inputs_1 , 1);
    mObjects[1]->addConnection(Connection(mObjects[0], 0), 0);

    parameters.clear();
    parameters.write("tag_01", "interval");
    mObjects[2] = new FrameLib_Expand<FrameLib_Tag>(context, &parameters, mProxy, 1);
    mObjects[2]->addConnection(Connection(mObjects[1], 0), 0);

    double fl_3_vector_0[] = { 512 };
    parameters.clear();
    parameters.write("interval", fl_3_vector_0, 1);
    mObjects[3] = new FrameLib_Expand<FrameLib_Interval>(context, &parameters, mProxy, 1);
    mObjects[3]->addConnection(Connection(mObjects[2], 0), 0);

    parameters.clear();
    parameters.write("tag_01", "length");
    mObjects[4] = new FrameLib_Expand<FrameLib_Tag>(context, &parameters, mProxy, 1);
    mObjects[4]->addConnection(Connection(mObjects[1], 0), 0);

    double fl_5_vector_0[] = { 65536 };
    double fl_5_vector_1[] = { 4096 };
    parameters.clear();
    parameters.write("buffer_size", fl_5_vector_0, 1);
    parameters.write("length", fl_5_vector_1, 1);
    mObjects[5] = new FrameLib_Expand<FrameLib_Source>(context, &parameters, mProxy, 2);
    mObjects[5]->addConnection(Connection(mObjects[3], 0), 0);
    mObjects[5]->addConnection(Connection(mObjects[4], 0), 1);

    parameters.clear();
    parameters.write("window", "hann");
    parameters.write("compensate", "linear");
    mObjects[6] = new FrameLib_Expand<FrameLib_Window>(context, &parameters, mProxy, 1);
    mObjects[6]->addConnection(Connection(mObjects[5], 0), 0);

    for (auto it = mObjects.begin(); it != mObjects.end(); it++)
    {
        (*it)->makeAutoOrderingConnections();

        if ((*it)->handlesAudio() && (*it)->getType() != ObjectType::Output)
            mAudioInObjects.push_back(*it);
        if ((*it)->handlesAudio() && (*it)->getType() == ObjectType::Output)
            mAudioOutObjects.push_back(*it);

        mNumAudioIns += (*it)->getNumAudioIns();
        mNumAudioOuts += (*it)->getNumAudioOuts();
    }
}

FrameLib_Context ScheduleTest1::initialise(FrameLib_Thread::Priorities priorities)
{
    FrameLib_Global::get(&mGlobal, priorities);
    return FrameLib_Context(mGlobal, this);
}

ScheduleTest1::~ScheduleTest1()
{
    for (auto it = mObjects.begin(); it != mObjects.end(); it++)
        delete *it;

    mObjects.clear();
    mAudioInObjects.clear();
    mAudioOutObjects.clear();
    delete mProxy;
    FrameLib_Global::release(&mGlobal);
}

void ScheduleTest1::reset(double samplerate, unsigned long maxvectorsize)
{
    for (auto it = mObjects.begin(); it != mObjects.end(); it++)
        (*it)->reset(samplerate, maxvectorsize);
}

void ScheduleTest1::process(double **inputs, double **outputs, unsigned long blockSize)
{
    FrameLib_AudioQueue queue;

    for (auto it = mAudioInObjects.begin(); it != mAudioInObjects.end(); it++)
    {
        (*it)->blockUpdate(inputs, outputs, blockSize, queue);

        inputs += (*it)->getNumAudioIns();
        outputs += (*it)->getNumAudioOuts();
    }

    for (auto it = mAudioOutObjects.begin(); it != mAudioOutObjects.end(); it++)
    {
        (*it)->blockUpdate(inputs, outputs, blockSize, queue);

        inputs += (*it)->getNumAudioIns();
        outputs += (*it)->getNumAudioOuts();
    }
}
