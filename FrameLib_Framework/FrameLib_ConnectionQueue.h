

#ifndef FRAMELIB_CONNECTIONQUEUE_H
#define FRAMELIB_CONNECTIONQUEUE_H

class FrameLib_ConnectionQueue
{
    
public:
    
    struct Item
    {        
        Item() : mNext(NULL) {}
        
        // Override to deal with changes in the input connections
        
        virtual void inputUpdate() = 0;
        
        Item *mNext;
    };
    
    FrameLib_ConnectionQueue() : mTop(NULL), mTail(NULL) {}
    
    void add(Item *object);
    
private:
    
    Item *mTop;
    Item *mTail;
};


#endif /* FRAMELIB_CONNECTIONQUEUE_H */
