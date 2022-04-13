
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

void FrameLib_ErrorReporter::ErrorReport::getErrorText(char *text, size_t N) const
{
    const char *errorPtr = mError;
    const char *itemsPtr = mItems;
    
    size_t j = 0, k = 0;
    
    for (unsigned long i = 0; i < mNumItems; i++, errorPtr += j + 1, itemsPtr += k + 1)
    {
        for (j = 0; errorPtr[j]; j++)
            if (errorPtr[j] == '#')
                break;
        
        if (!errorPtr[j])
            break;
        
        k = strlen(itemsPtr);
        copy(text, errorPtr, j, N);
        copy(text, itemsPtr, k, N);
    }
    
    copy(text, errorPtr, strlen(errorPtr), N);
}

void FrameLib_ErrorReporter::ErrorReport::copy(char*& dest, const char *str, size_t length, size_t& left) const
{
    size_t size = std::min(length, left - 1);
    std::copy(str, str + size, dest);
    dest += size;
    left -= size;
    dest[0] = 0;
}

// Retrieve errors (passes ownership to the host)

std::unique_ptr<FrameLib_ErrorReporter::ErrorList> FrameLib_ErrorReporter::getErrors()
{
    std::unique_ptr<ErrorList> reports(new ErrorList());
    
    FrameLib_LockHolder lockHolder(&mLock);
    std::swap(mReports, reports);
    mNotified = false;
    return reports;
}

