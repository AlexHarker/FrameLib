
#include "ext.h"
#include "FrameLib_DSP.h"

// FIX - abstract out max buffer interaction (buffer name / channel)
// FIX - consider adding anti-alising later....

class FrameLib_MaxRead : public FrameLib_Processor
{
    // Parameter Info and Enums
    
    enum ParameterList { kBuffer, kChannel, kMode, kUnits };
    enum Modes { kHermite, kBSpline, kLagrange, kLinear, kNone };
    enum Units { kMS, kSeconds, kSamples };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_MaxRead(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    // Process
    
    void process();
    
    // Data
    
    t_symbol *mBufferName;
    long mChan;
    Modes mMode;
    Units mUnits;
    
    static bool sInit;
    static ParameterInfo sParamInfo;
};
