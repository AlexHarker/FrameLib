
#include "FrameLib_MakeString.h"

// Constructor

FrameLib_MakeString::FrameLib_MakeString(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo), mNumIns(1)
{
    mParameters.addInt(kNumItems, "num_items", 1);
    mParameters.setClip(1, maxNumItems);
    mParameters.setInstantiation();
    
    mParameters.addString(kTag, "tag", 0);

    mParameters.addVariableBoolArray(kTriggers, "trigger_ins", true, maxNumItems, maxNumItems);
    mParameters.setInstantiation();
    
    mParameters.addVariableIntArray(kPadding, "padding", 0, maxNumItems, 0);
    
    
    // Read in once to get number of items needed
    
    mParameters.setErrorReportingEnabled(false);
    mParameters.set(serialisedParameters);
    mParameters.setErrorReportingEnabled(true);

    // If no number of items is specified explicity then examine the serialised parameters to determine the number needed
    
    if (!mParameters.changed(kNumItems))
    {
        for (int i = 0; i < maxNumItems; i++)
        {
            if (serialisedParameters->find(FrameLib_StringMaker<>(i + 1)) != serialisedParameters->end() || serialisedParameters->find(FrameLib_StringMaker<>("item_", i + 1, 2)) != serialisedParameters->end())
                mParameters.set(kNumItems, (long) (i + 1));
        }
    }
    
    // Read number of items and setup parameters
    
    mNumItems = mParameters.getInt(kNumItems);
    
    for (unsigned long i = 0; i < mNumItems; i++)
        mParameters.addString(kItems + i, FrameLib_StringMaker<>("item_", i + 1, 2), i + 1);
    
    // Read in again to get most parameters
    
    mParameters.set(serialisedParameters);

    // Setup the number of inputs
    
    for (unsigned long i = 0; i < mNumItems; i++)
    {
        mInputs[i] = getInputNumber(i);
        unsigned long input = mInputs[i] >= 0 ? static_cast<unsigned long>(mInputs[i]) + 1 : 0;
        mNumIns = std::max(mNumIns, input);
    }
    
    // Setup IO

    const double *triggers = mParameters.getArray(kTriggers);
    unsigned long triggersSize = mParameters.getArraySize(kTriggers);
    
    setIO(mNumIns, 1);
    
    for (long i = 0; i < mNumIns; i++)
        setInputMode(i, false, (i < triggersSize) && triggers[i], false);
    
    setOutputType(0, kFrameTagged);
    
    addParameterInput();
}

// Info

std::string FrameLib_MakeString::objectInfo(bool verbose)
{
    return formatInfo("Creates a tagged string by concatenating strings and numbers: "
                      "Items for concatenatation are set as parameter and can be altered dynamically. "
                      "Numeric inputs are indicated as in1 to inx (where x is the number of inputs created). "
                      "Only a single number is read from each input, interpreted as an integer. "
                      "Numbers can be optionally zero-padded. "
                      "The number of items to concatenate can be set explicitly by parameter. "
                      "Alternatively, it can be set implicitly by the item parameters present at instantiation.",
                      "Creates tagged strings by concatenating both strings and numbers.", verbose);
}

std::string FrameLib_MakeString::inputInfo(unsigned long idx, bool verbose)
{
    if (idx == (getNumIns() - 1))
        return parameterInputInfo(verbose);
    else
        return formatInfo("Numeric Input #", idx);
}

std::string FrameLib_MakeString::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_MakeString::ParameterInfo FrameLib_MakeString::sParamInfo;

FrameLib_MakeString::ParameterInfo::ParameterInfo()
{
    const int strBufSize = 256;
    char str[strBufSize];
    
    add("Sets the number of items.");
    add("Sets the output tag.");
    add("Sets which inputs trigger output using an array of bools. "
        "By default all inputs trigger output.");
    add("Sets the zero padding for each input using an array of integers. "
        "By default there is no zero padding.");
    
    for (int i = 0; i < maxNumItems; i++)
    {
        snprintf(str, strBufSize, "Sets the string or input for item %d.", i + 1);
        add(str);
    }
}

// Input Number Helper

long FrameLib_MakeString::getInputNumber(unsigned long idx)
{
    const char *str = mParameters.getString(kItems + idx);
    
    for (long i = 0; i < maxNumItems; i++)
    {
        if (!strcmp(str, FrameLib_StringMaker<>("in", i + 1, 0)))
            return i;
    }
    
    return -1;
}

// Update and Process

void FrameLib_MakeString::update()
{
    for (unsigned long i = 0; i < mNumItems; i++)
    {
        if (mParameters.changed(kItems + i))
        {
            long input = getInputNumber(i);
            mInputs[i] = input < mNumIns ? input : -1;
        }
    }
}

// Process

void FrameLib_MakeString::process()
{
    const char *tag = mParameters.getString(kTag);
    const double *padding = mParameters.getArray(kPadding);
    unsigned long paddingSize = mParameters.getArraySize(kPadding);

    // Form the string
    
    const unsigned long maxLength = FrameLib_Parameters::maxStrLen;
    char str[maxLength + 1];
    size_t length = 0;
    
    auto addString = [&](const char *newStr)
    {
        size_t addLength = strlen(newStr);
        if (length + addLength < maxLength)
        {
            strcpy(str + length, newStr);
            length += addLength;
        }
        else
        {
            getReporter()(kErrorObject, getProxy(), "string concatenation exceeds maximum string length");
            return true;
        }
        
        return false;
    };
    
    bool error = false;

    for (int i = 0; i < mNumItems && !error; i++)
    {
        long idx = mInputs[i];
        
        // Convert number or add fixed string

        if (idx >= 0)
        {            
            unsigned long sizeIn;
            long pad = idx < paddingSize ? static_cast<long>(padding[idx]) : 0;
            long number = 0;
            
            const double *input = getInput(mInputs[i], &sizeIn);
            
            if (sizeIn)
                number = static_cast<long>(input[0]);
                
            // Convert to string
            
            error = addString(FrameLib_StringMaker<maxLength + 1>(number, pad));
        }
        else
            error = addString(mParameters.getString(kItems + i));
    }
    
    // Allocate output
    
    requestOutputSize(0, FrameLib_Parameters::Serial::calcSize(tag, str));
    allocateOutputs();
    
    FrameLib_Parameters::Serial *output = getOutput(0);
    
    if (output)
        output->write(tag, str);
}
