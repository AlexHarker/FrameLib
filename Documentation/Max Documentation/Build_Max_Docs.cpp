
// fl includes

#include "Max_Object_List.h"

#include "FrameLib_Global.h"
#include "FrameLib_Context.h"
#include "FrameLib_Multistream.h"

// stl

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <libgen.h>

struct MessageArgument
{
    std::string mName;
    bool mOptional;
    std::string mType;
};

void findReplace(std::string& str, const char *findStr, const char *replaceStr)
{
    for (size_t startPos = str.find(findStr); startPos != std::string::npos; startPos = str.find(findStr))
        str.replace(startPos, strlen(findStr), replaceStr);
}

std::string formatInfo(std::string str)
{
    findReplace(str, ". ", ".<br />");
    findReplace(str, ": ", ":<br /><br />");
 
    return str;
}

std::string argumentName(std::string str)
{
    findReplace(str, " ", "-");
    
    return str;
}

std::string escapeXML(std::string str)
{
    std::string::size_type pos = 0;
    
    while (pos != std::string::npos)
    {
        pos = str.find_first_of("\"&<>'", pos);
        
        if (pos == std::string::npos)
            break;
        
        std::string replacement;
        
        switch (str[pos])
        {
            case '&':  replacement = "&amp;";  break;
            case '<':  replacement = "&lt;";   break;
            default: ;
        }
        
        str.replace(pos, 1, replacement);
        pos += replacement.size();
    }
    
    return str;
}

