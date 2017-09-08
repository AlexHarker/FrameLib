
#ifndef FRAMELIB_INFO_H
#define FRAMELIB_INFO_H

#include <string>
#include <sstream>
#include <iostream>

// FrameLib_Info is a class to simplify creating info strings (provides storage, index replacement and switching between brief and verbose versions)
// For many cases use of const strings with this class will be sufficient

class FrameLib_Info
{
    
public:
    
    const char *getInfo(const char *verboseStr, const char *briefStr, bool verbose)
    {
        const char *outStr = verbose ? verboseStr : briefStr;
        
        return outStr;
    }

    std::string getInfo(const char *verboseStr, const char *briefStr, unsigned long idx, bool verbose)
    {
        std::string info;
        std::ostringstream idxStr;

        idxStr << (idx + 1);

        info = getInfo(verboseStr, briefStr, verbose);
    
        for (size_t pos = info.find("#", 0); pos != std::string::npos;  pos = info.find("#", pos + 1))
            info.replace(pos, 1, idxStr.str());

        return info;
    }
    
    std::string getInfo(const char *verboseStr, const char *briefStr, const char *replaceStr, bool verbose)
    {
        std::string info = getInfo(verboseStr, briefStr, verbose);
        
        for (size_t pos = info.find("#", 0); pos != std::string::npos;  pos = info.find("#", pos + 1))
            info.replace(pos, 1, replaceStr);
        
        return info;
    }
};

#endif
