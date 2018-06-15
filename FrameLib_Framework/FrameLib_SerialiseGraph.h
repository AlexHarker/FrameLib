
#ifndef FrameLib_SerialiseGraph_h
#define FrameLib_SerialiseGraph_h

#include "FrameLib_Multistream.h"

enum ExportError { kExportSuccess, kExportPathError, kExportWriteError };

struct FrameLib_ObjectDescription
{
    FrameLib_ObjectDescription() : mNumStreams(0) {}
    
    struct Tagged
    {
        std::string mTag;
        DataType mType;
        std::string mString;
        std::vector<double> mVector;
    };
    
    struct Connection
    {
        Connection(unsigned long objectIdx, long outIdx, long inIdx) :
        mObjectIndex(objectIdx), mOutputIndex(outIdx), mInputIndex(inIdx) {}
        
        unsigned long mObjectIndex;
        unsigned long mOutputIndex;
        unsigned long mInputIndex;
    };
    
    std::string mObjectType;
    std::vector<Tagged> mParameters;
    std::vector<std::vector<double> > mInputs;
    std::vector<Connection> mConnections;
    
    unsigned long mNumStreams;
};

void serialiseGraph(std::vector<FrameLib_ObjectDescription>& objects, FrameLib_Multistream *requestObject);
std::string serialiseGraph(FrameLib_Multistream *requestObject);
ExportError exportGraph(FrameLib_Multistream *requestObject, const char *path,  const char *className);

#endif
