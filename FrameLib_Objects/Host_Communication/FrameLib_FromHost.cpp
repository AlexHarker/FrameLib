
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
    SerialQueue freeQueue;
    const std::vector<FrameLib_FromHost *>& objects = getObjectList(index);
    std::vector<SerialItem *> addSerials(objects.size());

    // Create one serial structure per object, swap and then the free queue will clear on destruct
    
    for (unsigned long i = 0; i < objects.size(); i++)
        addSerials[i] = new SerialItem(*serial);
    
    for (unsigned long i = 0; i < objects.size(); i++)
        objects[i]->updateSerialFrame(freeQueue, addSerials[i]);
}

void FrameLib_FromHost::Proxy::sendFromHost(unsigned long index, unsigned long stream, const FrameLib_Parameters::Serial *serial)
{
    // Copy serial date, update and then the free queue will clear on destruct
    
    SerialQueue freeQueue;
    SerialItem *addSerial = new SerialItem(*serial);
    getObject(index, stream)->updateSerialFrame(freeQueue, addSerial);
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
    void *firstOwner = mRegistered[0].mStreamOwner;
    bool copyStream = stream && mCopyStreams;
    bool copyFirstOwner = !stream && mCopyFirstOwner && firstOwner != streamOwner;
    
    if (copyStream || copyFirstOwner)
    {
        FrameLib_FromHost *first = getObject(copyFirstOwner ? firstOwner : streamOwner, 0);
        FrameLib_FromHost *current = getObject(streamOwner, stream);
        
        if (first->mMode == kValues && current->mMode == kValues)
        {
            first->mLock.acquire();
            std::vector<double> *firstFrame = first->mVectorFrame.get();
            OwnedFrame frame(firstFrame ? new std::vector<double>(*firstFrame) : nullptr);
            first->mLock.release();
            current->swapVectorFrame(frame);
        }
        else if (first->mMode == kParams && current->mMode == kParams)
        {
            FrameLib_Parameters::AutoSerial serial;
            SerialQueue freeQueue;

            first->mLock.acquire();
            SerialItem *addSerial = first->mSerialFrame.empty() ? nullptr : new SerialItem(first->mSerialFrame);
            first->mLock.release();
            if (addSerial)
                current->updateSerialFrame(freeQueue, addSerial);
        }
    }
}

// FrameLib_FromHost Class

// Constructor

FrameLib_FromHost::FrameLib_FromHost(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
: FrameLib_Processor(context, proxy, &sParamInfo, 1, 1)
, mProxy(castProxy<Proxy>(proxy))
, mStreamOwner(this)
, mStream(0)
{
    mParameters.addEnum(kMode, "mode", 0);
    mParameters.addEnumItem(kValues, "values");
    mParameters.addEnumItem(kParams, "params");
    mParameters.setInstantiation();
    
    mParameters.set(serialisedParameters);
    
    mMode = mParameters.getEnum<Modes>(kMode);
    
    setOutputType(0, mMode == kValues ? FrameType::Vector : FrameType::Tagged);
    
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
    return formatInfo("Turn messages or control signals from the host into frames: "
                      "In values mode the output is the last received value(s) as a vector. "
                      "In params mode messages are collected and output as a single tagged frame. "
                      "Typically params mode is used to help set object parameters. "
                      "Output is triggered by frames arriving at the trigger input.",
                      "Turn messages or control signals from the host into frames.", verbose);
}

std::string FrameLib_FromHost::inputInfo(unsigned long idx, bool verbose)
{
    return formatInfo("Trigger Input - triggers output", "Trigger Input", verbose);
}

std::string FrameLib_FromHost::outputInfo(unsigned long idx, bool verbose)
{
    return "Output";
}

// Parameter Info

FrameLib_FromHost::ParameterInfo FrameLib_FromHost::sParamInfo;

FrameLib_FromHost::ParameterInfo::ParameterInfo()
{
    add("Sets the mode: "
        "values - translate values from the host into vectors. "
        "params - translate host messages into concatenated tagged frames.");
}

// Process

void FrameLib_FromHost::process()
{
    unsigned long size = 0;
    
    FrameLib_SpinLockHolder lock(&mLock);
    
    if (mMode == kValues)
    {
        requestOutputSize(0, mVectorFrame ? static_cast<unsigned long>(mVectorFrame->size()): 0);
        allocateOutputs();
        
        double *output = getOutput(0, &size);
        copyVector(output, mVectorFrame ? mVectorFrame->data() : nullptr, size);
    }
    else
    {
        requestOutputSize(0, mSerialFrame.memorySize());
        allocateOutputs();
        
        FrameLib_Parameters::Serial *output = getOutput(0);
        
        if (output)
        {
            while (SerialItem *item = mSerialFrame.pop())
            {
                output->write(&item->mSerial);
                mSerialFreeFrame.push(item);
            }
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

void FrameLib_FromHost::updateSerialFrame(SerialQueue &freeQueue, SerialItem *addSerial)
{
    FrameLib_SpinLockHolder lock(&mLock);
    mSerialFrame.push(addSerial);
    freeQueue.transfer(mSerialFreeFrame);
}
