
#include "FrameLib_FromHost.h"
#include <algorithm>

// Owner Class

void FrameLib_FromHost::Proxy::write(const double *values, unsigned long N)
{
    typedef std::vector<std::vector<double> *> VectorPtrList;
    
    VectorPtrList swapVectors(mRegistered.size());
    
    // Copy vector once per registered object
    
    for (unsigned long i = 0; i < mRegistered.size(); i++)
    {
        swapVectors[i] = new std::vector<double>(N);
        swapVectors[i]->assign(values, values + N);
    }

    // Swap vectors once per registered object

    for (unsigned long i = 0; i < mRegistered.size(); i++)
    {
        mRegistered[i]->mLock.acquire();
        std::swap(mRegistered[i]->mVectorFrame, swapVectors[i]);
        mRegistered[i]->mLock.release();
    }
    
    // Delete old vectors
    
    for (VectorPtrList::iterator it = swapVectors.begin(); it != swapVectors.end(); it++)
        delete *it;
}

void FrameLib_FromHost::Proxy::write(const FrameLib_Parameters::Serial *serial)
{
    typedef std::vector<FrameLib_Parameters::AutoSerial *> SerialPtrList;
    
    SerialPtrList swapSerials(mRegistered.size());
    
    // Copy serial structure once per registered object
    
    for (unsigned long i = 0; i < mRegistered.size(); i++)
    {
        swapSerials[i] = new FrameLib_Parameters::AutoSerial();
        swapSerials[i]->write(serial);
    }
    
    // Swap serial structures once per registered object
    
    for (unsigned long i = 0; i < mRegistered.size(); i++)
    {
        mRegistered[i]->mLock.acquire();
        std::swap(mRegistered[i]->mSerialFrame, swapSerials[i]);
        mRegistered[i]->mLock.release();
    }
    
    // Delete old serial structures
    
    for (SerialPtrList::iterator it = swapSerials.begin(); it != swapSerials.end(); it++)
        delete *it;
}

void FrameLib_FromHost::Proxy::write(const char *tag, const char *string)
{
    FrameLib_Parameters::AutoSerial serial;
    serial.write(tag, string);
    write(&serial);
}

void FrameLib_FromHost::Proxy::write(const char *tag, const double *values, unsigned long N)
{
    FrameLib_Parameters::AutoSerial serial;
    serial.write(tag, values, N);
    write(&serial);
}

void FrameLib_FromHost::Proxy::registerObject(FrameLib_FromHost *object)
{
    if (std::find(mRegistered.begin(), mRegistered.end(), object) ==  mRegistered.end())
        mRegistered.push_back(object);
}

void FrameLib_FromHost::Proxy::unregisterObject(FrameLib_FromHost *object)
{
    std::vector<FrameLib_FromHost *>::iterator it;
    
    if ((it = std::find(mRegistered.begin(), mRegistered.end(), object)) !=  mRegistered.end())
        mRegistered.erase(it);
}

// FrameLib_FromHost Class

// Constructor

FrameLib_FromHost::FrameLib_FromHost(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 1, 1), mVectorFrame(NULL), mSerialFrame(NULL), mProxy(dynamic_cast<Proxy *>(proxy))
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kValues, "values");
    mParameters.addEnumItem(kParams, "params");
        
    mParameters.set(serialisedParameters);
    
    mMode = (Modes) mParameters.getValue(kMode);
    
    setOutputType(0, mMode == kValues ? kFrameNormal : kFrameTagged);
    
    if (mProxy)
        mProxy->registerObject(this);
}

FrameLib_FromHost::~FrameLib_FromHost()
{
    if (mProxy)
        mProxy->unregisterObject(this);
    delete mVectorFrame;
    delete mSerialFrame;
}

// Info

std::string FrameLib_FromHost::objectInfo(bool verbose)
{
    return formatInfo("Turn host messages into frames: In values mode the output is the last received value(s) as a vector. "
                   "In params mode messages are collected and output as a single tagged frame for setting parameters.",
                   "Turn host messages into frames.", verbose);
}

std::string FrameLib_FromHost::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Frame - triggers output", "Trigger Frame", verbose);
}

std::string FrameLib_FromHost::outputInfo(unsigned long idx, bool verbose)
{
    return "Output Frames";
}

// Parameter Info

FrameLib_FromHost::ParameterInfo FrameLib_FromHost::sParamInfo;

FrameLib_FromHost::ParameterInfo::ParameterInfo()
{
    add("Sets the object mode. "
        "values - translate values from max into vectors. "
        "params - translate messages into concatenated tagged frames to set parameters");
}

// Process

void FrameLib_FromHost::process()
{
    unsigned long size = 0;
    
    mLock.acquire();
    
    if (mMode == kValues)
    {
        requestOutputSize(0, mVectorFrame ? mVectorFrame->size(): 0);
        allocateOutputs();
        
        double *output = getOutput(0, &size);
        copyVector(output, &(*mVectorFrame)[0], size);
    }
    else
    {
        requestOutputSize(0, mSerialFrame ? mSerialFrame->size() : 0);
        allocateOutputs();
        
        FrameLib_Parameters::Serial *output = getOutput(0);
        
        if (output)
            output->write(mSerialFrame);
    }
    
    mLock.release();
}
