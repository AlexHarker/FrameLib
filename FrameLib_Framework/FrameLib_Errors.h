
#ifndef FRAMELIB_ERRORS_H
#define FRAMELIB_ERRORS_H

#include "FrameLib_Threading.h"
#include "FrameLib_Types.h"

#include <algorithm>
#include <memory>
#include <string>
#include <string.h>

// Possible sources of error

enum ErrorSource { kErrorObject, kErrorParameter, kErrorMemory, kErrorDSP  };

//  ErrorList Class

class FrameLib_ErrorReporter;

class ErrorList
{
    friend FrameLib_ErrorReporter;

    const static int sCharArraySize = 8192;
    const static int sReportArraySize = 1024;
    
public:
    
    // A single error report

    class ErrorReport
    {
        
    public:
        
        class ConstIterator
        {
            friend ErrorList;
            
            ConstIterator(const ErrorReport *ptr) : mPtr(ptr) {};
            
        public:
            
            const ErrorReport operator *() { return *mPtr; }
            const ErrorReport *operator ->() { return mPtr; }
            
            friend bool operator == (const ConstIterator &a, const ConstIterator &b) { return a.mPtr == b.mPtr; }
            friend bool operator != (const ConstIterator &a, const ConstIterator &b) { return a.mPtr != b.mPtr; }
            
            ConstIterator& operator ++()
            {
                mPtr++;
                return *this;
            }
            
            ConstIterator operator ++(int)
            {
                ConstIterator result = *this;
                operator++();
                return result;
            }

        private:
            
            // Pointer
            
            const ErrorReport *mPtr;
        };
        
        ErrorReport() : mSource(kErrorObject), mReporter(nullptr), mError(nullptr), mItems(nullptr), mNumItems(0) {}
        
        ErrorReport(ErrorSource source, FrameLib_Proxy *reporter, const char *error, const char *items, unsigned long numItems)
        : mSource(source), mReporter(reporter), mError(error), mItems(items), mNumItems(numItems) {}
        
        void getErrorText(std::string& text) const;
        ErrorSource getSource() const                   { return mSource; }
        FrameLib_Proxy* getReporter() const             { return mReporter; }
    
    private:
        
        // Data
        
        ErrorSource mSource;
        FrameLib_Proxy* mReporter;
        const char* mError;
        const char* mItems;
        unsigned long mNumItems;
    };
    
    ErrorList() : mReportsSize(0), mItemsSize(0), mFull(false) {}
    
    const ErrorReport operator[](size_t idx) const { return mReports[idx]; }
    size_t size() const { return mReportsSize; }

    ErrorReport::ConstIterator begin() const { return mReports; }
    ErrorReport::ConstIterator end() const { return mReports + mReportsSize; }
    
    bool isFull() const { return mFull; }
    
private:
    
    // Deleted
    
    ErrorList(const ErrorList&) = delete;
    ErrorList& operator=(const ErrorList&) = delete;
    
    bool addItem(const char *str)
    {
        char *ptr = mItems + mItemsSize;
        size_t size = + strlen(str) + 1;
        
        if (mItemsSize + size < sCharArraySize)
        {
            strcpy(ptr, str);
            mItemsSize += size;
            return true;
        }
        
        return false;
    }
    
    bool addItem(long number)
    {
        char charArray[32];
        sprintf(charArray, "%ld", number);
        return addItem(charArray);
    }
    
    bool addItem(double number)
    {
        char charArray[32];
        sprintf(charArray, "%lf", number);
        return addItem(charArray);
    }
    
    bool addItems()
    {
        return true;
    }
    
    template<typename T>
    bool addItems(T first)
    {
        return addItem(first);
    }
    
    template<typename T, typename... Args>
    bool addItems(T first, Args... args)
    {
        if (addItem(first))
            return addItems(args...);
        return false;
    }
    
    template<typename... Args>
    void add(ErrorSource source, FrameLib_Proxy *reporter, const char *error, Args... args)
    {
        char *ptr = mItems + mItemsSize;

        if (!mFull && (mReportsSize < sReportArraySize) && addItems(args...))
        {
            mReports[mReportsSize] = ErrorReport(source, reporter, error, ptr, sizeof...(args));
            mReportsSize++;
        }
        else
            mFull = true;
    }
    
    // Data
    
    ErrorReport mReports[sCharArraySize];
    char mItems[sCharArraySize];
    size_t mReportsSize;
    size_t mItemsSize;
    bool mFull;
};

// Error reporting class

class FrameLib_ErrorReporter
{
    
public:

    // Structure for notifying the host of errors
    
    struct HostNotifier : public FrameLib_Proxy
    {
        virtual void notify() = 0;
    };
    
    // Constructor
    
    FrameLib_ErrorReporter(HostNotifier *notifier) : mNotifier(notifier), mNotified (false), mReports(new ErrorList()) {}

    // Add and Retrieve Errors (list ownership is passed on retrieval)
    
    template<typename... Args>
    void reportError(ErrorSource source, FrameLib_Proxy *reporter, const char *error, Args... args)
    {
        mReports->add(source, reporter, error, args...);
        
        if (mNotifier && !mNotified)
        {
            mNotifier->notify();
            mNotified = true;
        }
    }
    
    std::unique_ptr<ErrorList> getErrors();
    
private:
    
    // Data
    
    HostNotifier *mNotifier;
    bool mNotified;
    std::unique_ptr<ErrorList> mReports;
    SpinLock mLock;
};

#endif
