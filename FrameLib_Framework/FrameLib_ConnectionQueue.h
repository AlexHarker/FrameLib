

#ifndef FRAMELIB_CONNECTIONQUEUE_H
#define FRAMELIB_CONNECTIONQUEUE_H

// Forwar Declarations

class FrameLib_MultiChannel;

class FrameLib_ConnectionQueue
{
    
public:
    
    FrameLib_ConnectionQueue() : mTop(NULL), mTail(NULL) {}
    
    void add(FrameLib_MultiChannel *object);
    
private:
    
    FrameLib_MultiChannel *mTop;
    FrameLib_MultiChannel *mTail;
};


#endif /* FRAMELIB_CONNECTIONQUEUE_H */
