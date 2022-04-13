    
#include "FrameLib_ParamAlias.h"

// Constructor

FrameLib_ParamAlias::FrameLib_ParamAlias(FrameLib_Context context, unsigned long numOuts, FrameLib_Proxy *proxy, Parameters::Info *info)
: FrameLib_Processor(context, proxy, info)
{
    setIO(0, numOuts);
    addParameterInput();
    setInputMode(0, true, true, false, FrameType::Tagged);
    
    for (unsigned long i = 0; i < numOuts; i++)
        setOutputType(i, FrameType::Tagged);
}

void FrameLib_ParamAlias::addAlias(unsigned long idx, const char* inTag, const char* outTag, long argumentIdx)
{
    mAliases.push_back(Alias(idx, inTag, outTag, argumentIdx));
}

FrameLib_Parameters::Serial *FrameLib_ParamAlias::aliasForConstruction(Serial *parametersIn, unsigned long idx)
{
    mSerial.clear();
    
    for (auto it = parametersIn->begin(); it != parametersIn->end(); it++)
    {
        for (auto jt = mAliases.begin(); jt != mAliases.end(); jt++)
        {
            if (jt->mIndex == idx)
            {
                if (!strcmp(jt->mInTag.c_str(), it.getTag()) || !strcmp(jt->mArgumentStr.c_str(), it.getTag()))
                {
                    it.alias(&mSerial, jt->mOutTag.c_str());
                    break;
                }
            }
        }
    }
    
    return &mSerial;
}

void FrameLib_ParamAlias::initialise()
{
    assert(!mParameters.size() && "Parameters have already been initialised");
    
    // Make space to store all the output dependencies
    
    std::vector<std::vector<FrameLib_DSP *>> dependencies;
    dependencies.resize(getNumOuts());
    
    // For each output get a list of output dependencies
    
    for (unsigned long i = 0; i < getNumOuts(); i++)
        addOutputDependencies(dependencies[i], i);
    
    // Now get each alias in turn and duplicate the parameter characteristics
    
    for (auto it = mAliases.begin(); it != mAliases.end(); it++)
    {
        unsigned long idx = it->mIndex;
     
        for (auto jt = dependencies[idx].begin(); jt != dependencies[idx].end(); jt++)
        {
            unsigned long idx = mParameters.size();
            const char *tag = it->mInTag.c_str();
            const Parameters *params = (*jt)->getParameters();
            unsigned long paramIdx = -1;
    
            if ((paramIdx = params->getIdx(it->mOutTag.c_str())) != static_cast<unsigned long>(-1))
            {
                double value = params->getDefault(paramIdx);

                switch(params->getType(paramIdx))
                {
                    case Type::Value:
                        
                        switch (params->getNumericType(paramIdx))
                        {
                            case NumericType::Bool:         mParameters.addBool(idx, tag, static_cast<bool>(value));    break;
                            case NumericType::Integer:      mParameters.addInt(idx, tag, static_cast<long>(value));     break;
                            default:                        mParameters.addDouble(idx, tag, value);                     break;
                        }
                        break;

                    case Type::Enum:
                        mParameters.addEnum(idx, tag);
                        for (long i = 0; i <= params->getMax(paramIdx); i++)
                            mParameters.addEnumItem(idx, params->getItemString(paramIdx, i));
                        break;
                        
                    case Type::String:
                        mParameters.addString(idx, tag);
                        break;
                        
                    case Type::Array:
                    {
                        auto size = params->getArraySize(paramIdx);
                        
                        switch (params->getNumericType(paramIdx))
                        {
                            case NumericType::Bool:         mParameters.addBoolArray(idx, tag, static_cast<bool>(value), size);     break;
                            case NumericType::Integer:      mParameters.addIntArray(idx, tag, static_cast<long>(value), size);      break;
                            default:                        mParameters.addDoubleArray(idx, tag, value, size);
                        }
                        break;
                    }
                        
                    case Type::VariableArray:
                    {
                        auto size = params->getArraySize(paramIdx);
                        auto maxSize = params->getArrayMaxSize(paramIdx);

                        switch (params->getNumericType(paramIdx))
                        {
                            case NumericType::Bool:         mParameters.addVariableBoolArray(idx, tag, static_cast<bool>(value), maxSize, size);    break;
                            case NumericType::Integer:      mParameters.addVariableIntArray(idx, tag, static_cast<long>(value), maxSize, size);     break;
                            default:                        mParameters.addVariableDoubleArray(idx, tag, value, maxSize, size);      break;
                        }
                        break;
                    }
                }
                
                if (params->getNumericType(paramIdx) == NumericType::Integer || params->getNumericType(paramIdx) == NumericType::Double)
                {
                    switch (params->getClipMode(paramIdx))
                    {
                        case ClipMode::None: break;
                        case ClipMode::Min:  mParameters.setMin(params->getMin(paramIdx));      break;
                        case ClipMode::Max:  mParameters.setMax(params->getMax(paramIdx));      break;
                        case ClipMode::Clip: mParameters.setClip(params->getMin(paramIdx), params->getMax(paramIdx));    break;
                    }
                }
                
                if (params->getInstantiation(paramIdx))
                    mParameters.setInstantiation();
                
                continue;
            }
            
            mParameters.addDouble(idx, tag);
        }
    }
}

std::string FrameLib_ParamAlias::objectInfo(bool verbose)
{
    return formatInfo("Dispatches parameters to a variety of aliases given a set of aliases: "
                      "This object is for C++ direct usage only. "
                      "Dispatches parameters to a variety of aliases given a set of aliases.", verbose);
}

std::string FrameLib_ParamAlias::inputInfo(unsigned long idx, bool verbose)
{
    return parameterInputInfo(verbose);
}

std::string FrameLib_ParamAlias::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Process

void FrameLib_ParamAlias::process()
{    
    const Serial *parametersIn = getInput(0);
    auto matches = allocAutoArray<Alias *>(parametersIn ? parametersIn->numTags() : 0);

    // Request zero sized outputs
    
    for (unsigned long i = 0; i < getNumOuts(); i++)
        requestOutputSize(i, 0);
    
    // Calculate tag sizes and assign to outputs
    
    if (parametersIn)
    {
        for (auto it = parametersIn->begin(); it != parametersIn->end(); it++)
        {
            for (auto jt = mAliases.begin(); jt != mAliases.end(); jt++)
            {
                if (!strcmp(jt->mInTag.c_str(), it.getTag()))
                {
                    matches[it.getIndex()] = &(*jt);
                    unsigned long size = it.getType() == DataType::Vector ? Serial::calcSize(jt->mOutTag.c_str(), it.getVectorSize()) :  Serial::calcSize(jt->mOutTag.c_str(), it.getString());
                    requestAddedOutputSize(jt->mIndex, size);
                    break;
                }
            }
        }
    }
    
    // Allocate and write to outputs
    
    if (allocateOutputs())
    {
        for (auto it = parametersIn->begin(); it != parametersIn->end(); it++)
        {
            Alias *alias = matches[it.getIndex()];
            
            if (alias)
                it.alias(getOutput(alias->mIndex), alias->mOutTag.c_str());
        }
    }
}
