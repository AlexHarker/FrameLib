
#ifndef FRAMELIB_HOSTPROXY_H
#define FRAMELIB_HOSTPROXY_H

template <class T>
class FrameLib_HostProxy : public virtual FrameLib_Proxy
{
    friend T;

    struct RefList
    {
        RefList(void *streamOwner) : mStreamOwner(streamOwner) {}
        
        void *mStreamOwner;
        std::vector<T *> mObjects;
    };
    
    const std::vector<T *>& getObjectList(unsigned long index) { return mRegistered[index].mObjects; }
    T *getObject(unsigned long index, unsigned long stream) { return mRegistered[index].mObjects[stream]; }

    unsigned long registerObject(T *object, void *streamOwner, unsigned long stream)
    {
        for (auto it = mRegistered.begin(); it != mRegistered.end(); it++)
        {
            // Find the owner first and if it exists insert into the stream list
            
            if (it->mStreamOwner == streamOwner)
            {
                if (it->mObjects.size() <= stream)
                    it->mObjects.resize(stream + 1);
                else
                    assert ((it->mObjects[stream] == NULL) && "stream is already registered");
                
                it->mObjects[stream] = object;
            
                return it - mRegistered.begin();
            }
        }

        // Create a new list for this object
        
        mRegistered.push_back(RefList(streamOwner));
        mRegistered.back().mObjects.push_back(object);
        
        return mRegistered.size() - 1;
    }
    
    void unregisterObject(T *object, void *streamOwner, unsigned long stream)
    {        
        for (auto it = mRegistered.begin(); it != mRegistered.end(); it++)
        {
            // Find the owner first and if it exists remove from the stream space
            
            if (it->mStreamOwner == streamOwner)
            {
                assert(it->mObjects[stream] == object && "object not registered to expected stream");
                
                it->mObjects[stream] = NULL;
                
                if (it->mObjects.size() == stream + 1)
                {
                    while (stream && it->mObjects[stream] == NULL)
                        stream--;
                    
                    if (stream)
                        it->mObjects.resize(stream);
                    else
                        mRegistered.erase(it);
                }
                
                return;
            }
        }
        
        assert("object not registered to expected stream owner");
    }
    
    // Data
    
    std::vector<RefList> mRegistered;
};

#endif
