
#include "m_pd.h"
#include "FrameLib_DSP.h"

// FIX - abstract out max buffer interaction (buffer name / channel)
// FIX - consider adding anti-alising later....

class FrameLib_PDRead : public FrameLib_Processor
{
    // Parameter Info and Enums
    
    enum ParameterList { kBuffer, kChannel, kMode, kUnits };
    enum Modes { kHermite, kBSpline, kLagrange, kLinear, kNone };
    enum Units { kMS, kSeconds, kSamples };

    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor
    
    FrameLib_PDRead(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    // Process
    
    void update() { mBufferName = gensym(mParameters.getString(kBuffer)); }
    void process();
    
    // Data
    
    t_symbol *mBufferName;
    long mChan;
    Modes mMode;
    Units mUnits;
    
    static ParameterInfo sParamInfo;
};
