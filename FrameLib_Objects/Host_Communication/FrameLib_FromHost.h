
#ifndef FRAMELIB_FROMHOST_H
#define FRAMELIB_FROMHOST_H

#include "FrameLib_DSP.h"
#include "FrameLib_HostProxy.h"

class FrameLib_FromHost final : public FrameLib_Processor
{
    // Typedef for concision
    
    typedef std::unique_ptr<std::vector<double>> OwnedFrame;
    
    // A FIFO list for storing parameter frame additions
    
    // Forward declaration
    
    struct SerialQueue;
    
    struct SerialItem : public FrameLib_Queue<SerialItem, SerialItem, SerialQueue>::Node
    {
        SerialItem(const FrameLib_Parameters::Serial& serial) : mSerial(serial) {}
        
        SerialItem(const SerialQueue& list)
        {
            for (SerialItem *item = list.mHead; item; item = item->mNext)
                mSerial.write(&item->mSerial);
        }
        
        FrameLib_Parameters::AutoSerial mSerial;
    };
    
    struct SerialQueue : public FrameLib_Queue<SerialItem, SerialItem, SerialQueue>
    {
        friend SerialItem;
        
        ~SerialQueue()
        {
            while (SerialItem *item = pop())
                delete item;
        }

        unsigned long memorySize() const
        {
            unsigned long summedSize = 0;
            
            for (SerialItem *item = mHead; item; item = item->mNext)
                summedSize += item->mSerial.size();
                
            return summedSize;
        }
    };
    
public:

    // The owner should inherit from this class and use these calls to send to all registered objects

    struct Proxy : public FrameLib_HostProxy<FrameLib_FromHost>
    {
        Proxy(bool copyStreams) : mCopyStreams(copyStreams) {}
        
        // Send a vector frame
        
        void sendFromHost(unsigned long index, const double *values, unsigned long N);
        void sendFromHost(unsigned long index, unsigned long stream, const double *values, unsigned long N);
        
        // Send a parameter frame
        
        void sendFromHost(unsigned long index, const FrameLib_Parameters::Serial *serial);
        void sendFromHost(unsigned long index, unsigned long stream, const FrameLib_Parameters::Serial *serial);
        
        // Send a parameter that takes a string
        
        void sendFromHost(unsigned long index, const char *tag, const char *string);
        void sendFromHost(unsigned long index, unsigned long stream, const char *tag, const char *string);
        
        // Send a parameter that takes a vector
        
        void sendFromHost(unsigned long index, const char *tag, const double *values, unsigned long N);
        void sendFromHost(unsigned long index, unsigned long stream, const char *tag, const double *values, unsigned long N);
        
        // Copy data from the first stream to another stream
        
        void copyData(void *streamOwner, unsigned long stream);
        
        bool mCopyStreams;
    };
    
private:
    
    // Parameter Info and Enums
    
    enum ParameterList { kMode };
    enum Modes { kValues, kParams };
    
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };

public:
    
    // Constructor / Destructor
    
    FrameLib_FromHost(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy);
    ~FrameLib_FromHost();
    
    // Stream Awareness
    
    void setStream(void *streamOwner, unsigned long stream) override;
    
    // Info
    
    std::string objectInfo(bool verbose) override;
    std::string inputInfo(unsigned long idx, bool verbose) override;
    std::string outputInfo(unsigned long idx, bool verbose) override;

private:
    
    void process() override;
    
    // Swapping data with the proxy
    
    OwnedFrame swapVectorFrame(OwnedFrame& swapVector);
    void updateSerialFrame(SerialQueue &freeList, SerialItem *addSerial);

// Data
    
    FrameLib_SpinLock mLock;
    OwnedFrame mVectorFrame;
    SerialQueue mSerialFrame;
    SerialQueue mSerialFreeFrame;
    Modes mMode;
    
    Proxy *mProxy;
    void *mStreamOwner;
    unsigned long mStream;
    
    static ParameterInfo sParamInfo;
};

#endif
