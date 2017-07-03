

#ifndef FRAMELIB_CONNECTIONQUEUE_H
#define FRAMELIB_CONNECTIONQUEUE_H

class FrameLib_ConnectionQueue
{
    
public:
    
    class Item
    {
        friend FrameLib_ConnectionQueue;
        
    public:
        
        Item() : mNext(NULL) {}
        
    private:
        
        // Override to deal with changes in IO (inputUpdate() returns true is output size has changed)
        
        virtual bool inputUpdate() = 0;
        virtual void outputUpdate() = 0;
        
        Item *mNext;
    };
    
    FrameLib_ConnectionQueue() : mTop(NULL), mTail(NULL) {}
    
    void add(Item *object);
    
private:
    
    Item *mTop;
    Item *mTail;
};

#endif /* FRAMELIB_CONNECTIONQUEUE_H */
