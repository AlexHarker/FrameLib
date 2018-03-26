
#include "FrameLib_SerialiseGraph.h"

#include <cxxabi.h>

void getTypeString(std::string &name, FrameLib_Object<FrameLib_MultiChannel> *obj)
{
    int status;
    const char *type_mangled_name = typeid(*obj).name();
    char *real_name = abi::__cxa_demangle(type_mangled_name, 0, 0, &status);
    
    name = real_name;
    
    free(real_name);
}

size_t getSerialNum(std::vector<FrameLib_Object<FrameLib_MultiChannel> *>& serialised, FrameLib_Object<FrameLib_MultiChannel> *obj)
{
    return 1 + std::find(serialised.begin(), serialised.end(), obj) - serialised.begin();
}

void serialiseGraph(std::vector<FrameLib_Object<FrameLib_MultiChannel> *>& serial, FrameLib_MultiChannel *object)
{
    if (std::find(serial.begin(), serial.end(), object) != serial.end())
        return;
    
    // First search up
    
    for (int i = 0; i < object->getNumIns(); i++)
    {
        FrameLib_MultiChannel::Connection connect = object->getConnection(i);
        if (connect.mObject) serialiseGraph(serial, connect.mObject);
    }
    
    for (int i = 0; i < object->getNumOrderingConnections(); i++)
    {
        FrameLib_MultiChannel::Connection connect = object->getOrderingConnection(i);
        if (connect.mObject) serialiseGraph(serial, connect.mObject);
    }
    
    // Then add at the end unless it's already been added
    
    if (std::find(serial.begin(), serial.end(), object) == serial.end())
        serial.push_back(object);
    
    // Then search down
    
    std::vector<FrameLib_MultiChannel *> outputDependencies;
    
    object->addOutputDependencies(outputDependencies);
    
    for (typename std::vector<FrameLib_MultiChannel *>::iterator it = outputDependencies.begin(); it != outputDependencies.end(); it++)
        serialiseGraph(serial, *it);
}

void serialiseGraph(std::string& described, FrameLib_MultiChannel *object)
{
    std::vector<FrameLib_Object<FrameLib_MultiChannel> *> serial;
    char stringFormatted[1024];
    
    serialiseGraph(serial, object);
    
    for (std::vector<FrameLib_Object<FrameLib_MultiChannel> *>::iterator it = serial.begin(); it != serial.end(); it++)
    {
        FrameLib_Object<FrameLib_MultiChannel> *obj = *it;
        std::string main_name;
        size_t main_num = getSerialNum(serial, obj);

        getTypeString(main_name, obj);
        
        sprintf(stringFormatted, "fl%ld = %s\n", main_num, main_name.c_str());
        described.append(stringFormatted);
        
        for (int i = 0; i < obj->getNumIns(); i++)
        {
            FrameLib_Object<FrameLib_MultiChannel>::Connection connect = obj->getConnection(i);
            if (connect.mObject)
            {
                sprintf(stringFormatted, "fl%ld.addConnection(Connection(fl%ld, %ld) %d)\n", main_num, getSerialNum(serial, connect.mObject), connect.mIndex, i);
                described.append(stringFormatted);
            }
        }
        
        for (int i = 0; i < obj->getNumOrderingConnections(); i++)
        {
            FrameLib_Object<FrameLib_MultiChannel>::Connection connect = obj->getOrderingConnection(i);
            if (connect.mObject)
            {
                sprintf(stringFormatted, "fl%ld.addOrderingConnection(Connection(fl%ld, %ld))\n", main_num, getSerialNum(serial, connect.mObject), connect.mIndex);
                described.append(stringFormatted);
            }
        }
    }
}
