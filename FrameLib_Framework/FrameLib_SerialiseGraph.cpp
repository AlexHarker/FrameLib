
#include "FrameLib_SerialiseGraph.h"
#include "FrameLib_Export.h"

#include <sstream>
#include <fstream>
#include <cstdio>

#ifdef __GNUC__
#include <cxxabi.h>

void unmangleName(std::string& name, FrameLib_Object<FrameLib_Multistream> *obj)
{
    int status;
    
    const char *type_mangled_name = typeid(*obj).name();
    char *real_name = abi::__cxa_demangle(type_mangled_name, 0, 0, &status);
    
    name = real_name;
    free(real_name);
}
#else
void unmangleName(std::string& name, FrameLib_Object<FrameLib_Multistream> *obj)
{
    // FIX - needs implementing
    
    const char *type_mangled_name = typeid(*obj).name();
    name = type_mangled_name;
}
#endif

bool invalidPosition(size_t pos, size_t lo, size_t hi)
{
    return pos == std::string::npos || (pos < lo) || (pos > hi);
}

void removeCharacters(std::string& name, size_t pos, size_t count, size_t& end, size_t& removed)
{
    name.erase(pos, count);
    end -= count;
    removed += count;
}

size_t resolveFunctionType(std::string& name, size_t beg, size_t end)
{
    size_t searchPos;
    size_t removed = 0;
    
    // Now determine if there are parameters to deal with and if so erase them
    
    if (!invalidPosition(searchPos = name.find("(", beg), beg, end))
        removeCharacters(name, searchPos, 1 + end - searchPos, end, removed);
    
    // Reset search to the end character
    
    searchPos = end;

    // Look for a template at the end and match the beginning template bracket

    if (name[searchPos] == '>')
    {
        for (int bracketCount = 1; bracketCount; bracketCount = (name[searchPos] == '>') ? ++bracketCount : --bracketCount)
            if (invalidPosition(searchPos = name.find_last_of("<>", searchPos - 1), beg, end))
                return removed;
    }
        
    // Find a space (the beginning of the function name) and remove everything before it inclusive of the space
            
    if (!invalidPosition(searchPos = name.rfind(" ", searchPos - 1), beg, end))
        removeCharacters(name, beg, 1 + searchPos - beg, end, removed);
    
    return removed;
}

size_t findAndResolveFunctions(std::string& name, size_t beg, size_t end)
{
    size_t function1, function2;
    size_t removed = 0;
    
    while (1)
    {
        // Recurse across the string to find any ampersands followed by a bracket (which are the start of a function)
    
        if (invalidPosition(function1 = name.find("&(", beg), beg, end))
            return removed;
    
        function1 = function1 + 1;
        function2 = function1 + 1;
    
        // Find the balancing bracket
    
        for (int bracketCount = 1; bracketCount; bracketCount = (name[function2] == '(') ? ++bracketCount : --bracketCount)
            if (invalidPosition(function2 = name.find_first_of("()", function2 + 1), beg, end))
                return removed;
    
        // Erase the main brackets (last first to keep the positions correct)
        
        removeCharacters(name, function2, 1, end, removed);
        removeCharacters(name, function1, 1, end, removed);
        function2 -= 2;
        
        // Recurse downwards to resolve functions within functions
        
        size_t currentRemoved = findAndResolveFunctions(name, function1, function2);
        end -= currentRemoved;
        removed += currentRemoved;

        // We are now looking at a single function with no nested functions to resolve
        
        resolveFunctionType(name, function1, function2 - currentRemoved);
    }
}

void getTypeString(std::string& name, FrameLib_Object<FrameLib_Multistream> *obj)
{
	unmangleName(name, obj);

    // Resolve functions recursively
    
    findAndResolveFunctions(name, 0, name.length() - 1);
}

