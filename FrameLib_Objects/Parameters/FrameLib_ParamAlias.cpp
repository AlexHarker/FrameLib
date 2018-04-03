    
#include "FrameLib_ParamAlias.h"

// Constructor

FrameLib_ParamAlias::FrameLib_ParamAlias(FrameLib_Context context, unsigned long numOuts, void *owner, Parameters::Info *info) : FrameLib_Processor(context, owner, info)
{
    setIO(0, numOuts);
    addParameterInput();
    setInputMode(0, true, true, false, kFrameTagged);
    
    for (unsigned long i = 0; i < numOuts; i++)
        setOutputType(i, kFrameTagged);
}

void FrameLib_ParamAlias::addAlias(unsigned long idx, const char* inTag, const char* outTag, long argumentIdx)
{
    mAliases.push_back(Alias(idx, inTag, outTag, argumentIdx));
}

FrameLib_Parameters::Serial *FrameLib_ParamAlias::aliasForConstruction(FrameLib_Parameters::Serial *parametersIn, unsigned long idx)
{
    mSerial.clear();
    
    for (Parameters::Serial::Iterator it = parametersIn->begin(); it != parametersIn->end(); it++)
    {
        for (std::vector<Alias>::iterator jt = mAliases.begin(); jt != mAliases.end(); jt++)
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
    
    std::vector<std::vector<FrameLib_DSP *> > dependencies;
    dependencies.resize(getNumOuts());
    
    // For each output get a list of output dependencies
    
    for (unsigned long i = 0; i < getNumOuts(); i++)
        addOutputDependencies(dependencies[i], i);
    
    // Now get each alias in turn and duplicate the parameter characteristics
    
    for (std::vector<Alias>::iterator it = mAliases.begin(); it != mAliases.end(); it++)
    {
        unsigned long idx = it->mIndex;
     
        for (std::vector<FrameLib_DSP *>::iterator jt = dependencies[idx].begin(); jt != dependencies[idx].end(); jt++)
        {
            unsigned long idx = mParameters.size();
            const char *tag = it->mInTag.c_str();
            const Parameters *params = (*jt)->getParameters();
            unsigned paramIdx = -1;
    
            if ((paramIdx = params->getIdx(it->mOutTag.c_str())) != -1)
            {
                double value = params->getDefault(paramIdx);

                switch(params->getType(paramIdx))
                {
                    case Parameters::kValue:
                        
                        switch (params->getNumericType(paramIdx))
                        {
                            case Parameters::kNumericBool:      mParameters.addBool(idx, tag, value);       break;
                            case Parameters::kNumericInteger:   mParameters.addInt(idx, tag, value);        break;
                            default:                            mParameters.addDouble(idx, tag, value);     break;
                        }
                        break;

                    case Parameters::kEnum:
                        mParameters.addEnum(idx, tag);
                        for (long i = 0; i <= params->getMax(paramIdx); i++)
                            mParameters.addEnumItem(idx, params->getItemString(paramIdx, i).c_str());
                        break;
                        
                    case Parameters::kString:
                        mParameters.addString(idx, tag);
                        break;
                        
                    case Parameters::kArray:
                    {
                        size_t size = params->getArraySize(paramIdx);
                        
                        switch (params->getNumericType(paramIdx))
                        {
                            case Parameters::kNumericBool:      mParameters.addBoolArray(idx, tag, value, size);       break;
                            case Parameters::kNumericInteger:   mParameters.addIntArray(idx, tag, value, size);        break;
                            default:                            mParameters.addDoubleArray(idx, tag, value, size);
                        }
                        break;
                    }
                        
                    case Parameters::kVariableArray:
                    {
                        size_t size = params->getArraySize(paramIdx);
                        size_t maxSize = params->getArrayMaxSize(paramIdx);

                        switch (params->getNumericType(paramIdx))
                        {
                            case Parameters::kNumericBool:      mParameters.addVariableBoolArray(idx, tag, value, maxSize, size);        break;
                            case Parameters::kNumericInteger:   mParameters.addVariableIntArray(idx, tag, value, maxSize, size);         break;
                            default:                            mParameters.addVariableDoubleArray(idx, tag, value, maxSize, size);      break;
                        }
                        break;
                    }
                }
                
                if (params->getNumericType(paramIdx) == Parameters::kNumericInteger || params->getNumericType(paramIdx) == Parameters::kNumericDouble)
                {
                    switch (params->getClipMode(paramIdx))
                    {
                        case Parameters::kNone: break;
                        case Parameters::kMin:  mParameters.setMin(params->getMin(paramIdx));      break;
                        case Parameters::kMax:  mParameters.setMax(params->getMax(paramIdx));      break;
                        case Parameters::kClip: mParameters.setClip(params->getMin(paramIdx), params->getMax(paramIdx));    break;
                    }
                }
                
                // FIX!!
                
                //mParameters.setInstantiation(params->instantionOnly)
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
    return "Tagged Output Frames";
}

// Process

void FrameLib_ParamAlias::process()
{    
    const Serial *parametersIn = getInput(0);
    Alias **matches = alloc<Alias *>(parametersIn ? parametersIn->numTags() : 0);

    // Request zero sized outputs
    
    for (unsigned long i = 0; i < getNumOuts(); i++)
        requestOutputSize(i, 0);
    
    // Calculate tag sizes and assign to outputs
    
    if (parametersIn)
    {
        for (Parameters::Serial::Iterator it = parametersIn->begin(); it != parametersIn->end(); it++)
        {
            for (std::vector<Alias>::iterator jt = mAliases.begin(); jt != mAliases.end(); jt++)
            {
                if (!strcmp(jt->mInTag.c_str(), it.getTag()))
                {
                    matches[it.getIndex()] = &(*jt);
                    size_t size = it.getType() == kVector ? Serial::calcSize(jt->mOutTag.c_str(), it.getVectorSize()) :  Serial::calcSize(jt->mOutTag.c_str(), it.getString());
                    requestAddedOutputSize(jt->mIndex, size);
                    break;
                }
            }
        }
    }
    
    // Allocate and write to outputs
    
    if (allocateOutputs())
    {
        for (Serial::Iterator it = parametersIn->begin(); it != parametersIn->end(); it++)
        {
            Alias *alias = matches[it.getIndex()];
            
            if (alias)
                it.alias(getOutput(alias->mIndex), alias->mOutTag.c_str());
        }
    }
    
    dealloc(matches);
}
