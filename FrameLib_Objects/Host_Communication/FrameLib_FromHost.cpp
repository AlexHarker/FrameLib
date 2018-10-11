
#include "FrameLib_FromHost.h"
#include <algorithm>

// Proxy Class

// Send a vector frame

void FrameLib_FromHost::Proxy::sendFromHost(unsigned long index, const double *values, unsigned long N)
{
    const std::vector<FrameLib_FromHost *>& objects = getObjectList(index);
    std::vector<OwnedFrame> swapVectors(objects.size());
    
    // Create one vector per object, then swap (deconstructing the old ones in this thread)
    
    for (unsigned long i = 0; i < objects.size(); i++)
        swapVectors[i] = OwnedFrame(new std::vector<double>(values, values + N));
    
    for (unsigned long i = 0; i < objects.size(); i++)
        swapVectors[i] = objects[i]->swapVectorFrame(swapVectors[i]);
}

void FrameLib_FromHost::Proxy::sendFromHost(unsigned long index, unsigned long stream, const double *values, unsigned long N)
{
    // Copy vector and swap (deconstructing the old vector in this thread)

    OwnedFrame inputSwap(new std::vector<double>(values, values + N));
    getObject(index, stream)->swapVectorFrame(inputSwap);
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

// Copy data from the first stream

void FrameLib_FromHost::Proxy::copyData(void *streamOwner, unsigned long stream)
{
    if (stream && mCopyStreams)
    {
        FrameLib_FromHost *first = getObject(streamOwner, 0);
        FrameLib_FromHost *current = getObject(streamOwner, stream);
        
        if (first->mMode == kValues && current->mMode == kValues)
        {
            first->mLock.acquire();
            OwnedFrame frame(new std::vector<double>(*first->mVectorFrame.get()));
            first->mLock.release();
            current->swapVectorFrame(frame);
        }
        else if (first->mMode == kParams && current->mMode == kParams)
        {
            FrameLib_Parameters::AutoSerial serial;
            SerialList freeList;

            first->mLock.acquire();
            SerialList::Item *addSerial = new SerialList::Item(first->mSerialFrame);
            first->mLock.release();
            current->updateSerialFrame(freeList, addSerial);
        }
    }
}

// FrameLib_FromHost Class

// Constructor

FrameLib_FromHost::FrameLib_FromHost(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy) : FrameLib_Processor(context, proxy, &sParamInfo, 1, 1), mProxy(dynamic_cast<Proxy *>(proxy)), mStreamOwner(this), mStream(0)
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
}

// Stream Awareness

void FrameLib_FromHost::setStream(void *streamOwner, unsigned long stream)
{
    void *prevOwner = mStreamOwner;
    unsigned long prevStream = mStream;
    mStreamOwner = streamOwner;
    mStream = stream;
    
    if (mProxy)
    {
        mProxy->unregisterObject(this, prevOwner, prevStream);
        mProxy->registerObject(this, streamOwner, stream);
        mProxy->copyData(streamOwner, stream);
    }
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
    
    FrameLib_SpinLockHolder lock(&mLock);
    
    if (mMode == kValues)
    {
        requestOutputSize(0, mVectorFrame ? mVectorFrame->size(): 0);
        allocateOutputs();
        
        double *output = getOutput(0, &size);
        copyVector(output, mVectorFrame ? mVectorFrame->data() : nullptr, size);
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
}

// Swap vector frame

FrameLib_FromHost::OwnedFrame FrameLib_FromHost::swapVectorFrame(OwnedFrame& swapVector)
{
    FrameLib_SpinLockHolder lock(&mLock);
    std::swap(mVectorFrame, swapVector);
    return std::move(swapVector);
}

// Swap vector frame

void FrameLib_FromHost::updateSerialFrame(SerialList &freeList, SerialList::Item *addSerial)
{
    FrameLib_SpinLockHolder lock(&mLock);
    mSerialFrame.push(addSerial);
    freeList.reassign(mSerialFreeFrame);
}
