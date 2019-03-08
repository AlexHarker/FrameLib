// fl includes
#include "FrameLib_Objects.h"
#include "FrameLib_Global.h"
#include "FrameLib_Context.h"
#include "FrameLib_Multistream.h"
#include <Python/Python.h>
//#include "Doc_Storage.h"
// stl
#include <string>
#include <vector>
#include <iostream>
#include <fstream>




void write_info()
    {
        std::vector<std::string> objectNames = {};
        objectNames[1] = "fl.and~";
        objectNames[2] = "fl.atan2~";
        objectNames[3] = "fl.copysign~";
        objectNames[4] = "fl.diff~";
        objectNames[5] = "fl.divide~";
        objectNames[6] = "fl.equals~";
        objectNames[7] = "fl.greaterthan~";
        objectNames[8] = "fl.greaterthaneq~";
        objectNames[9] = "fl.hypot~";
        objectNames[10] = "fl.lessthan~";
        objectNames[11] = "fl.lessthaneq~";
        objectNames[12] = "fl.max~";
        objectNames[13] = "fl.min~";
        objectNames[14] = "fl.minus~";
        objectNames[15] = "fl.modulo~";
        objectNames[16] = "fl.notequals~";
        objectNames[17] = "fl.or~";
        objectNames[18] = "fl.plus~";
        objectNames[19] = "fl.pow~";
        objectNames[20] = "fl.times~";
        objectNames[21] = "fl.info~";
        objectNames[22] = "fl.read~";
        objectNames[23] = "fl.complexdivide~";
        objectNames[24] = "fl.complexmultiply~";
        objectNames[25] = "fl.complexplus~";
        objectNames[26] = "fl.complexpow~";
        objectNames[27] = "fl.cartopol~";
        objectNames[28] = "fl.complexconj~";
        objectNames[29] = "fl.complexcos~";
        objectNames[30] = "fl.complexcosh~";
        objectNames[31] = "fl.complexexp~";
        objectNames[32] = "fl.complexlog~";
        objectNames[33] = "fl.complexlog10~";
        objectNames[34] = "fl.complexsin~";
        objectNames[35] = "fl.complexsinh~";
        objectNames[36] = "fl.complexsqrt~";
        objectNames[37] = "fl.complextan~";
        objectNames[38] = "fl.complextanh~";
        objectNames[39] = "fl.poltocar~";
        objectNames[40] = "fl.complexexpr~";
        objectNames[41] = "fl.expr~";
        objectNames[42] = "fl.0dfsvf~";
        objectNames[43] = "fl.onepole~";
        objectNames[44] = "fl.onepolezero~";
        objectNames[45] = "fl.resonant~";
        objectNames[46] = "fl.sallenkey~";
        objectNames[47] = "fl.gaussian~";
        objectNames[48] = "fl.ramp~";
        objectNames[49] = "fl.random~";
        objectNames[50] = "fl.uniform~";
        objectNames[51] = "fl.frommax~";
        objectNames[52] = "fl.tomax~";
        objectNames[53] = "fl.sink~";
        objectNames[54] = "fl.source~";
        objectNames[55] = "fl.trace~";
        objectNames[56] = "fl.constant~";
        objectNames[57] = "fl.convert~";
        objectNames[58] = "fl.lookup~";
        objectNames[59] = "fl.map~";
        objectNames[60] = "fl.samplerate~";
        objectNames[61] = "fl.combinetags~";
        objectNames[62] = "fl.filtertags~";
        objectNames[63] = "fl.getparam~";
        objectNames[64] = "fl.tag~";
        objectNames[65] = "fl.untag~";
        objectNames[66] = "fl.route~";
        objectNames[67] = "fl.select~";
        objectNames[68] = "fl.audiotrigger~";
        objectNames[69] = "fl.future~";
        objectNames[70] = "fl.interval~";
        objectNames[71] = "fl.once~";
        objectNames[72] = "fl.perblock~";
        objectNames[73] = "fl.coordinatesystem~";
        objectNames[74] = "fl.spatial~";
        objectNames[75] = "fl.convolve~";
        objectNames[76] = "fl.correlate~";
        objectNames[77] = "fl.fft~";
        objectNames[78] = "fl.ifft~";
        objectNames[79] = "fl.multitaper~";
        objectNames[80] = "fl.window~";
        objectNames[81] = "fl.recall~";
        objectNames[82] = "fl.register~";
        objectNames[83] = "fl.store~";
        objectNames[84] = "fl.pack~";
        objectNames[85] = "fl.streamid~";
        objectNames[86] = "fl.unpack~";
        objectNames[87] = "fl.clip~";
        objectNames[88] = "fl.fold~";
        objectNames[89] = "fl.wrap~";
        objectNames[90] = "fl.clip~";
        objectNames[91] = "fl.ewma~";
        objectNames[92] = "fl.ewmsd~";
        objectNames[93] = "fl.framedelta~";
        objectNames[94] = "fl.lag~";
        objectNames[95] = "fl.timemean~";
        objectNames[96] = "fl.timemedian~";
        objectNames[97] = "fl.timestddev~";
        objectNames[98] = "fl.now~";
        objectNames[99] = "fl.ticks~";
        objectNames[100] = "fl.timedelta~";
        objectNames[101] = "fl.timer~";
        objectNames[102] = "fl.abs~";
        objectNames[103] = "fl.acos~";
        objectNames[104] = "fl.acosh~";
        objectNames[105] = "fl.asin~";
        objectNames[106] = "fl.asinh~";
        objectNames[107] = "fl.atan~";
        objectNames[108] = "fl.atanh~";
        objectNames[109] = "fl.cbrt~";
        objectNames[110] = "fl.ceil~";
        objectNames[111] = "fl.cos~";
        objectNames[112] = "fl.cosh~";
        objectNames[113] = "fl.erf~";
        objectNames[114] = "fl.erfc~";
        objectNames[115] = "fl.exp~";
        objectNames[116] = "fl.exp2~";
        objectNames[117] = "fl.floor~";
        objectNames[118] = "fl.log~";
        objectNames[119] = "fl.log10~";
        objectNames[120] = "fl.not~";
        objectNames[121] = "fl.round~";
        objectNames[122] = "fl.sin~";
        objectNames[123] = "fl.sinh~";
        objectNames[124] = "fl.sqrt~";
        objectNames[125] = "fl.tan~";
        objectNames[126] = "fl.tanh~";
        objectNames[127] = "fl.trunc~";
        objectNames[128] = "fl.accumpoint~";
        objectNames[129] = "fl.argmax~";
        objectNames[130] = "fl.argmin~";
        objectNames[131] = "fl.centroid~";
        objectNames[132] = "fl.chop~";
        objectNames[133] = "fl.crest~";
        objectNames[134] = "fl.flatness~";
        objectNames[135] = "fl.geometricmean~";
        objectNames[136] = "fl.join~";
        objectNames[137] = "fl.kurtosis~";
        objectNames[138] = "fl.length~";
        objectNames[139] = "fl.mean~";
        objectNames[140] = "fl.medianfilter~";
        objectNames[141] = "fl.nanfilter~";
        objectNames[142] = "fl.nonzero~";
        objectNames[143] = "fl.pad~";
        objectNames[144] = "fl.peaks~";
        objectNames[145] = "fl.percentile~";
        objectNames[146] = "fl.product~";
        objectNames[147] = "fl.reverse~";
        objectNames[148] = "fl.rms~";
        objectNames[149] = "fl.shift~";
        objectNames[150] = "fl.skewness~";
        objectNames[151] = "fl.sort~";
        objectNames[152] = "fl.split~";
        objectNames[153] = "fl.spread~";
        objectNames[154] = "fl.standarddeviation~";
        objectNames[155] = "fl.subframe~";
        objectNames[156] = "fl.sum~";
        objectNames[157] = "fl.vmax~";
        objectNames[158] = "fl.vmin~";
        objectNames[159] = "fl.dispatch~";
        objectNames[160] = "fl.chain~";
        
        // Create a Global, Context
        FrameLib_Global *Global = nullptr;
        FrameLib_Global::get(&Global);
        FrameLib_Multistream *Object;
        FrameLib_Proxy *Proxy = new FrameLib_Proxy();
        FrameLib_Context context(Global, nullptr);
        FrameLib_Parameters::AutoSerial parameters;
        
//        Object = new FrameLib_Expand<FrameLib_Interval>(context, nullptr, Proxy, 1);
        Object = new FrameLib_Expand<FrameLib_Source>(context, nullptr, Proxy, 1);
        
        enum InfoFlags { kInfoDesciption = 0x01, kInfoInputs = 0x02, kInfoOutputs = 0x04, kInfoParameters = 0x08 };
        bool verbose = true;
        std::ofstream myfile;
        std::string sp = " "; // code is more readable with sp rather than " "
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








