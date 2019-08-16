
#ifndef FrameLib_SerialiseGraph_h
#define FrameLib_SerialiseGraph_h

#include "FrameLib_Multistream.h"

/**
 
 @defgroup Serialisation
 
 */

enum ExportError { kExportSuccess, kExportPathError, kExportWriteError };

/**
 
 @struct FrameLib_ObjectDescription
 
 @ingroup Serialisation
 
 @brief a data-based representation of a FrameLib object in a network, used for serialisation purposes.
 
 */

struct FrameLib_ObjectDescription
{
    /** Creates a new empty FrameLib_ObjectDescription object */
    
    FrameLib_ObjectDescription() : mNumStreams(0) {}
    
    /**
     
     @struct Tagged
     
     @brief a description of a single tagged string or vector of doubles.
     
     */
    
    struct Tagged
    {
        std::string mTag;               /**< The tag for the data */
        DataType mType;                 /**< The type of the tagged data (either kSingleString or kVector) */
        std::string mString;            /**< The string if mType is kSingleString */
        std::vector<double> mVector;    /**< The values of the vector if mType is kVector */
    };
    
    /**
     
     @struct Connection
     
     @brief a description of a connection to the input of the described object.
     
     */
    
    struct Connection
    {
        /** Creates a new Connection object
         * @param objectIdx an index representing the position of the connected object in a list
         * @param outIdx the index of the connected output of the connected object
         * @param inIdx the index of the connected input of this object
         */
        
        Connection(unsigned long objectIdx, long outIdx, long inIdx) :
        mObjectIndex(objectIdx), mOutputIndex(outIdx), mInputIndex(inIdx) {}
        
        unsigned long mObjectIndex;      /**< an index representing the position of the connected object in a list */
        unsigned long mOutputIndex;      /**< the index of the connected output of the connected object */
        unsigned long mInputIndex;       /**< the index of the connected input of this object */
    };
    
    std::string mObjectType;                    /**< the C++ object type as a string */
    std::vector<Tagged> mParameters;            /**< a vector/list of tagged parameter values */
    std::vector<std::vector<double>> mInputs;   /**< a vector/list of frames that represent fixed inputs of the object */
    std::vector<Connection> mConnections;       /**< a vector/list of connections to inputs of this object */
    
    unsigned long mNumStreams;                  /**< the number of explicit streams for this object */
};

/**
 
 @ingroup Serialisation

 @brief serialised a graph as a std::vector of FrameLib_ObjectDescription structs from a pointer to any object in the graph.
 
 @param objects a reference to a std::vector<FrameLib_ObjectDescription> in which to return the serialised description.
 @param requestObject a pointer to any pointer to a FrameLib_Multistream object within a graph.

 */

void serialiseGraph(std::vector<FrameLib_ObjectDescription>& objects, FrameLib_Multistream *requestObject);

/**
 
 @ingroup Serialisation

 @brief export C++ code for a graph from a pointer to any object in the graph.
 
 @param requestObject a pointer to any pointer to a FrameLib_Multistream object within a graph.
 @param path a C-string containing the path in which to write the output files.
 @param className a C-string containing the name of the class to be created in the exported code.
 @return an ExportError indicating if any errors have occured.
 
 */

ExportError exportGraph(FrameLib_Multistream *requestObject, const char *path,  const char *className);

#endif
