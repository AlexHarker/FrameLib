
static char exportHeader[] = "\n\
#include \"FrameLib_Multistream.h\"\n\
#include <vector>\n\n\
class $\n\
{\n\n\
public:\n\n\
    $(FrameLib_Proxy *proxy = new FrameLib_Proxy());\n\
    ~$();\n\n\
    void reset(double samplerate, unsigned long maxvectorsize);\n\
    void process(double **inputs, double **output, unsigned long blockSize);\n\n\
    unsigned long getNumAudioIns() const { return mNumAudioIns; }\n\
    unsigned long getNumAudioOuts() const  { return mNumAudioOuts; }\n\n\
private:\n\n\
    FrameLib_Global *mGlobal;\n\
    std::vector<FrameLib_Multistream *> mObjects;\n\
    std::vector<FrameLib_Multistream *> mAudioObjects;\n\
    unsigned long mNumAudioIns;\n\
    unsigned long mNumAudioOuts;\n\
    FrameLib_Proxy *mProxy;\n\
};";

static char exportCPPOpen[] = "\n\
#include \"$.h\"\n\
#include \"FrameLib_Objects.h\"\n\n\
$::$(FrameLib_Proxy *proxy) : mGlobal(nullptr), mNumAudioIns(0), mNumAudioOuts(0), mProxy(proxy)\n\
{\n\
    using Connection = FrameLib_Object<FrameLib_Multistream>::Connection;\n\n\
    FrameLib_Global::get(&mGlobal, FrameLib_Thread::defaultPriorities());\n\
    FrameLib_Context context(mGlobal, this);\n\
    FrameLib_Parameters::AutoSerial parameters;\n\n";

static char exportCPPClose[] = "\
    for (auto it = mObjects.begin(); it != mObjects.end(); it++)\n\
    {\n\
        (*it)->autoOrderingConnections();\n\n\
        if ((*it)->handlesAudio())\n\
            mAudioObjects.push_back(*it);\n\n\
        mNumAudioIns += (*it)->getNumAudioIns();\n\
        mNumAudioOuts += (*it)->getNumAudioOuts();\n\
    }\n\
}\n\n\
$::~$()\n\
{\n\
    for (auto it = mObjects.begin(); it != mObjects.end(); it++)\n\
        delete *it;\n\n\
    mObjects.clear();\n\
    mAudioObjects.clear();\n\
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
    for (auto it = mAudioObjects.begin(); it != mAudioObjects.end(); it++)\n\
    {\n\
        (*it)->blockUpdate(inputs, outputs, blockSize);\n\n\
        inputs += (*it)->getNumAudioIns();\n\
        outputs += (*it)->getNumAudioOuts();\n\
    }\n\
}";

static char exportIndent[] = "    ";
