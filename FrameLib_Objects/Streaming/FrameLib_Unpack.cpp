
#include "FrameLib_Unpack.h"

// Constructor

FrameLib_Unpack::FrameLib_Unpack(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy, unsigned long nStreams) : FrameLib_Multistream(kProcessor, context, proxy, false, 1), mParameters(context, proxy, &sParamInfo)
{
    mParameters.addInt(kOutputs, "num_outs", 2, 0);
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    setIO(1, mParameters.getInt(kOutputs));
    
    mSerialisedParameters.write(serialisedParameters);
}

// Info

std::string FrameLib_Unpack::objectInfo(bool verbose)
{
    return formatInfo("Unpacks multistream input into individual streams: "
                      "Multistream inputs are unpacked in order across the outputs.",
                      "Unpacks multistream input into individual streams.", verbose);
}

std::string FrameLib_Unpack::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Multi-stream Input - to be unpacked into individual streams", "Multi-stream Input", idx, verbose);
}

std::string FrameLib_Unpack::outputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Output # - single stream", "Output #", idx, verbose);
}

bool FrameLib_Unpack::inputUpdate()
{
    bool change = false;
    
    for (unsigned long i = 0; i < getNumOuts(); i++)
    {
        bool exists = i < getInputNumStreams(0);
        bool slotExists = mOutputs[i].size();
        
        // Check for changes
        
        change |= exists != slotExists;
        change |= exists && slotExists && getInputChan(0, i) != mOutputs[i][0];
        
        // Store current value
        
        mOutputs.clear();
            
        if (exists)
            mOutputs[i].push_back(getInputChan(0, i));
    }
    
    return change;
}

// Parameter Info

FrameLib_Unpack::ParameterInfo FrameLib_Unpack::sParamInfo;

FrameLib_Unpack::ParameterInfo::ParameterInfo()
{
    add("Sets the number of outputs.");
}
