
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
        mInfo = verbose ? verboseStr : briefStr;
        
        return mInfo.c_str();
    }

    const char *getInfo(const char *verboseStr, const char *briefStr, unsigned long idx, bool verbose)
    {
        std::ostringstream idxStr;

        idxStr << (idx + 1);

        getInfo(verboseStr, briefStr, verbose);
    
        for (size_t pos = mInfo.find("#", 0); pos != std::string::npos;  pos = mInfo.find("#", pos + 1))
            mInfo.replace(pos, 1, idxStr.str());

        return mInfo.c_str();
    }
    
    const char *getInfo(const char *verboseStr, const char *briefStr, const char *replaceStr, bool verbose)
    {
        getInfo(verboseStr, briefStr, verbose);
        
        for (size_t pos = mInfo.find("#", 0); pos != std::string::npos;  pos = mInfo.find("#", pos + 1))
            mInfo.replace(pos, 1, replaceStr);
        
        return mInfo.c_str();
    }
    
private:
      
    std::string mInfo;
};

#endif
