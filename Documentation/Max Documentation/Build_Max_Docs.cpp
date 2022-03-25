
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

void find_replace(std::string& str, const char *findStr, const char *replaceStr)
{
    for (size_t start_pos = str.find(findStr); start_pos != std::string::npos; start_pos = str.find(findStr))
        str.replace(start_pos, strlen(findStr), replaceStr);
}

std::string format_info(std::string str)
{
    find_replace(str, ". ", ".<br />");
    find_replace(str, ": ", ":<br /><br />");
    
    return str;
}

std::string argument_name(std::string str)
{
    find_replace(str, " ", "-");
    
    return str;
}

std::string escape_xml(std::string str)
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

bool write_info(FrameLib_Multistream* frameLibObject, std::string inputName, MaxObjectArgsMode argsMode)
{
    std::string fileName(__FILE__);
    std::string dirPath = dirname(const_cast<char *>(fileName.c_str()));
    std::string tmpFolder = dirPath + "/__tmp__/raw_xml/";
    
    enum InfoFlags { kInfoDesciption = 0x01, kInfoInputs = 0x02, kInfoOutputs = 0x04, kInfoParameters = 0x08 };

    std::ofstream file;
    std::string sp = " ";               // code is more readable with sp rather than " "
    std::string object = inputName;     // refactor to not copy variable.

    std::string object_category = "!@#@#$";
    std::string object_keywords = "boilerplate keywords";
    std::string object_info;
    std::string object_description;
    std::string object_digest;
    
    // Store some indentations - groups of 4 spaces
    
    std::string tab_1 = "    ";
    std::string tab_2 = tab_1 + tab_1;
    std::string tab_3 = tab_2 + tab_1;
    std::string tab_4 = tab_2 + tab_2;
    
    // Write to a temporary relative location
    
    file.open(tmpFolder + object + ".maxref.xml");

    const FrameLib_Parameters *params = frameLibObject->getParameters();
    
    auto to_lower = [](std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
        return s;
    };
    
    auto write_digest_description = [&](const std::string& tab, const std::string& digest, const std::string& description)
    {
        file << tab + "<digest> \n";
        file << tab + tab_1 + digest + "\n";
        file << tab + "</digest> \n";
        file << tab + "<description> \n";
        file << tab + tab_1 + description + "\n";
        file << tab + "</description> \n";
    };

    auto write_attribute = [&](const char *name, const char *type, const char *digest, const char *description, const char *label)
    {
        file << tab_2 + "<attribute name='" + name + "' get='1' set='1' type='"+ type + "' size='1'> \n";
        write_digest_description(tab_3, digest, description);
        file << tab_3 + "<attributelist> \n";
        file << tab_4 + "<attribute name='basic' get='1' set='1' type='int' size='1' value='1' /> \n";
        file << tab_4 + "<attribute name='label' get='1' set='1' type='symbol' size='1' value='" + label + "' />\n";
        file << tab_3 + "</attributelist> \n";
        file << tab_2 + "</attribute> \n";
    };
    
    auto write_argument = [&](unsigned long idx)
    {
        long pIdx = -1;
        
        for (unsigned long i = 0; params && i < params->size(); i++)
            if (params->getArgumentIdx(i) == idx)
                pIdx = static_cast<long>(i);
        
        if (pIdx == -1)
        {
            if (idx)
                file << tab_1 + "</objarglist> \n \n";
            else
                file << tab_1 + "<objarglist /> \n \n";
            return false;
        }
                
        if (!idx)
            file << tab_1 + "<objarglist> \n";
        
        std::string type = "number";
        
        if (params->getNumericType(pIdx) == FrameLib_Parameters::NumericType::Integer)
            type = "int";
        else if (params->getType(pIdx) == FrameLib_Parameters::Type::Enum)
            type = "symbol";
        else if (params->getType(pIdx) == FrameLib_Parameters::Type::String)
            type = "symbol";
        
        std::string rawDescription = escape_xml(params->getInfo(pIdx));
        std::string name = params->getName(pIdx);
        std::string digest = rawDescription.substr(0, rawDescription.find_first_of(".:"));
        std::string description = format_info(rawDescription);
            
        file << tab_2 + "<objarg name='" + name + "' optional='1' type='" + type + "'> \n";
        write_digest_description(tab_3, digest, description);
        file << tab_2 + "</objarg> \n";
    
        return true;
    };
    
    auto write_arguments_all_inputs = [&]()
    {
        std::string digest("The input vector to use for any disconnected inputs");
        std::string description("Values typed as arguments will be used as a vector for any inputs that are not connected. Either single values or multi-valued vectors can be entered. The behaviour is similar to that for arguments to standard objects such as +~, *~ or zl.reg.");
        
        file << tab_1 + "<objarglist> \n";
        file << tab_2 + "<objarg name='default-input' optional='1' type='list'> \n";
        write_digest_description(tab_3, digest, description);
        file << tab_2 + "</objarg> \n";
        file << tab_1 + "</objarglist> \n \n";
    };
    
    auto write_arguments_distributed = [&]()
    {
        file << tab_1 + "<objarglist> \n";
        
        for (unsigned long i = 1; i < frameLibObject->getNumIns(); i++)
        {
            std::string inputName = to_lower(frameLibObject->inputInfo(i));
            std::string digest("The value to use for input " + std::to_string(i + 1) +  " if it is disconnected");
            std::string description("Sets a single value for " + inputName);
            
            file << tab_2 + "<objarg name='" + argument_name(inputName) + "' optional='1' type='number'> \n";
            write_digest_description(tab_3, digest, description);
            file << tab_2 + "</objarg> \n";
        }
        
        file << tab_1 + "</objarglist> \n \n";
    };
    
    auto write_message = [&](const char *name, const char *digest, const char *description, const std::vector<MessageArgument>& args)
    {
        file << tab_2 + "<method name='" + name + "'> \n";
        
        if (args.size())
        {
            file << tab_3 + "<arglist> \n";
            for (auto it = args.cbegin(); it != args.cend(); it++)
                file << tab_4 + "<arg name='" + it->mName + "' optional='" + std::to_string(it->mOptional) + "' type='" + it->mType + "' /> \n";
            file << tab_3 + "</arglist> \n";
        }
        else
            file << tab_3 + "<arglist /> \n";
        
        write_digest_description(tab_3, digest, description);
        
        file << tab_2 + "</method> \n";
    };
    
    // Check that the file has opened correctly
    
    if (!file.is_open())
        return false;
    
    // Write some stuff at the top of every xml file.
    file << "<?xml version='1.0' encoding='utf-8' standalone='yes'?> \n" ;
    file << "<?xml-stylesheet href='./_c74_ref.xsl' type='text/xsl'?> \n \n" ;
    file << "<c74object name='" << object << "' " << "module='FrameLib' " << "category=" << "'" << object_category << std::string("'> ") << "\n \n";
    
    // Description
    
    // split the object info into a description and a digest
    object_info = escape_xml(frameLibObject->objectInfo(true));
    std::size_t pos = object_info.find_first_of(":.");
    object_digest = object_info.substr(0, pos) + ".";
    object_description = object_info.substr(pos + 1);
    
    // now write that info into sections
    
    write_digest_description(tab_1, object_digest, object_description);
    
    // Parameters
    
    file << tab_1 + "<!--PARAMETERS-->\n";

    if (!params || !params->size())
    {
        // If object has no parameters create the 'no parameters template'

        file << tab_1 + "<misc name = 'Parameters'> \n";
        file << tab_2 + "<entry> \n";
        file << tab_3 + "<description> \n";
        file << tab_4 + "This object has no parameters. \n";
        file << tab_3 + "</description> \n";
        file << tab_2 + "</entry> \n";
        file << tab_1 + "</misc> \n \n";
    }
    else
    {
        file << tab_1 + "<misc name = 'Parameters'> \n"; // Write parameters tag to start misc section named Parameters
        for (int i = 0; params && i < params->size(); i++)
        {
            FrameLib_Parameters::Type type = params->getType(i);
            //FrameLib_Parameters::NumericType numericType = params->getNumericType(i); // remove possibly. its not being used
            std::string defaultStr = params->getDefaultString(i);
            
            // Name, type and default value
            
            if (defaultStr.size())
                file << tab_2 + "<entry name = '/" + params->getName(i) + " [" + params->getTypeString(i) + "]' > \n";
            else
                file << tab_2 + "<entry name = '/" + params->getName(i) + " [" + params->getTypeString(i) + "]' > \n";

            // Construct the description
            
            file << tab_3 + "<description> \n";
            file << tab_4 + escape_xml(params->getInfo(i)); // The description
            
            if (type == FrameLib_Parameters::Type::Enum)
            {
                file << "<br></br> \n" ; // if enum put a break big break between description and the enum options
                    
                for (long j = 0; j <= params->getMax(i); j++)
                {
                    std::string enum_param_num = std::to_string(j);
                    
                    if (j == params->getMax(i))
                        file << tab_4 + "<bullet>[" + enum_param_num + "]" + " - " + params->getItemString(i, j) + "</bullet>";
                    else if (j != params->getMax(i))
                        file << tab_4 + "<bullet>[" + enum_param_num + "]" + " - " + params->getItemString(i, j) + "</bullet> \n";
                }
            }
            file << "\n" + tab_3 + "</description> \n";
            file << tab_2 + "</entry> \n";
        }
        file << tab_1 + "</misc> \n \n";
    }
    
    // Metadata
    
    file << tab_1 + "<!--METADATA-->\n";
    file << tab_1 + "<metadatalist> \n";
    file << tab_2 + "<metadata name='author'>Alex Harker</metadata> \n";
    file << tab_2 + "<metadata name='tag'>FrameLib</metadata> \n";
    file << tab_2 + "<metadata name='tag'>" + object_category + "</metadata> \n";
    file << tab_1 + "</metadatalist> \n \n";
    
    // Arguments
    
    file << tab_1 + "<!--ARGUMENTS-->\n";
    switch (argsMode)
    {
        case kAsParams:
            for (unsigned long i = 0; write_argument(i); i++);
            break;
            
        case kAllInputs:
            write_arguments_all_inputs();
            break;

        case kDistribute:
            write_arguments_distributed();
            break;
    }

    // Messages
    
    std::vector<MessageArgument> emptyArgs;
    std::vector<MessageArgument> infoArgs { { "items", true, "list"} };
    std::vector<MessageArgument> resetArgs { { "samplerate", true, "number" } };
    std::vector<MessageArgument> processArgs { { "length", false, "int" } };
    
    file << tab_1 + "<!--MESSAGES-->\n";
    file << tab_1 + "<methodlist> \n";
    write_message("info", "Get Object Info", "--detail--", infoArgs);
    if (frameLibObject->handlesAudio())
    {
        write_message("process", "Process in non-realtime", "--detail--", processArgs);
        write_message("reset", "Reset a non-realtime network", "--detail--", resetArgs );
        write_message("signal", "Synchronise with audio or accept signal IO", "--detail--", emptyArgs );
    }
    write_message("frame", "Connect FrameLib objects", "Used internally by FrameLib connection routines. User messages have no effect", emptyArgs);
    write_message("sync", "Synchronise FrameLib audio objects", "Used internally by FrameLib connection routines. User messages have no effect", emptyArgs);
    file << tab_1 + "</methodlist> \n \n";

    // Attributes
    
    file << tab_1 + "<!--ATTRIBUTES-->\n";
    file << tab_1 + "<attributelist> \n";
    if (frameLibObject->handlesAudio())
        write_attribute("buffer", "symbol", "Non-realtime Buffer", "Sets the non-realtime buffer for this object", "Buffer");
    write_attribute("rt", "int", "Realtime flag", "Sets the realtime state for this object", "Realtime");
    write_attribute("id", "symbol", "Context ID", "Sets the context name for this object", "ID");
    file << tab_1 + "</attributelist> \n \n";
    
    // Seealso
    
    file << tab_1 + "<!--SEEALSO-->\n";
    file << tab_1 + "<seealsolist> \n";
    file << tab_1 + "</seealsolist> \n";
    
    // Keywords
    
    file << tab_1 + "<misc name = 'Discussion'> \n";
    file << tab_2 + "<entry name = 'Keywords'> \n";
    file << tab_3 + "<description> \n";
    file << tab_4 + object_keywords + "\n";
    file << tab_3 + "</description> \n";
    file << tab_2 + "</entry> \n";
    file << tab_1 + "</misc> \n \n";
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
        
        if (!write_info(&obj, FrameLib_ObjectInfo<T>().name(), FrameLib_ObjectInfo<T>().template option<MaxObjectArgsMode, 0>()))
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
