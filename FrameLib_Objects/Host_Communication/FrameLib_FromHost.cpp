
#include "FrameLib_FromHost.h"
#include <algorithm>

// Proxy Class

// Send a vector frame

void FrameLib_FromHost::Proxy::sendFromHost(unsigned long index, const double *values, unsigned long N)
{
    const std::vector<FrameLib_FromHost *>& objects = getObjectList(index);
    std::vector<std::vector<double> *> swapVectors(objects.size());
    
    // Create one vector per object, swap and then delete the old vectors
    
    for (unsigned long i = 0; i < objects.size(); i++)
        swapVectors[i] = new std::vector<double>(values, values + N);
    
    for (unsigned long i = 0; i < objects.size(); i++)
        swapVectors[i] = objects[i]->swapVectorFrame(swapVectors[i]);
    
    for (unsigned long i = 0; i < objects.size(); i++)
        delete swapVectors[i];
}

void FrameLib_FromHost::Proxy::sendFromHost(unsigned long index, unsigned long stream, const double *values, unsigned long N)
{
    // Copy vector, swap and delete the old vector

    std::vector<double> *inputSwap = new std::vector<double>(values, values + N);
    getObject(index, stream)->swapVectorFrame(inputSwap);
    delete inputSwap;
}

void FrameLib_FromHost::Proxy::sendFromHost(unsigned long index, const FrameLib_Parameters::Serial *serial)
{
    SerialList freeList;
    const std::vector<FrameLib_FromHost *>& objects = getObjectList(index);
    std::vector<SerialList::Item *> addSerials(objects.size());

    // Create one serial structure per object, swap and then the free list will clear on destruct
    
    for (unsigned long i = 0; i < objects.size(); i++)
        addSerials[i] = new SerialList::Item(*serial);
    
    for (unsigned long i = 0; i < objects.size(); i++)
        objects[i]->updateSerialFrame(freeList, addSerials[i]);
}

void FrameLib_FromHost::Proxy::sendFromHost(unsigned long index, unsigned long stream, const FrameLib_Parameters::Serial *serial)
{
    // Copy serial date, update and then the free list will clear on destruct
    
    SerialList freeList;
    SerialList::Item *addSerial = new SerialList::Item(*serial);
    getObject(index, stream)->updateSerialFrame(freeList, addSerial);
}

// Send a parameter that takes a string

void FrameLib_FromHost::Proxy::sendFromHost(unsigned long index, const char *tag, const char *string)
{
    FrameLib_Parameters::AutoSerial serial(tag, string);
    sendFromHost(index, &serial);
}

void FrameLib_FromHost::Proxy::sendFromHost(unsigned long index, unsigned long stream, const char *tag, const char *string)
{
    FrameLib_Parameters::AutoSerial serial(tag, string);
    sendFromHost(index, stream, &serial);
}

// Send a parameter that takes a vector

void FrameLib_FromHost::Proxy::sendFromHost(unsigned long index, const char *tag, const double *values, unsigned long N)
{
    FrameLib_Parameters::AutoSerial serial(tag, values, N);
    sendFromHost(index, &serial);
}

void FrameLib_FromHost::Proxy::sendFromHost(unsigned long index, unsigned long stream, const char *tag, const double *values, unsigned long N)
{
    FrameLib_Parameters::AutoSerial serial(tag, values, N);
    sendFromHost(index, stream, &serial);
}

// FrameLib_FromHost Class

// Constructor

FrameLib_FromHost::FrameLib_FromHost(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 1, 1), mVectorFrame(NULL), mProxy(dynamic_cast<Proxy *>(proxy)), mStreamOwner(this), mStream(0)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kValues, "values");
    mParameters.addEnumItem(kParams, "params");
        
    mParameters.set(serialisedParameters);
    
    mMode = (Modes) mParameters.getValue(kMode);
    
    setOutputType(0, mMode == kValues ? kFrameNormal : kFrameTagged);
    
    if (mProxy)
        mProxy->registerObject(this, mStreamOwner, mStream);
}

FrameLib_FromHost::~FrameLib_FromHost()
{
    if (mProxy)
        mProxy->unregisterObject(this, mStreamOwner, mStream);
    delete mVectorFrame;
}

// Stream Awareness

void FrameLib_FromHost::setStream(void *streamOwner, unsigned long stream)
{
    if (mProxy)
    {
        mProxy->unregisterObject(this, mStreamOwner, mStream);
        mProxy->registerObject(this, streamOwner, stream);
    }
    
    mStreamOwner = streamOwner;
    mStream = stream;
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
        requestOutputSize(0, mSerialFrame.size());
        allocateOutputs();
        
        FrameLib_Parameters::Serial *output = getOutput(0);
        
        for (SerialList::Item *item = mSerialFrame.pop(); output && item; item = mSerialFrame.pop())
        {
            output->write(&item->mSerial);
            mSerialFreeFrame.push(item);
        }
    }
    
    mLock.release();
}

// Swap vector frame

std::vector<double> *FrameLib_FromHost::swapVectorFrame(std::vector<double> *swapVector)
{
    mLock.acquire();
    std::swap(mVectorFrame, swapVector);
    mLock.release();
    return swapVector;
}

// Swap vector frame

void FrameLib_FromHost::updateSerialFrame(SerialList &freeList, SerialList::Item *addSerial)
{
    mLock.acquire();
    mSerialFrame.push(addSerial);
    freeList.reassign(mSerialFreeFrame);
    mLock.release();
}
