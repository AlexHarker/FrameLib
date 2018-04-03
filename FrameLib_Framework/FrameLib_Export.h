
static char exportHeader[] = "\n\
#include <FrameLib_MultiChannel.h>\n\
#include <vector>\n\n\
class FrameLib_Export\n\
{\n\n\
public:\n\n\
    FrameLib_Export();\n\
    ~FrameLib_Export();\n\n\
    void process(double **inputs, double **output, unsigned long blockSize);\n\n\
    unsigned long getNumAudioIns() const { return mNumAudioIns; }\n\
    unsigned long getNumAudioOuts() const  { return mNumAudioOuts; }\n\n\
private:\n\n\
    FrameLib_Global *mGlobal;\n\
    std::vector<FrameLib_MultiChannel *> mObjects;\n\
    std::vector<FrameLib_MultiChannel *> mAudioObjects;\n\
    unsigned long mNumAudioIns;\n\
    unsigned long mNumAudioOuts;\n\
};";

static char exportCPPOpen[] = "\n\
#include <export.h>\n\
#include <FrameLib_Objects.h>\n\n\
FrameLib_Export::FrameLib_Export() : mNumAudioIns(0), mNumAudioOuts(0)\n\
{\n\
typedef FrameLib_Object<FrameLib_MultiChannel>::Connection Connection;\n\n\
    FrameLib_Global::get(&mGlobal);\n\
    FrameLib_Context context(mGlobal, this);\n\
    FrameLib_Parameters::AutoSerial parameters;\n\
    void *owner = this;\n\n";

static char exportCPPClose[] = "\
    for (std::vector<FrameLib_MultiChannel *>::iterator it = mObjects.begin(); it != mObjects.end(); it++)\n\
    {\n\
        if ((*it)->handlesAudio())\n\
            mAudioObjects.push_back(*it);\n\n\
        mNumAudioIns += (*it)->getNumAudioIns();\n\
        mNumAudioOuts += (*it)->getNumAudioOuts();\n\
    }\n\n\
    mObjects.clear();\n\
    mAudioObjects.clear();\n\
}\n\n\
FrameLib_Export::~FrameLib_Export()\n\
{\n\
    for (std::vector<FrameLib_MultiChannel *>::iterator it = mObjects.begin(); it != mObjects.end(); it++)\n\
        delete *it;\n\n\
    mObjects.clear();\n\
    mAudioObjects.clear();\n\
    FrameLib_Global::release(&mGlobal);\n\
}\n\n\
void FrameLib_Export::process(double **inputs, double **outputs, unsigned long blockSize)\n\
{\n\
    for (std::vector<FrameLib_MultiChannel *>::iterator it = mAudioObjects.begin(); it != mAudioObjects.end(); it++)\n\
    {\n\
        (*it)->blockUpdate(inputs, outputs, blockSize);\n\n\
        inputs += (*it)->getNumAudioIns();\n\
        outputs += (*it)->getNumAudioOuts();\n\
    }\n\
}";

static char exportIndent[] = "    ";
