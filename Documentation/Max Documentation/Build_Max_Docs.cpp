
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

void find_replace(std::string& str, const char *findStr, const char *replaceStr)
{
    for (size_t start_pos = str.find(findStr); start_pos != std::string::npos; start_pos = str.find(findStr))
        str.replace(start_pos, strlen(findStr), replaceStr);
}

std::string format_text(std::string str)
{
    find_replace(str, ". ", ".<br />");
    find_replace(str, ": ", ":<br /><br />");
    
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

bool write_info(FrameLib_Multistream* frameLibObject, std::string inputName)
{
    std::string fileName(__FILE__);
    std::string dirPath = dirname(const_cast<char *>(fileName.c_str()));
    std::string tmpFolder = dirPath + "/__tmp__/raw_xml/";
    
    enum InfoFlags { kInfoDesciption = 0x01, kInfoInputs = 0x02, kInfoOutputs = 0x04, kInfoParameters = 0x08 };
    bool verbose = true;
    std::ofstream myfile;
    std::string sp = " "; // code is more readable with sp rather than " "
    std::string object = inputName; // refactor to not copy variable.
    myfile.open(tmpFolder + object + ".maxref.xml"); // change to some temporary relative location
    std::string object_category = "!@#@#$";
    std::string object_keywords = "boilerplate keywords";
    std::string object_info;
    std::string object_description;
    std::string object_digest;
    // store some indentations - groups of 4 spaces
    std::string tab_1 = "    ";
    std::string tab_2 = "        ";
    std::string tab_3 = "            ";
    std::string tab_4 = "                ";
    
    const FrameLib_Parameters *params = frameLibObject->getParameters();
    
    auto write_attribute = [&](const char *name, const char *type, const char *digest, const char *description, const char *label)
    {
        myfile << tab_2 + "<attribute name='" + name + "' get='1' set='1' type='"+ type + "' size='1'> \n";
        myfile << tab_3 + "<digest> \n";
        myfile << tab_4 + digest + " \n";
        myfile << tab_3 + "</digest> \n";
        myfile << tab_3 + "<description> \n";
        myfile << tab_4 + description + " \n";
        myfile << tab_3 + "</description> \n";
        myfile << tab_3 + "<attributelist> \n";
        myfile << tab_4 + "<attribute name='basic' get='1' set='1' type='int' size='1' value='1' /> \n";
        myfile << tab_4 + "<attribute name='label' get='1' set='1' type='symbol' size='1' value='" + label + "' />\n";
        myfile << tab_3 + "</attributelist> \n";
        myfile << tab_2 + "</attribute> \n";
    };
    
    auto write_argument = [&](int idx)
    {
        int pIdx = -1;
        
        for (int i = 0; params && i < params->size(); i++)
            if (params->getArgumentIdx(i) == idx)
                pIdx = i;
        
        if (pIdx == -1)
        {
            if (idx)
                myfile << tab_1 + "</objarglist> \n \n";
            else
                myfile << tab_1 + "<objarglist /> \n \n";
            return false;
        }
                
        if (!idx)
            myfile << tab_1 + "<objarglist> \n";
        
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
        std::string description = format_text(rawDescription);
            
        myfile << tab_2 + "<objarg name='" + name + "' optional='1' type='" + type + "'> \n";
        myfile << tab_3 + "<digest> \n";
        myfile << tab_4 + digest + "\n";
        myfile << tab_3 + "</digest> \n";
        myfile << tab_3 + "<description> \n";
        myfile << tab_4 + description + "\n";
        myfile << tab_3 + "</description> \n";
        myfile << tab_2 + "</objarg> \n";
    
        return true;
    };
    
    auto write_message = [&](const char *name, const char *digest, const char *description)
    {
        myfile << tab_2 + "<method name='" + name + "'> \n";
        //myfile << tab_3 + "<arglist> \n";
        //<arg name="input" optional="0" type="int" />
        //myfile << tab_3 + "</arglist> \n";
        myfile << tab_3 + "<digest> \n";
        myfile << tab_4 + digest + " \n";
        myfile << tab_3 + "</digest> \n";
        myfile << tab_3 + "<description> \n";
        myfile << tab_4 + description + " \n";
        myfile << tab_3 + "</description> \n";
        myfile << tab_2 + "</method> \n";
    };
    
    // Check that the file has opened correctly
    
    if (!myfile.is_open())
        return false;
    
    // Write some stuff at the top of every xml file.
    myfile << "<?xml version='1.0' encoding='utf-8' standalone='yes'?> \n" ;
    myfile << "<?xml-stylesheet href='./_c74_ref.xsl' type='text/xsl'?> \n \n" ;
    myfile << "<c74object name='" << object << "' " << "module='FrameLib' " << "category=" << "'" << object_category << std::string("'> ") << "\n \n";
    
    // Description
    
    // split the object info into a description and a digest
    object_info = escape_xml(frameLibObject->objectInfo(verbose));
    std::size_t pos = object_info.find_first_of(":.");
    object_digest = object_info.substr(0, pos);
    object_description = object_info.substr(pos + 1);
    
    // now write that info into sections
    myfile << tab_1 + "<digest> \n";
    myfile << tab_2 + object_digest + "." + "\n";
    myfile << tab_1 + "</digest> \n \n";
    
    myfile << tab_1 + "<description> \n";
    myfile << tab_2 + object_description + "\n";
    myfile << tab_1 + "</description> \n \n";
    
    // Parameters
    
    // If object has no parameters create the 'no parameters template'
    
    if (!params || !params->size()) {
        myfile << tab_1 + "<misc name = 'Parameters'> \n";
        myfile << tab_2 + "<entry> \n";
        myfile << tab_3 + "<description> \n";
        myfile << tab_4 + "This object has no parameters. \n";
        myfile << tab_3 + "</description> \n";
        myfile << tab_2 + "</entry> \n";
        myfile << tab_1 + "</misc> \n \n";
    }
    
    // Loop over parameters
    
    if (params->size() != 0) {
        myfile << tab_1 + "<misc name = 'Parameters'> \n \n"; // Write parameters tag to start misc section named Parameters
        for (int i = 0; params && i < params->size(); i++)
        {
            std::string param_num = std::to_string(i+1);
            FrameLib_Parameters::Type type = params->getType(i);
//            FrameLib_Parameters::NumericType numericType = params->getNumericType(i); // remove possibly. its not being used
            std::string defaultStr = params->getDefaultString(i);
            
            // Name, type and default value
            if (defaultStr.size()) {
                
                myfile << tab_2 + "<entry name = '" + param_num + ". /" + params->getName(i) + " [" + params->getTypeString(i) + "]' > \n";
            }
            else {
                
                myfile << tab_2 + "<entry name = '" + param_num + ". /" + params->getName(i) + " [" + params->getTypeString(i) + "]' > \n";
            }
            // Construct the description
            myfile << tab_3 + "<description> \n";
            myfile << tab_4 + escape_xml(params->getInfo(i)); // The description
            
            // Verbose - arguments, range (for numeric types), enum items (for enums), array sizes (for arrays), description
            if (verbose)
            {
                if (type == FrameLib_Parameters::Type::Enum){
                    
                    myfile << "<br></br> \n" ; // if enum put a break big break between description and the enum options
                    
                    for (long j = 0; j <= params->getMax(i); j++) {
                        std::string enum_param_num = std::to_string(j);
                        if (j == params->getMax(i))
                            
                            myfile << tab_4 + "<bullet>[" + enum_param_num + "]" + " - " + params->getItemString(i, j) + "</bullet>";
                        
                        else if (j != params->getMax(i))
                            
                            myfile << tab_4 + "<bullet>[" + enum_param_num + "]" + " - " + params->getItemString(i, j) + "</bullet> \n";
                    }
                }
                myfile << "\n" + tab_3 + "</description> \n";
                myfile << tab_2 + "</entry> \n \n";
            }
        }
        myfile << tab_1 + "</misc> \n \n";
    }
    // All the tail end stuff is going to here like see also and key words. //
    
    // Metadata //
    myfile << tab_1 + "<!--METADATA-->\n";
    myfile << tab_1 + "<metadatalist> \n";
    myfile << tab_2 + "<metadata name='author'>Alex Harker</metadata> \n";
    myfile << tab_2 + "<metadata name='tag'>FrameLib</metadata> \n";
    myfile << tab_2 + "<metadata name='tag'>" + object_category + "</metadata> \n";
    myfile << tab_1 + "</metadatalist> \n \n";
    
    // Arguments //
    myfile << tab_1 + "<!--ARGUMENTS-->\n";
    for (int i = 0; write_argument(i); i++);
    
    // Messages //
    myfile << tab_1 + "<!--MESSAGES-->\n";
    myfile << tab_1 + "<methodlist> \n";
    write_message("info", "Get Object Info", "--detail--");
    if (frameLibObject->handlesAudio())
    {
        write_message("process", "Process in non-realtime", "--detail--");
        write_message("reset", "Reset a non-realtime network", "--detail--");
        write_message("signal", "Synchronise with audio or accept signal IO", "--detail--");
    }
    write_message("frame", "Connect FrameLib objects", "Used internally by FrameLib connection routines. User messages have no effect");
    write_message("sync", "Synchronise FrameLib audio objects", "Used internally by FrameLib connection routines. User messages have no effect");
    myfile << tab_1 + "</methodlist> \n \n";

    // Attributes //
    myfile << tab_1 + "<!--ATTRIBUTES-->\n";
    myfile << tab_1 + "<attributelist> \n";
    if (frameLibObject->handlesAudio())
        write_attribute("buffer", "symbol", "Non-realtime Buffer", "Sets the non-realtime buffer for this object", "Buffer");
    write_attribute("rt", "int", "Realtime flag", "Sets the realtime state for this object", "Realtime");
    write_attribute("id", "symbol", "Context ID", "Sets the context name for this object", "ID");
    myfile << tab_1 + "</attributelist> \n \n";
    
    // Seealso //
    myfile << tab_1 + "<!--SEEALSO-->\n";
    myfile << tab_1 + "<seealsolist> \n";
    myfile << tab_1 + "</seealsolist> \n";
    
    // Keywords //
    myfile << tab_1 + "<misc name = 'Discussion'> \n";
    myfile << tab_2 + "<entry name = 'Keywords'> \n";
    myfile << tab_3 + "<description> \n";
    myfile << tab_4 + object_keywords + "\n";
    myfile << tab_3 + "</description> \n";
    myfile << tab_2 + "</entry> \n";
    myfile << tab_1 + "</misc> \n \n";
    myfile << "</c74object>";
    myfile.close();
    
    return true;
}

template<typename T>
struct DocumentationGenerator
{
    void operator ()(FrameLib_Context context, FrameLib_Parameters::Serial *parameters,  FrameLib_Proxy *proxy, bool *success)
    {
        T obj(context, parameters, proxy, 1);
        
        if (!write_info(&obj, FrameLib_ObjectName<T>().name()))
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
