// fl includes
#include "FrameLib_Objects.h"
#include "FrameLib_Global.h"
#include "FrameLib_Context.h"
#include "FrameLib_Multistream.h"
// stl
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

void write_info()
    {
        // Create a Global, Context
        FrameLib_Global *Global = nullptr;
        FrameLib_Global::get(&Global);
        FrameLib_Multistream *Object;
        FrameLib_Proxy *Proxy = new FrameLib_Proxy();
        FrameLib_Context context(Global, nullptr);
        FrameLib_Parameters::AutoSerial parameters;
        
        Object = new FrameLib_Expand<FrameLib_Interval>(context, nullptr, Proxy, 1);
        
        enum InfoFlags { kInfoDesciption = 0x01, kInfoInputs = 0x02, kInfoOutputs = 0x04, kInfoParameters = 0x08 };
        bool verbose = true;
        std::ofstream myfile;
        std::string sp = " "; // code is more readable with sp rather than " "
        std::string object = object_classname(mUserObject)->s_name;
        std::string object = "FRAMELIBOBJECT";
        myfile.open ("/Users/jamesbradbury/" + object + ".maxref.xml");
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

        // Write some stuff at the top of every xml file.
        myfile << "<?xml version='1.0' encoding='utf-8' standalone='yes'?> \n" ;
        myfile << "<?xml-stylesheet href='./_c74_ref.xsl' type='text/xsl'?> \n \n" ;
        myfile << "<c74object name='" << object << "' " << "module='FrameLib' " << "category=" << "'" << object_category << std::string("'> ") << "\n \n";

        // Description

        // split the object info into a description and a digest
        object_info = Object->objectInfo(verbose);
        std::size_t pos = object_info.find(":");
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
        const FrameLib_Parameters *params = Object->getParameters();
        
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
            for (long i = 0; params && i < params->size(); i++)
            {
                std::string param_num = std::to_string(i);
                FrameLib_Parameters::Type type = params->getType(i);
                FrameLib_Parameters::NumericType numericType = params->getNumericType(i); // remove possibly. its not being used
                std::string defaultStr = params->getDefaultString(i);

                // Name, type and default value

                if (defaultStr.size()) {

                    myfile << tab_2 + "<" + "entry name = " + "'" + param_num + ". " + "/" + params->getName(i) + sp + "[" + params->getTypeString(i) + "]' " + "\n";
                }
                else {

                    myfile << tab_2 + "<" + "entry name = " + "'" + param_num + ". " + "/" + params->getName(i) + sp + "[" + params->getTypeString(i) + "]' " + "\n";
                }
                // Construct the description
                myfile << tab_3 + "<description> \n";
                myfile << tab_4 + params->getInfo(i);

                // Verbose - arguments, range (for numeric types), enum items (for enums), array sizes (for arrays), description
                if (verbose)
                {
                    if (type == FrameLib_Parameters::kEnum){

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
        myfile << tab_1 + "<metadatalist> \n";
        myfile << tab_2 + "<metadata name='author'>Alex Harker</metadata> \n";
        myfile << tab_2 + "<metadata name='tag'>FrameLib</metadata> \n";
        myfile << tab_2 + "<metadata name='tag'>" + object_category + "</metadata> \n";
        myfile << tab_1 + "</metadatalist> \n \n";

        // Seealso //
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
    }

int main() 
{
    write_info();
    return 0;
}








