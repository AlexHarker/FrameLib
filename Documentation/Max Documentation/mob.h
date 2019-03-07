
#include "FrameLib_Multistream.h"
#include <vector>

class mob 
{
public:

    mob(FrameLib_Proxy *proxy = new FrameLib_Proxy());
    ~mob();

private:
    FrameLib_Global *mGlobal;
    // std::vector<FrameLib_Multistream *> mObjects;
    FrameLib_Multistream *mObject;
    FrameLib_Proxy *mProxy;
    std::vector<FrameLib_Multistream *> mAudioObjects;
    unsigned long mNumAudioIns;
    unsigned long mNumAudioOuts;
};
