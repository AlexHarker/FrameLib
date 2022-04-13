
static char exportHeader[] = "\n\
#include <FrameLib_Multistream.h>\n\
#include <vector>\n\n\
class $\n\
{\n\
public:\n\n\
    $(FrameLib_Proxy *proxy = new FrameLib_Proxy(), FrameLib_Thread::Priorities priorities = FrameLib_Thread::defaultPriorities());\n\
    ~$();\n\n\
    void reset(double samplerate, unsigned long maxvectorsize);\n\
    void process(double **inputs, double **output, unsigned long blockSize);\n\n\
    unsigned long getNumAudioIns() const { return mNumAudioIns; }\n\
    unsigned long getNumAudioOuts() const  { return mNumAudioOuts; }\n\n\
    void setMultithreading(bool multithread)  { mProcessingQueue->setMultithreading(multithread); }\n\
    void setTimeOuts(double relative = 0.0, double absolute = 0.0) { mProcessingQueue->setTimeOuts(relative, absolute); }\n\n\
private:\n\n\
    FrameLib_Context initialise(FrameLib_Thread::Priorities priorities);\n\n\
    FrameLib_Global *mGlobal;\n\
    FrameLib_Context::ProcessingQueue mProcessingQueue;\n\n\
    std::vector<FrameLib_Multistream *> mObjects;\n\
    std::vector<FrameLib_Multistream *> mAudioInObjects;\n\
    std::vector<FrameLib_Multistream *> mAudioOutObjects;\n\n\
    unsigned long mNumAudioIns;\n\
    unsigned long mNumAudioOuts;\n\n\
    FrameLib_Proxy *mProxy;\n\
};";

static char exportCPPOpen[] = "\n\
#include \"$.h\"\n\
#include \"FrameLib_Objects.h\"\n\n\
$::$(FrameLib_Proxy *proxy, FrameLib_Thread::Priorities priorities)\n\
: mGlobal(nullptr)\n\
, mProcessingQueue(initialise(priorities))\n\
, mNumAudioIns(0)\n\
, mNumAudioOuts(0)\n\
, mProxy(proxy)\n\
{\n\
    using Connection = FrameLib_Object<FrameLib_Multistream>::Connection;\n\n\
    FrameLib_Context context(mGlobal, this);\n\
    FrameLib_Parameters::AutoSerial parameters;\n\n\
    mProcessingQueue->setTimeOuts();\n\n";

static char exportCPPClose[] = "\
    for (auto it = mObjects.begin(); it != mObjects.end(); it++)\n\
    {\n\
        (*it)->makeAutoOrderingConnections();\n\n\
        if ((*it)->handlesAudio() && (*it)->getType() != ObjectType::Output)\n\
            mAudioInObjects.push_back(*it);\n\
        if ((*it)->handlesAudio() && (*it)->getType() == ObjectType::Output)\n\
            mAudioOutObjects.push_back(*it);\n\n\
        mNumAudioIns += (*it)->getNumAudioIns();\n\
        mNumAudioOuts += (*it)->getNumAudioOuts();\n\
    }\n\
}\n\n\
FrameLib_Context $::initialise(FrameLib_Thread::Priorities priorities)\n\
{\n\
    FrameLib_Global::get(&mGlobal, priorities);\n\
    return FrameLib_Context(mGlobal, this);\n\
}\n\n\
$::~$()\n\
{\n\
    for (auto it = mObjects.begin(); it != mObjects.end(); it++)\n\
        delete *it;\n\n\
    mObjects.clear();\n\
    mAudioInObjects.clear();\n\
    mAudioOutObjects.clear();\n\
    delete mProxy;\n\
    FrameLib_Global::release(&mGlobal);\n\
}\n\n\
void $::reset(double samplerate, unsigned long maxvectorsize)\n\
{\n\
    for (auto it = mObjects.begin(); it != mObjects.end(); it++)\n\
        (*it)->reset(samplerate, maxvectorsize);\n\
}\n\n\
void $::process(double **inputs, double **outputs, unsigned long blockSize)\n\
{\n\
    {\n\
        FrameLib_AudioQueue queue;\n\n\
        for (auto it = mAudioInObjects.begin(); it != mAudioInObjects.end(); it++)\n\
        {\n\
            (*it)->blockUpdate(inputs, outputs, blockSize, queue);\n\n\
            inputs += (*it)->getNumAudioIns();\n\
            outputs += (*it)->getNumAudioOuts();\n\
        }\n\
    }\n\n\
    for (auto it = mAudioOutObjects.begin(); it != mAudioOutObjects.end(); it++)\n\
    {\n\
        (*it)->blockUpdate(inputs, outputs, blockSize);\n\n\
        inputs += (*it)->getNumAudioIns();\n\
        outputs += (*it)->getNumAudioOuts();\n\
    }\n\
}";

static char exportIndent[] = "    ";
