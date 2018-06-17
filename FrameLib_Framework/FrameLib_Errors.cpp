
#include "FrameLib_Errors.h"

// Report Class

void FrameLib_ErrorReporter::ErrorReport::getErrorText(std::string& text) const
{
    text = mError;
    const char *ptr = mItems;
    
    for (unsigned long i = 0; i < mNumItems; i++)
    {
        size_t pos = text.find("#");
        
        if (pos == std::string::npos)
            break;
        
        text.replace(pos, 1, ptr);
        ptr += strlen(ptr) + 1;
    }
}

// Retrieve errors (passes ownership to the host)

std::unique_ptr<FrameLib_ErrorReporter::ErrorList> FrameLib_ErrorReporter::getErrors()
{
    std::unique_ptr<ErrorList> reports(new ErrorList());
    
    SpinLockHolder lockHolder(&mLock);
    std::swap(mReports, reports);
    mNotified = false;
    return reports;
}

