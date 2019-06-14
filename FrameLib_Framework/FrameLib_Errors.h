
#ifndef FRAMELIB_ERRORS_H
#define FRAMELIB_ERRORS_H

#include "FrameLib_Threading.h"
#include "FrameLib_Types.h"

#include <algorithm>
#include <memory>
#include <string>
#include <cstdio>
#include <cstring>

/**
 
 @defgroup Hosting
 
 */


// Possible sources of error

enum ErrorSource { kErrorObject, kErrorParameter, kErrorMemory, kErrorDSP  };

/**
 
 @class FrameLib_ErrorReporter
 
 @ingroup Hosting
 
 @brief a class used to report errors to the host environment.
 
 */

class FrameLib_ErrorReporter
{
    
public:
    
    /**
     
     @class HostNotifier
     
     @brief a virtual struct used to supply a method for notifying the host of errors.
     
     */
    
    struct HostNotifier : public FrameLib_Proxy
    {
        virtual void notify() = 0;
    };
    
    /**
     
     @class ErrorReport
     
     @brief a report for a single error.
     
     */

    class ErrorReport
    {
        friend FrameLib_ErrorReporter;
        
        ErrorReport(ErrorSource source, FrameLib_Proxy *reporter, const char *error, const char *items, unsigned long numItems)
        : mSource(source), mReporter(reporter), mError(error), mItems(items), mNumItems(numItems) {}
        
    public:
        
        ErrorReport() : mSource(kErrorObject), mReporter(nullptr), mError(nullptr), mItems(nullptr), mNumItems(0) {}

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
    
    /**
     
     @class ErrorList
     
     @brief a list of ErrorReport objects.
     
     */
    
    class ErrorList
    {
        friend FrameLib_ErrorReporter;
        
        const static int sCharArraySize = 8192;
        const static int sReportArraySize = 1024;
        
    public:
        
    public:
        
        /**
         
         @class ConstIterator
         
         @brief an iterator for reports in the list (with an underlying const ErrorReport type).
         
         */
        
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
        
        ErrorList() : mReportsSize(0), mItemsSize(0), mFull(false) {}
        
        // Non-copyable
        
        ErrorList(const ErrorList&) = delete;
        ErrorList& operator=(const ErrorList&) = delete;
        
        const ErrorReport operator[](size_t idx) const { return mReports[idx]; }
        size_t size() const { return mReportsSize; }
        
        ConstIterator begin() const { return mReports; }
        ConstIterator end() const { return mReports + mReportsSize; }
        
        bool isFull() const { return mFull; }
        
    private:
        
        bool addItem(const char *str)
        {
            char *ptr = mItems + mItemsSize;
            size_t size = strlen(str) + 1;
            
            if (mItemsSize + size < sCharArraySize)
            {
                std::copy(str, str + size, ptr);
                mItemsSize += size;
                return true;
            }
            
            return false;
        }
        
        bool addItem(size_t number)
        {
            const int strBufferSize = 32;
            char charArray[strBufferSize];
            snprintf(charArray, strBufferSize, "%zu", number);
            return addItem(charArray);
        }
        
        bool addItem(long number)
        {
            const int strBufferSize = 32;
            char charArray[strBufferSize];
            snprintf(charArray, strBufferSize, "%ld", number);
            return addItem(charArray);
        }
        
        bool addItem(double number)
        {
            const int strBufferSize = 32;
            char charArray[strBufferSize];
            snprintf(charArray, strBufferSize, "%lf", number);
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
    FrameLib_SpinLock mLock;
};

#endif
