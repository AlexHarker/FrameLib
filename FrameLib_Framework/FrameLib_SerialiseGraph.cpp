
#include "FrameLib_SerialiseGraph.h"
#include "FrameLib_Export.h"

#include <cxxabi.h>
#include <sstream>
#include <fstream>

void getTypeString(std::string &name, FrameLib_Object<FrameLib_MultiChannel> *obj)
{
    int status;
    const char *type_mangled_name = typeid(*obj).name();
    char *real_name = abi::__cxa_demangle(type_mangled_name, 0, 0, &status);
    
    name = real_name;
    
    // Remove brackets
    
    size_t pos;
    
    while ((pos = name.find("(")) != std::string::npos)
        name.erase(pos, 1);
    
    while ((pos = name.find(")")) != std::string::npos)
        name.erase(pos, 1);
    
    free(real_name);
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

template <class T>
void addConnection(FrameLib_ObjectDescription& description, std::vector<FrameLib_Object<T> *> serial, typename FrameLib_Object<T>::Connection connect, unsigned long idx)
{
    if (connect.mObject)
    {
        size_t objectIdx = std::find(serial.begin(), serial.end(), connect.mObject) - serial.begin();
        description.mConnections.push_back(FrameLib_ObjectDescription::Connection(objectIdx, connect.mIndex, idx));
    }
}

void serialiseGraph(std::vector<FrameLib_ObjectDescription>& objects, FrameLib_MultiChannel *requestObject)
{
    std::vector<FrameLib_Object<FrameLib_MultiChannel> *> serial;
    unsigned long size = 0;

    // Serialise object pointers
    
    serialiseGraph(serial, requestObject);
    
    for (std::vector<FrameLib_Object<FrameLib_MultiChannel> *>::iterator it = serial.begin(); it != serial.end(); it++)
    {
        // Create a space and store the typename and number of streams
        
        FrameLib_MultiChannel *object = dynamic_cast<FrameLib_MultiChannel *>(*it);
        objects.push_back(FrameLib_ObjectDescription());
        FrameLib_ObjectDescription& description = objects.back();
        
        getTypeString(description.mObjectType, object);
        description.mNumStreams = object->getNumStreams();
        
        // Parameters
        
        const FrameLib_Parameters::Serial *parameters = object->getSerialised();
        
        if (parameters && object->getParameters())
        {
            for (FrameLib_Parameters::Serial::Iterator it = parameters->begin(); it != parameters->end(); it++)
            {
                long idx = object->getParameters()->getIdx(it.getTag());
                
                if (idx < 0)
                    continue;
                
                description.mParameters.push_back(FrameLib_ObjectDescription::Tagged());
                FrameLib_ObjectDescription::Tagged& tagged = description.mParameters.back();
                
                tagged.mTag = object->getParameters()->getName(idx);
                tagged.mType = it.getType();
                
                if (tagged.mType == kVector)
                {
                    const double *vector = it.getVector(&size);
                    tagged.mVector.assign(vector, vector + size);
                }
                else
                    tagged.mString = it.getString();
            }
        }
        
        // Inputs
        
        description.mInputs.resize(object->getNumIns());

        for (unsigned long i = 0; i < object->getNumIns(); i++)
        {
            if (!object->isConnected(i))
            {
                const double *vector = object->getFixedInput(i, &size);
                description.mInputs[i].assign(vector, vector + size);
            }
        }
        
        // Connections - Normal and Ordering
        
        for (int i = 0; i < object->getNumIns(); i++)
            addConnection(description, serial, object->getConnection(i), i);
        for (int i = 0; i < object->getNumOrderingConnections(); i++)
            addConnection(description, serial, object->getOrderingConnection(i), -1);
    }
}

void serialiseVector(std::stringstream& output, size_t index, const char *type, size_t idx, const std::vector<double>& vector)
{
    if (!vector.size())
        return;
    
    output << exportIndent << "double fl_" << index << "_" << type << "_" << idx << "[] = { ";
    
    for (std::vector<double>::const_iterator it = vector.begin(); it != vector.end(); it++)
    {
        char formatted[1024];

        if (it != vector.begin())
            output << ", ";
        
        // FIX - need 100% accuracy but human readable
        
        if (round(*it) == *it)
            sprintf(formatted, "%lld", (long long) *it);
        else
            sprintf(formatted, "%lf", *it);
        output << formatted;
    }
    
    output << " };\n";
}

std::string serialiseGraph(FrameLib_MultiChannel *requestObject)
{
    std::vector<FrameLib_ObjectDescription> objects;
    std::stringstream output;

    serialiseGraph(objects, requestObject);

    output << exportIndent << "mObjects.resize(" << objects.size() <<");\n\n";

    for (std::vector<FrameLib_ObjectDescription>::iterator it = objects.begin(); it != objects.end(); it++)
    {
        size_t index = it - objects.begin();
        
        // Params
        
        for (std::vector<FrameLib_ObjectDescription::Tagged>::iterator jt = it->mParameters.begin(); jt != it->mParameters.end(); jt++)
            serialiseVector(output, index, "vector", jt - it->mParameters.begin(), jt->mVector);

        output << exportIndent << "parameters.clear();\n";

        for (std::vector<FrameLib_ObjectDescription::Tagged>::iterator jt = it->mParameters.begin(); jt != it->mParameters.end(); jt++)
        {
            size_t idx = jt - it->mParameters.begin();
            if (jt->mType == kVector)
                output << exportIndent << "parameters.write(\"" << jt->mTag << "\", fl_" << index << "_vector_" << idx << ", " << jt->mVector.size() << ");\n";
            else
                output << exportIndent <<"parameters.write(\"" << jt->mTag << "\", \"" << jt->mString << "\");\n";
        }
        
        // Object declaration
        
        output << exportIndent << "mObjects[" << index << "] = new " << it->mObjectType << "(context, &parameters, owner," << it->mNumStreams << ");\n";
        
        // Inputs
        
        for (std::vector<std::vector<double> >::iterator jt = it->mInputs.begin(); jt != it->mInputs.end(); jt++)
            serialiseVector(output, index, "inputs", jt - it->mInputs.begin(), *jt);
    
        for (std::vector<std::vector<double> >::iterator jt = it->mInputs.begin(); jt != it->mInputs.end(); jt++)
        {
            if (jt->size())
            {
                unsigned long idx = jt - it->mInputs.begin();
                output << exportIndent << "mObjects[" << index << "]->setFixedInput(" << idx << ", fl_" << index << "_inputs_" << idx <<" , " << jt->size() << ");\n";
            }
        }
        
        // Connections
        
        for (std::vector<FrameLib_ObjectDescription::Connection>::iterator jt = it->mConnections.begin(); jt != it->mConnections.end(); jt++)
        {
            if (jt->mInputIndex == -1)
                output << exportIndent << "mObjects[" << index << "]->addOrderingConnection(Connection(mObjects[" << jt->mObjectIndex << "], " << jt->mOutputIndex << ");\n";
            else
                output << exportIndent << "mObjects[" << index << "]->addConnection(Connection(mObjects[" << jt->mObjectIndex << "], " << jt->mOutputIndex << "), " << jt->mInputIndex << ");\n";
        }
        
        output << "\n";
    }
    
    return output.str();
}

void exportReplaceClassName(std::string& code, const char *classname)
{
    size_t pos = 0;

    while ((pos = code.find("$")) != std::string::npos)
        code.replace(pos, 1, classname);
}

void exportFilePath(std::string& path, const char *className, const char *ext)
{
    path.append("/");
    path.append(className);
    path.append(ext);
}

void exportGraph(FrameLib_MultiChannel *requestObject, const char *path, const char *className)
{
    std::stringstream header;
    std::stringstream cpp;
    std::ofstream headerFile;
    std::ofstream cppFile;

    std::string constructor = serialiseGraph(requestObject);
    std::string headerName(path);
    std::string cppName(path);
    std::string headerContents(exportHeader);
    std::string cppOpenContents(exportCPPOpen);
    std::string cppCloseContents(exportCPPClose);

    exportFilePath(headerName, className, ".h");
    exportFilePath(cppName, className, ".cpp");
        
    exportReplaceClassName(headerContents, className);
    exportReplaceClassName(cppOpenContents, className);
    exportReplaceClassName(cppCloseContents, className);

    header << headerContents;
    cpp << cppOpenContents << constructor << cppCloseContents;
    
    headerFile.open(headerName.c_str(), std::ofstream::out);
    headerFile << header.rdbuf();;
    headerFile.close();
    
    cppFile.open(cppName.c_str(), std::ofstream::out);
    cppFile << cpp.rdbuf();;
    cppFile.close();
}
