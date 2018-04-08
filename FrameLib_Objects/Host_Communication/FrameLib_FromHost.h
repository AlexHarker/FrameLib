
#ifndef FRAMELIB_FROMHOST_H
#define FRAMELIB_FROMHOST_H

#include "FrameLib_DSP.h"
#include "FrameLib_HostProxy.h"

class FrameLib_FromHost : public FrameLib_Processor
{
    // A FIFO list for storing parameter frame additions
    
    struct SerialList
    {
        // N.B. - copy construction is allowed, but doesn't copy anything, so we can construct vectors
        
        SerialList() : mTop(NULL), mTail(NULL) {}
        SerialList(const SerialList&) : mTop(NULL), mTail(NULL) {}
        ~SerialList() { clear(); }
        
        struct Item
        {
            Item() : mNext(NULL) {}
            Item(const FrameLib_Parameters::Serial& serial) : mSerial(serial), mNext(NULL) {}

            FrameLib_Parameters::AutoSerial mSerial;
            Item *mNext;
        };
        
        void push(Item *item)
        {
            assert (item->mNext == NULL && "item already in a list");
            
            if (mTail)
            {
                mTail->mNext = item;
                mTail = item;
            }
            else
                mTop = mTail = item;
        }
        
        Item *pop()
        {
            Item *item = mTop;
            
            mTop = mTop ? mTop->mNext : NULL;
            mTail = (mTail == item) ? NULL : mTail;
            
            if (item)
                item->mNext = NULL;
            
            return item;
        }
        
        size_t size()
        {
            unsigned long summedSize = 0;
            
            for (Item *item = mTop; item; item = item->mNext)
                summedSize += item->mSerial.size();
                
            return summedSize;
        }
        
        void clear()
        {
            for (Item *item = pop(); item; item = pop())
                delete item;
        }
        
        void reassign(SerialList& list)
        {
            mTop = list.mTop;
            mTail = list.mTail;
            list.mTop = NULL;
            list.mTail = NULL;
        }
        
    private:
        
        // Deleted
        
        SerialList& operator=(const SerialList&);
        
        // Data
        
        Item *mTop;
        Item *mTail;
    };
    
public:

    // The owner should inherit from this class and use these calls to send to all registered objects

    struct Proxy : public FrameLib_HostProxy<FrameLib_FromHost>
    {
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
    
    virtual void setStream(void *streamOwner, unsigned long stream);
    
    // Info
    
    std::string objectInfo(bool verbose);
    std::string inputInfo(unsigned long idx, bool verbose);
    std::string outputInfo(unsigned long idx, bool verbose);

private:
    
    void process();
    
    // Swapping data with the proxy
    
    void swapVectorFrame(std::vector<double> *swapVector);
    void updateSerialFrame(SerialList &freeList, SerialList::Item *addSerial);

// Data
    
    SpinLock mLock;
    std::vector<double> *mVectorFrame;
    SerialList mSerialFrame;
    SerialList mSerialFreeFrame;
    Modes mMode;
    
    Proxy *mProxy;
    void *mStreamOwner;
    unsigned long mStream;
    
    static ParameterInfo sParamInfo;
};

#endif