bool writeInfo(FrameLib_Multistream* frameLibObject, std::string inputName, MaxObjectArgsMode argsMode)
{
    std::string fileName(__FILE__);
    std::string dirPath = dirname(const_cast<char *>(fileName.c_str()));
    std::string tmpFolder = dirPath + "/__tmp__/raw_xml/";
    
    enum InfoFlags { kInfoDesciption = 0x01, kInfoInputs = 0x02, kInfoOutputs = 0x04, kInfoParameters = 0x08 };
    
    std::ofstream file;
    std::string sp = " ";               // code is more readable with sp rather than " "
    std::string object = inputName;     // refactor to not copy variable.
    
    std::string objectCategory = "!@#@#$";
    std::string objectKeywords = "boilerplate keywords";
    std::string objectInfo;
    std::string objectDescription;
    std::string objectDigest;
    
    // Store some indentations - groups of 4 spaces
    
    std::string tab1 = "    ";
    std::string tab2 = tab1 + tab1;
    std::string tab3 = tab2 + tab1;
    std::string tab4 = tab2 + tab2;
    
    // Write to a temporary relative location
    
    file.open(tmpFolder + object + ".maxref.xml");
    
    const FrameLib_Parameters *params = frameLibObject->getParameters();
    
    auto toLower = [](std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
        return s;
    };
    
    auto writeDigestDescription = [&](const std::string& tab, const std::string& digest, const std::string& description)
    {
        file << tab + "<digest> \n";
        file << tab + tab1 + digest + "\n";
        file << tab + "</digest> \n";
        file << tab + "<description> \n";
        file << tab + tab1 + description + "\n";
        file << tab + "</description> \n";
    };
    
    auto writeAttribute = [&](const char *name, const char *type, const char *digest, const char *description, const char *label)
    {
        file << tab2 + "<attribute name='" + name + "' get='1' set='1' type='"+ type + "' size='1'> \n";
        writeDigestDescription(tab3, digest, description);
        file << tab3 + "<attributelist> \n";
        file << tab4 + "<attribute name='basic' get='1' set='1' type='int' size='1' value='1' /> \n";
        file << tab4 + "<attribute name='label' get='1' set='1' type='symbol' size='1' value='" + label + "' />\n";
        file << tab3 + "</attributelist> \n";
        file << tab2 + "</attribute> \n";
    };
    
    auto writeArgument = [&](unsigned long idx)
    {
        long pIdx = -1;
        
        for (unsigned long i = 0; params && i < params->size(); i++)
            if (params->getArgumentIdx(i) == idx)
                pIdx = static_cast<long>(i);
        
        if (pIdx == -1)
        {
            if (idx)
                file << tab1 + "</objarglist> \n \n";
            else
                file << tab1 + "<objarglist /> \n \n";
            return false;
        }
        
        if (!idx)
            file << tab1 + "<objarglist> \n";
        
        std::string type = "number";
        
        if (params->getNumericType(pIdx) == FrameLib_Parameters::NumericType::Integer)
            type = "int";
        else if (params->getType(pIdx) == FrameLib_Parameters::Type::Enum)
            type = "symbol";
        else if (params->getType(pIdx) == FrameLib_Parameters::Type::String)
            type = "symbol";
        
        std::string rawDescription = escapeXML(params->getInfo(pIdx));
        std::string name = params->getName(pIdx);
        std::string digest = rawDescription.substr(0, rawDescription.find_first_of(".:"));
        std::string description = formatInfo(rawDescription);
        
        file << tab2 + "<objarg name='" + name + "' optional='1' type='" + type + "'> \n";
        writeDigestDescription(tab3, digest, description);
        file << tab2 + "</objarg> \n";
        
        return true;
    };
    
    auto writeArgumentsAllInputs = [&]()
    {
        std::string digest("The input vector to use for any disconnected inputs");
        std::string description("Values typed as arguments will be used as a vector for any inputs that are not connected. Either single values or multi-valued vectors can be entered. The behaviour is similar to that for arguments to standard objects such as +~, *~ or zl.reg.");
        
        file << tab1 + "<objarglist> \n";
        file << tab2 + "<objarg name='default-input' optional='1' type='list'> \n";
        writeDigestDescription(tab3, digest, description);
        file << tab2 + "</objarg> \n";
        file << tab1 + "</objarglist> \n \n";
    };
    
    auto writeArgumentsDistributed = [&]()
    {
        file << tab1 + "<objarglist> \n";
        
        for (unsigned long i = 1; i < frameLibObject->getNumIns(); i++)
        {
            std::string inputName = toLower(frameLibObject->inputInfo(i));
            std::string digest("The value to use for input " + std::to_string(i + 1) +  " if it is disconnected");
            std::string description("Sets a single value for " + inputName);
            
            file << tab2 + "<objarg name='" + argumentName(inputName) + "' optional='1' type='number'> \n";
            writeDigestDescription(tab3, digest, description);
            file << tab2 + "</objarg> \n";
        }
        
        file << tab1 + "</objarglist> \n \n";
    };
    
    auto writeMessage = [&](const char *name, const char *digest, const char *description, const std::vector<MessageArgument>& args)
    {
        file << tab2 + "<method name='" + name + "'> \n";
        
        if (args.size())
        {
            file << tab3 + "<arglist> \n";
            for (auto it = args.cbegin(); it != args.cend(); it++)
                file << tab4 + "<arg name='" + it->mName + "' optional='" + std::to_string(it->mOptional) + "' type='" + it->mType + "' /> \n";
            file << tab3 + "</arglist> \n";
        }
        else
            file << tab3 + "<arglist /> \n";
        writeDigestDescription(tab3, digest, description);
        
        file << tab2 + "</method> \n";
    };
    
    // Check that the file has opened correctly
    
    if (!file.is_open())
        return false;
    
    // Write some stuff at the top of every xml file
    
    file << "<?xml version='1.0' encoding='utf-8' standalone='yes'?> \n" ;
    file << "<?xml-stylesheet href='./_c74_ref.xsl' type='text/xsl'?> \n \n" ;
    file << "<c74object name='" << object << "' " << "module='FrameLib' " << "category=" << "'" << objectCategory << std::string("'> ") << "\n \n";
    
    // Description
    
    // Split the object info into a description and a digest
    
    objectInfo = escapeXML(frameLibObject->objectInfo(true));
    std::size_t pos = objectInfo.find_first_of(":.");
    objectDigest = objectInfo.substr(0, pos) + ".";
    objectDescription = objectInfo.substr(pos + 1);
    
    // Now write that info into sections
    
    writeDigestDescription(tab1, objectDigest, objectDescription);
    
    // Parameters
    
    file << tab1 + "<!--PARAMETERS-->\n";
    
    if (!params || !params->size())
    {
        // If object has no parameters create the 'no parameters template'
        
        file << tab1 + "<misc name = 'Parameters'> \n";
        file << tab2 + "<entry> \n";
        file << tab3 + "<description> \n";
        file << tab4 + "This object has no parameters. \n";
        file << tab3 + "</description> \n";
        file << tab2 + "</entry> \n";
        file << tab1 + "</misc> \n \n";
    }
    else
    {
        // Write parameters tag to start misc section named Parameters
        
        file << tab1 + "<misc name = 'Parameters'> \n";
        for (int i = 0; params && i < params->size(); i++)
        {
            FrameLib_Parameters::Type type = params->getType(i);
            //FrameLib_Parameters::NumericType numericType = params->getNumericType(i);
            std::string defaultStr = params->getDefaultString(i);
            
            // Name, type and default value
            
            if (defaultStr.size())
                file << tab2 + "<entry name = '/" + params->getName(i) + " [" + params->getTypeString(i) + "]' > \n";
            else
                file << tab2 + "<entry name = '/" + params->getName(i) + " [" + params->getTypeString(i) + "]' > \n";
            
            // Construct the description
            
            file << tab3 + "<description> \n";
            file << tab4 + escapeXML(params->getInfo(i));
            
            if (type == FrameLib_Parameters::Type::Enum)
            {
                file << "<br></br> \n" ; // if enum put a break big break between description and the enum options
                
                for (long j = 0; j <= params->getMax(i); j++)
                {
                    std::string enumParamNum = std::to_string(j);
                    
                    if (j == params->getMax(i))
                        file << tab4 + "<bullet>[" + enumParamNum + "]" + " - " + params->getItemString(i, j) + "</bullet>";
                    else if (j != params->getMax(i))
                        file << tab4 + "<bullet>[" + enumParamNum + "]" + " - " + params->getItemString(i, j) + "</bullet> \n";
                }
            }
            file << "\n" + tab3 + "</description> \n";
            file << tab2 + "</entry> \n";
        }
        file << tab1 + "</misc> \n \n";
    }
    
    // Metadata
    
    file << tab1 + "<!--METADATA-->\n";
    file << tab1 + "<metadatalist> \n";
    file << tab2 + "<metadata name='author'>Alex Harker</metadata> \n";
    file << tab2 + "<metadata name='tag'>FrameLib</metadata> \n";
    file << tab2 + "<metadata name='tag'>" + objectCategory + "</metadata> \n";
    file << tab1 + "</metadatalist> \n \n";
    
    // Arguments
    
    file << tab1 + "<!--ARGUMENTS-->\n";
    switch (argsMode)
    {
        case kAsParams:
            for (unsigned long i = 0; writeArgument(i); i++);
            break;
            
        case kAllInputs:
            writeArgumentsAllInputs();
            break;
            
        case kDistribute:
            writeArgumentsDistributed();
            break;
    }
    
    // Messages
    
    std::vector<MessageArgument> emptyArgs;
    std::vector<MessageArgument> infoArgs { { "items", true, "list"} };
    std::vector<MessageArgument> resetArgs { { "samplerate", true, "number" } };
    std::vector<MessageArgument> processArgs { { "length", false, "int" } };
    
    file << tab1 + "<!--MESSAGES-->\n";
    file << tab1 + "<methodlist> \n";
    writeMessage("info", "Get Object Info", "--detail--", infoArgs);
    if (frameLibObject->handlesAudio())
    {
        writeMessage("process", "Process in non-realtime", "--detail--", processArgs);
        writeMessage("reset", "Reset a non-realtime network", "--detail--", resetArgs );
        writeMessage("signal", "Synchronise with audio or accept signal IO", "--detail--", emptyArgs );
    }
    writeMessage("frame", "Connect FrameLib objects", "Used internally by FrameLib connection routines. User messages have no effect", emptyArgs);
    writeMessage("sync", "Synchronise FrameLib audio objects", "Used internally by FrameLib connection routines. User messages have no effect", emptyArgs);
    file << tab1 + "</methodlist> \n \n";
    
    // Attributes
    
    file << tab1 + "<!--ATTRIBUTES-->\n";
    file << tab1 + "<attributelist> \n";
    if (frameLibObject->handlesAudio())
        writeAttribute("buffer", "symbol", "Non-realtime Buffer", "Sets the non-realtime buffer for this object", "Buffer");
    writeAttribute("rt", "int", "Realtime flag", "Sets the realtime state for this object", "Realtime");
    writeAttribute("id", "symbol", "Context ID", "Sets the context name for this object", "ID");
    file << tab1 + "</attributelist> \n \n";
    
    // Seealso
    
    file << tab1 + "<!--SEEALSO-->\n";
    file << tab1 + "<seealsolist> \n";
    file << tab1 + "</seealsolist> \n";
    
    // Keywords
    
    file << tab1 + "<misc name = 'Discussion'> \n";
    file << tab2 + "<entry name = 'Keywords'> \n";
    file << tab3 + "<description> \n";
    file << tab4 + objectKeywords + "\n";
    file << tab3 + "</description> \n";
    file << tab2 + "</entry> \n";
    file << tab1 + "</misc> \n \n";
    file << "</c74object>";
    file.close();
    
    return true;
}

template<typename T>
struct DocumentationGenerator
{
    void operator ()(FrameLib_Context context, FrameLib_Parameters::Serial *parameters,  FrameLib_Proxy *proxy, bool *success)
    {
        T obj(context, parameters, proxy, 1);
        
        if (!writeInfo(&obj, FrameLib_ObjectInfo<T>().name(), FrameLib_ObjectInfo<T>().template option<MaxObjectArgsMode, 0>()))
            *success = false;
    }
};

int main()
{
    // Create a Global, Proxy, Context and Parameters
    
    FrameLib_Global *global = nullptr;
    FrameLib_Global::get(&global, FrameLib_Thread::defaultPriorities());
    FrameLib_Proxy *proxy = new FrameLib_Proxy();
    FrameLib_Context context(global, nullptr);
    FrameLib_Parameters::AutoSerial parameters;
    
    bool success = true;
    
    // Loop over objects using template list
    
    FrameLib_DSPList::execute<DocumentationGenerator>(context, &parameters, proxy, &success);
    
    // Cleanup
    
    delete proxy;
    FrameLib_Global::release(&global);
    return success ? 0 : 1;
}

