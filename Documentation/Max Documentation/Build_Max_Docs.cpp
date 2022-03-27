
#include "Max_Object_List.h"

#include "FrameLib_Global.h"
#include "FrameLib_Context.h"
#include "FrameLib_Multistream.h"

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

void findReplace(std::string& str, const std::string& findStr, const std::string& replaceStr)
{
    for (size_t startPos = str.find(findStr); startPos != std::string::npos; startPos = str.find(findStr))
        str.replace(startPos, findStr.length(), replaceStr);
}

void findReplaceOnce(std::string& str, const std::string& findStr, const std::string& replaceStr)
{
    str.replace(str.find(findStr), findStr.length(), replaceStr);
}

std::string replaceAudioStream(std::string str)
{
    findReplace(str, "[1]", "[N]");
    
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

std::string maxIndexString(const FrameLib_Parameters *params, unsigned long idx)
{
    long maxIndex = 1;

    for (unsigned long i = 0; i < params->size(); i++)
    {
        std::string testName = params->getName(i);
        
        if (testName.find("num") != std::string::npos)
        {
            maxIndex = params->getMax(i);
            break;
        }
    }
    
    return std::to_string(maxIndex);
}

bool detectIndexedParam(const FrameLib_Parameters *params, unsigned long idx)
{
    std::string name = params->getName(idx);
    
    return name.find("_01") != std::string::npos;
}

std::string getParamName(const FrameLib_Parameters *params, unsigned long idx)
{
    std::string name = params->getName(idx);
    
    if (detectIndexedParam(params, idx))
        name += "..." + maxIndexString(params, idx);
        
    return name;
}

std::string formatParameterInfo(std::string str)
{
    findReplace(str, ". ", ".<br />");
    findReplace(str, ": ", ":<br /><br />");
    
    return str;
}

std::string processParamInfo(const FrameLib_Parameters *params, unsigned long idx)
{
    std::string info = params->getInfo(idx);
    
    if (detectIndexedParam(params, idx))
        findReplaceOnce(info, "1", "N [1-" + maxIndexString(params, idx) + "]");
    
    if (params->getType(idx) == FrameLib_Parameters::Type::Enum)
        return formatParameterInfo(escapeXML(info));
    else
        return escapeXML(info);

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
    
    std::string objectDoc = "<o>" + inputName + "</o>";
    std::string objectCategory = "!@#@#$";
    std::string objectKeywords = "boilerplate keywords";
    std::string objectInfo;
    std::string objectDescription;
    std::string objectDigest;
    
    // Store some indentations - groups of 2 spaces
    
    std::string tab1 = "  ";
    std::string tab2 = tab1 + tab1;
    std::string tab3 = tab2 + tab1;
    std::string tab4 = tab2 + tab2;
    
    
    // Reuseable strings
    
    std::string conversionTutorial("More info on conversion between Max messages and FrameLib can be found in <link name='04_fl_conversion' module='framelib' type='tutorial'>Tutorial 4</link>.");
    std::string nonRealtimeTutorial("More info on non-realtime processing with FrameLib can be found in <link name='11_fl_nrt' module='framelib' type='tutorial'>Tutorial 11</link>.");
    
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
        file << tab + "<digest>\n";
        file << tab + tab1 + digest + "\n";
        file << tab + "</digest>\n";
        file << tab + "<description>\n";
        file << tab + tab1 + description + "\n";
        file << tab + "</description>\n";
    };
    
    auto writeAttribute = [&](const std::string& name,
                              const std::string& type,
                              const std::string& digest,
                              const std::string& description,
                              const std::string& label)
    {
        file << tab2 + "<attribute name='" + name + "' get='1' set='1' type='"+ type + "' size='1'>\n";
        writeDigestDescription(tab3, digest, description);
        file << tab3 + "<attributelist>\n";
        file << tab4 + "<attribute name='basic' get='1' set='1' type='int' size='1' value='1' />\n";
        file << tab4 + "<attribute name='label' get='1' set='1' type='symbol' size='1' value='" + label + "' />\n";
        file << tab3 + "</attributelist>\n";
        file << tab2 + "</attribute>\n";
    };
    
    auto writeParamAsArgument = [&](unsigned long idx)
    {
        long paramIdx = -1;
        
        for (unsigned long i = 0; params && i < params->size(); i++)
            if (params->getArgumentIdx(i) == idx)
                paramIdx = static_cast<long>(i);
        
        if (paramIdx == -1)
        {
            if (idx)
                file << tab1 + "</objarglist>\n\n";
            else
                file << tab1 + "<objarglist />\n\n";
            return false;
        }
        
        if (!idx)
            file << tab1 + "<objarglist>\n";
        
        std::string type = "number";
        
        if (params->getNumericType(paramIdx) == FrameLib_Parameters::NumericType::Integer)
            type = "int";
        else if (params->getType(paramIdx) == FrameLib_Parameters::Type::Enum)
            type = "symbol";
        else if (params->getType(paramIdx) == FrameLib_Parameters::Type::String)
            type = "symbol";
        
        std::string name = getParamName(params, paramIdx);
        std::string rawDescription = processParamInfo(params, paramIdx);
        std::string digest = rawDescription.substr(0, rawDescription.find_first_of(".:"));
        std::string description = "This argument sets the " + name + " parameter:<br /><br />" + rawDescription;
        
        if (detectIndexedParam(params, paramIdx))
            description = "Arguments set parameters " + name + ":<br /><br />" + rawDescription;
        
        file << tab2 + "<objarg name='" + name + "' optional='1' type='" + type + "'>\n";
        writeDigestDescription(tab3, digest, description);
        file << tab2 + "</objarg>\n";
        
        bool stop = detectIndexedParam(params, paramIdx);
        
        if (stop)
            file << tab1 + "</objarglist>\n\n";
        
        return !stop;
    };
    
    auto writeArgumentsAllInputs = [&]()
    {
        std::string digest("The input vector to use for any disconnected inputs");
        std::string description("Values typed as arguments will be used as a vector for any inputs that are not connected. Either single values or multi-valued vectors can be entered. The behaviour is similar to that for arguments to standard objects such as +~, *~ or zl.reg.");
        
        file << tab1 + "<objarglist>\n";
        file << tab2 + "<objarg name='default-input' optional='1' type='list'>\n";
        writeDigestDescription(tab3, digest, description);
        file << tab2 + "</objarg>\n";
        file << tab1 + "</objarglist>\n\n";
    };
    
    auto writeArgumentsDistributed = [&]()
    {
        file << tab1 + "<objarglist>\n";
        
        for (unsigned long i = 1; i < frameLibObject->getNumIns(); i++)
        {
            std::string argumentName = toLower(frameLibObject->inputInfo(i));
            std::string digest("The value to use for input " + std::to_string(i + 1) +  " if it is disconnected");
            std::string description("Sets a single value for " + argumentName);

            findReplace(argumentName, " ", "-");

            file << tab2 + "<objarg name='" + argumentName + "' optional='1' type='number'>\n";
            writeDigestDescription(tab3, digest, description);
            file << tab2 + "</objarg>\n";
        }
        
        file << tab1 + "</objarglist>\n\n";
    };
    
    auto writeMessage = [&](const std::string& name,
                            const std::string& digest,
                            const std::string& description,
                            const std::vector<MessageArgument>& args)
    {
        file << tab2 + "<method name='" + name + "'>\n";
        
        if (args.size())
        {
            file << tab3 + "<arglist>\n";
            for (auto it = args.cbegin(); it != args.cend(); it++)
                file << tab4 + "<arg name='" + it->mName + "' optional='" + std::to_string(it->mOptional) + "' type='" + it->mType + "' />\n";
            file << tab3 + "</arglist>\n";
        }
        else
            file << tab3 + "<arglist />\n";
        writeDigestDescription(tab3, digest, description);
        
        file << tab2 + "</method>\n";
    };
    
    // Check that the file has opened correctly
    
    if (!file.is_open())
        return false;
    
    // Write some stuff at the top of every xml file
    
    file << "<?xml version='1.0' encoding='utf-8' standalone='yes'?>\n" ;
    file << "<?xml-stylesheet href='./_c74_ref.xsl' type='text/xsl'?>\n\n" ;
    file << "<c74object name='" << object << "' " << "module='FrameLib' " << "category=" << "'" << objectCategory << std::string("'>") << "\n\n";
    
    // Description
    
    // Split the object info into a description and a digest
    
    objectInfo = escapeXML(frameLibObject->objectInfo(true));
    std::size_t pos = objectInfo.find_first_of(":.");
    objectDigest = objectInfo.substr(0, pos);
    objectDescription = objectInfo.substr(objectInfo[pos + 1] == ' ' ? pos + 2 : pos + 1);
    
    if (object == "fl.tomax~")
    {
        findReplaceOnce(objectDigest, "host messages or control signals", "Max messages");
        findReplaceOnce(objectDescription, "the host environment. Vector and tagged frame types are both supported if supported by the host", "Max");
        objectDescription += "<br /><br/>" + conversionTutorial;
    }
    else if (object == "fl.frommax~")
    {
        findReplaceOnce(objectDigest, "messages or control signals from the host", "Max messages");
        objectDescription += "<br /><br/>" + conversionTutorial;
    }
    
    // Now write that info into sections
    
    writeDigestDescription(tab1, objectDigest, objectDescription);
    file << "\n";
    
    // Parameters
    
    file << tab1 + "<!--PARAMETERS-->\n";
    
    if (!params || !params->size())
    {
        // If object has no parameters create the 'no parameters template'
        
        file << tab1 + "<misc name = 'Parameters'>\n";
        file << tab2 + "<entry>\n";
        file << tab3 + "<description>\n";
        file << tab4 + objectDoc + " has no parameters.\n";
        file << tab3 + "</description>\n";
        file << tab2 + "</entry>\n";
        file << tab1 + "</misc>\n\n";
    }
    else
    {
        // Write parameters tag to start misc section named Parameters
        
        file << tab1 + "<misc name = 'Parameters'>\n";
        for (unsigned long i = 0; params && i < params->size(); i++)
        {
            // Name, type and default value

            std::string name = getParamName(params, i);
            std::string defaultStr = params->getDefaultString(i);
         
            FrameLib_Parameters::Type type = params->getType(i);
                    
            if (defaultStr.size())
                file << tab2 + "<entry name = '/" + name + " [" + params->getTypeString(i) + "]' >\n";
            else
                file << tab2 + "<entry name = '/" + name + " [" + params->getTypeString(i) + "]' >\n";
            
            // Construct the description
            
            file << tab3 + "<description>\n";
            
            // Place enum items first in the description
            
            // FIX - move to processParamInfo??
            
            if (type == FrameLib_Parameters::Type::Enum)
            {
                for (long j = 0; j <= params->getMax(i); j++)
                {
                    std::string enumParamNum = std::to_string(j);
                    
                    if (j == params->getMax(i))
                        file << tab4 + "<bullet>[" + enumParamNum + "]" + " - <m>" + params->getItemString(i, j) + "</m></bullet>";
                    else if (j != params->getMax(i))
                        file << tab4 + "<bullet>[" + enumParamNum + "]" + " - <m>" + params->getItemString(i, j) + "</m></bullet>\n";
                }
                
                file << "<br />\n" ; // if enum put a break big break between the enum options and the description
            }
            
            file << tab4 + processParamInfo(params, i);
            file << "\n" + tab3 + "</description>\n";
            file << tab2 + "</entry>\n";
            
            if (detectIndexedParam(params, i))
                break;
        }
        file << tab1 + "</misc>\n\n";
    }
    
    // Metadata
    
    file << tab1 + "<!--METADATA-->\n";
    file << tab1 + "<metadatalist>\n";
    file << tab2 + "<metadata name='author'>Alex Harker</metadata>\n";
    file << tab2 + "<metadata name='tag'>FrameLib</metadata>\n";
    file << tab2 + "<metadata name='tag'>" + objectCategory + "</metadata>\n";
    file << tab1 + "</metadatalist>\n\n";
    
    // Arguments
    
    file << tab1 + "<!--ARGUMENTS-->\n";
    switch (argsMode)
    {
        case kAsParams:
            for (unsigned long i = 0; writeParamAsArgument(i); i++);
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
    
    std::string infoDescription("Print info about " + objectDoc + " to the max window for reference purposes. If no arguments are provided then all information is posted to the Max window. Else, a set of flags is used to select which sections of the reference to display, and whether or not the information should be provided in a shortened form.<br /> <br />The following flags are available:<br /><br /><bullet><m>description</m> - display the object description.</bullet><bullet><m>inputs</m> - display info on inputs.</bullet><bullet><m>outputs</m> - display info on outputs.</bullet><bullet><m>io</m> - display info on both inputs and outputs.</bullet><bullet><m>parameters</m> - display info on the object parameters.</bullet><bullet><m>quick</m> - display shorten versions of any info displayed.</bullet>");
    std::string processDescription("Process a non-realtime network,advancing time by the number of samples specified by the required <m>length</m> argument. <br /><br />This will only take effect if the object has its <m>rt</m> attribute set to <m>0</m> " + nonRealtimeTutorial);
    std::string resetDescription("Resets a non-realtime network to the start of time ready for processing, optionally setting the sample rate. If the sample rate is omitted it will be set to the global sample rate.<br /><br />This will only take effect if " + objectDoc + " has its <m>rt</m> attribute set to <m>0</m>. " + nonRealtimeTutorial);
    std::string signalDescription("Used to synchonise FrameLib objects with Max's audio processing.");
    std::string connectionDescription("Used internally by FrameLib's connection routines. User messages have no effect.");

    if (frameLibObject->getNumAudioChans())
    {
        std::string ioString;
        signalDescription = "";
        
        if (frameLibObject->getNumAudioIns())
        {
            signalDescription += "Audio Inputs are as follows:<br /><br />";
            ioString = "inputs";
        }
        else
        {
            signalDescription += "Audio Outputs are as follows:<br /><br />";
            ioString = "outputs";
        }
            
        for (unsigned long i = 0; i < frameLibObject->getNumAudioChans(); i++)
            signalDescription += "<bullet>" + replaceAudioStream(frameLibObject->audioInfo(i, true)) + ".</bullet>";
        
        signalDescription += "<br />There are N audio " + ioString + ", as set by the number of explicitly set streams.";
    }
    
    file << tab1 + "<!--MESSAGES-->\n";
    file << tab1 + "<methodlist>\n";
    
    // Input Special Case
    
    if (object == "fl.frommax~")
    {
        std::vector<MessageArgument> intArgs { { "value", false, "int" } };
        std::vector<MessageArgument> floatArgs { { "value", false, "float" } };
        std::vector<MessageArgument> listArgs { { "values", false, "list" } };
        std::vector<MessageArgument> anythingArgs { { "parameter-input", false, "list" } };

        std::string intDescription("Performs the same function as <m>list</m> but with a single value.");
        std::string floatDescription("Performs the same function as <m>list</m> but with a single value.");
        std::string listDescription("If <o>fl.frommax~</o> is set to values mode then incoming lists are stored as a vector. Whenever <o>fl.frommax~</o> is scheduled the vector is output, until it is replaced by a new set of values.<br /><br />If <o>fl.frommax~</o> is in params mode then numerical inputs are ignored.");
        std::string anythingDescription("If <o>fl.frommax~</o> is set to params mode then messages starting with a symbol will be accumulated into an internal tagged frame. When <o>fl.frommax~</o> is next scheduled to output the tagged frame is output and the internal frame is reset to empty.<br /><br />If <o>fl.frommax~</o> is in values mode then messages starting with a symbol are ignored.");
        
        writeMessage("int", "Value to convert to a vector frame", intDescription, intArgs);
        writeMessage("float", "Value to convert to a vector frame", floatDescription, floatArgs);
        writeMessage("list", "Values to convert to a vector frame", listDescription, listArgs);
        writeMessage("anything", "Tagged values to accumulate into a tagged output frame", anythingDescription, anythingArgs);
    }
    
    writeMessage("info", "Get Object Info", infoDescription, infoArgs);
    if (frameLibObject->handlesAudio())
    {
        writeMessage("process", "Process a network in non-realtime", processDescription, processArgs);
        writeMessage("reset", "Reset a non-realtime network", resetDescription, resetArgs );
        writeMessage("signal", "Synchronise with audio or accept signal IO", signalDescription, emptyArgs );
    }
    writeMessage("frame", "Connect FrameLib objects", connectionDescription, emptyArgs);
    writeMessage("sync", "Synchronise FrameLib audio objects", connectionDescription, emptyArgs);
    file << tab1 + "</methodlist>\n\n";
    
    // Attributes
    
    std::string bufferDescription("Sets the non-realtime <o>buffer~</o> for " + objectDoc +". This is the <o>buffer~</o> used for IO in a non-realtime setting.<br /><br />" + nonRealtimeTutorial);
    std::string rtDescription("Sets the realtime state for " + objectDoc + ". When set to <m>0</m> " + objectDoc + " can form part of a non-realtime network for processing in message threads, using <o>buffer~</o> objects for audio IO.<br /><br />" + nonRealtimeTutorial);
    std::string idDescription("Sets the context name for " + objectDoc + ".<br /><br />More info on FrameLib contexts can be found in <link name='10_fl_contexts' module='framelib' type='tutorial'>Tutorial 10</link>.");
    
    file << tab1 + "<!--ATTRIBUTES-->\n";
    file << tab1 + "<attributelist>\n";
    if (frameLibObject->handlesAudio())
        writeAttribute("buffer", "symbol", "Non-realtime Buffer", bufferDescription, "Buffer");
    writeAttribute("rt", "int", "Realtime flag", rtDescription, "Realtime");
    writeAttribute("id", "symbol", "Context ID", idDescription, "ID");
    file << tab1 + "</attributelist>\n\n";
    
    // Seealso
    
    file << tab1 + "<!--SEEALSO-->\n";
    file << tab1 + "<seealsolist>\n";
    file << tab1 + "</seealsolist>\n";
    
    // Output Special Case
    
    if (object == "fl.tomax~")
    {
        file << tab1 + "<misc name = 'Output'>\n";
        file << tab2 + "<entry name = 'anything'>\n";
        file << tab3 + "<description>\n";
        file << tab4 + "If the frame input to <o>fl.tomax~</o> is a tagged frame then the output is a set of messages each starting with the relevant tag and followed by the related value(s).\n";
        file << tab3 + "</description>\n";
        file << tab2 + "</entry>\n";
        file << tab2 + "<entry name = 'list'>\n";
        file << tab3 + "<description>\n";
        file << tab4 + "If the frame input to <o>fl.tomax~</o> is a vector then the output is a list.\n";
        file << tab3 + "</description>\n";
        file << tab2 + "</entry>\n";
        file << tab1 + "</misc>\n\n";
    }
    
    // Keywords
    
    file << tab1 + "<misc name = 'Discussion'>\n";
    file << tab2 + "<entry name = 'Keywords'>\n";
    file << tab3 + "<description>\n";
    file << tab4 + objectKeywords + "\n";
    file << tab3 + "</description>\n";
    file << tab2 + "</entry>\n";
    file << tab1 + "</misc>\n\n";
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