void serialiseGraph(std::vector<FrameLib_Object<FrameLib_Multistream> *>& serial, FrameLib_Multistream *object)
{
    if (std::find(serial.begin(), serial.end(), object) != serial.end())
        return;
    
    // First search up
    
    for (unsigned long i = 0; i < object->getNumIns(); i++)
    {
        FrameLib_Multistream::Connection connect = object->getConnection(i);
        if (connect.mObject) serialiseGraph(serial, connect.mObject);
    }
    
    for (unsigned long i = 0; i < object->getNumOrderingConnections(); i++)
    {
        FrameLib_Multistream::Connection connect = object->getOrderingConnection(i);
        if (connect.mObject) serialiseGraph(serial, connect.mObject);
    }
    
    // Then add at the end unless it's already been added
    
    if (std::find(serial.begin(), serial.end(), object) == serial.end())
        serial.push_back(object);
    
    // Then search down
    
    std::vector<FrameLib_Multistream *> outputDependencies;
    
    object->addOutputDependencies(outputDependencies);
    
    for (auto it = outputDependencies.begin(); it != outputDependencies.end(); it++)
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

void serialiseGraph(std::vector<FrameLib_ObjectDescription>& objects, FrameLib_Multistream *requestObject)
{
    std::vector<FrameLib_Object<FrameLib_Multistream> *> serial;
    unsigned long size = 0;
    const unsigned long kOrdering = -1;

    // Serialise object pointers
    
    serialiseGraph(serial, requestObject);
    
    for (auto it = serial.begin(); it != serial.end(); it++)
    {
        // Create a space and store the typename and number of streams
        
        FrameLib_Multistream *object = dynamic_cast<FrameLib_Multistream *>(*it);
        objects.push_back(FrameLib_ObjectDescription());
        FrameLib_ObjectDescription& description = objects.back();
        
        getTypeString(description.mObjectType, object);
        description.mNumStreams = object->getNumStreams();
        
        // Parameters
        
        const FrameLib_Parameters::Serial *parameters = object->getSerialised();
        
        if (parameters && object->getParameters())
        {
            for (auto it = parameters->begin(); it != parameters->end(); it++)
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
        
        for (unsigned long i = 0; i < object->getNumIns(); i++)
            addConnection(description, serial, object->getConnection(i), i);
        for (unsigned long i = 0; i < object->getNumOrderingConnections(); i++)
            addConnection(description, serial, object->getOrderingConnection(i), kOrdering);
    }
}

void serialiseVector(std::stringstream& output, size_t index, const char *type, size_t idx, const std::vector<double>& vector)
{
    if (!vector.size())
        return;
    
    output << exportIndent << "double fl_" << index << "_" << type << "_" << idx << "[] = { ";
    
    for (auto it = vector.begin(); it != vector.end(); it++)
    {
        const int strBufSize = 1024;
        char formatted[strBufSize];

        if (it != vector.begin())
            output << ", ";
        
        // FIX - need 100% accuracy but human readable
        
        if (round(*it) == *it)
            snprintf(formatted, strBufSize, "%lld", (long long) *it);
        else
            snprintf(formatted, strBufSize, "%lf", *it);
        output << formatted;
    }
    
    output << " };\n";
}

std::string serialiseGraph(FrameLib_Multistream *requestObject)
{
    std::vector<FrameLib_ObjectDescription> objects;
    std::stringstream output;

    serialiseGraph(objects, requestObject);

    output << exportIndent << "mObjects.resize(" << objects.size() <<");\n\n";

    for (auto it = objects.begin(); it != objects.end(); it++)
    {
        size_t index = it - objects.begin();
        
        // Params
        
        for (auto jt = it->mParameters.begin(); jt != it->mParameters.end(); jt++)
            serialiseVector(output, index, "vector", jt - it->mParameters.begin(), jt->mVector);

        output << exportIndent << "parameters.clear();\n";

        for (auto jt = it->mParameters.begin(); jt != it->mParameters.end(); jt++)
        {
            size_t idx = jt - it->mParameters.begin();
            if (jt->mType == kVector)
                output << exportIndent << "parameters.write(\"" << jt->mTag << "\", fl_" << index << "_vector_" << idx << ", " << jt->mVector.size() << ");\n";
            else
                output << exportIndent <<"parameters.write(\"" << jt->mTag << "\", \"" << jt->mString << "\");\n";
        }
        
        // Object declaration
        
        output << exportIndent << "mObjects[" << index << "] = new " << it->mObjectType << "(context, &parameters, mProxy, " << it->mNumStreams << ");\n";
        
        // Inputs
        
        for (auto jt = it->mInputs.begin(); jt != it->mInputs.end(); jt++)
            serialiseVector(output, index, "inputs", jt - it->mInputs.begin(), *jt);
    
        for (auto jt = it->mInputs.begin(); jt != it->mInputs.end(); jt++)
        {
            if (jt->size())
            {
                unsigned long idx = static_cast<unsigned long>(jt - it->mInputs.begin());
                output << exportIndent << "mObjects[" << index << "]->setFixedInput(" << idx << ", fl_" << index << "_inputs_" << idx <<" , " << jt->size() << ");\n";
            }
        }
        
        // Connections
        
        for (auto jt = it->mConnections.begin(); jt != it->mConnections.end(); jt++)
        {
            const unsigned long kOrdering = -1;

            if (jt->mInputIndex == kOrdering)
                output << exportIndent << "mObjects[" << index << "]->addOrderingConnection(Connection(mObjects[" << jt->mObjectIndex << "], " << jt->mOutputIndex << "));\n";
            else
                output << exportIndent << "mObjects[" << index << "]->addConnection(Connection(mObjects[" << jt->mObjectIndex << "], " << jt->mOutputIndex << "), " << jt->mInputIndex << ");\n";
        }
        
        output << "\n";
    }
    
    return output.str();
}

std::string exportClassName(const char *codeIn, const char *classname)
{
    std::string codeOut(codeIn);
    
    size_t pos = 0;

    while ((pos = codeOut.find("$")) != std::string::npos)
        codeOut.replace(pos, 1, classname);
    
    return codeOut;
}

ExportError exportWriteFile(std::stringstream& contents, const char *path, const char *className, const char *ext)
{
    std::string fileName(path);

    fileName.append("/");
    fileName.append(className);
    fileName.append(ext);
    
    std::ofstream file(fileName.c_str(), std::ofstream::out);
    
    if (!file.is_open())
        return kExportPathError;
    
    file << contents.rdbuf();
    file.close();
    
    return file.fail() ? kExportWriteError : kExportSuccess;
}

ExportError exportGraph(FrameLib_Multistream *requestObject, const char *path, const char *className)
{
    ExportError error = kExportSuccess;
    std::stringstream header, cpp;
    
    header << exportClassName(exportHeader, className);
    cpp << exportClassName(exportCPPOpen, className) << serialiseGraph(requestObject) << exportClassName(exportCPPClose, className);

    if ((error = exportWriteFile(header, path, className, ".h")))
        return error;
    
    if ((error = exportWriteFile(cpp, path, className, ".cpp")))
        return error;
    
    return kExportSuccess;
}
