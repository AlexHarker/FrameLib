
#ifndef FRAMELIB_ERRORS_H
#define FRAMELIB_ERRORS_H

#include "FrameLib_Types.h"
#include "FrameLib_Threading.h"

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
     
     @class ErrorReport
     
     @brief a report for a single error.
     
     */

    class ErrorReport
    {
        friend class FrameLib_ErrorReporter;
        
        ErrorReport(ErrorSource source, FrameLib_Proxy *reporter, const char *error, const char *items, size_t numChars, unsigned long numItems)
        : mSource(source), mReporter(reporter), mError(error), mItems(items), mItemSize(numChars), mNumItems(numItems) {}
        
    public:
        
        ErrorReport() : mSource(kErrorObject), mReporter(nullptr), mError(nullptr), mItems(nullptr), mItemSize(0), mNumItems(0) {}

        void getErrorText(std::string& text) const;
        void getErrorText(char *text, size_t N) const;
        ErrorSource getSource() const                   { return mSource; }
        FrameLib_Proxy* getReporter() const             { return mReporter; }
    
    private:
        
        void copy(char*& dest, const char *str, size_t length, size_t& left) const;

        // Data
        
        ErrorSource mSource;
        FrameLib_Proxy* mReporter;
        const char* mError;
        const char* mItems;
        size_t mItemSize;
        unsigned long mNumItems;
    };
    
    /**
     
     @class ErrorList
     
     @brief a list of ErrorReport objects.
     
     */
    
    class ErrorList
    {
        friend class FrameLib_ErrorReporter;
        
        const static int sCharArraySize = 8192;
        const static int sReportArraySize = 1024;
        
    public:
        
        /**
         
         @class ConstIterator
         
         @brief an iterator for reports in the list (with an underlying const ErrorReport type).
         
         */
        
        class ConstIterator
        {
            friend class ErrorList;
            
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
        ConstIterator last() const { return mReports + mReportsSize - 1; }
        ConstIterator end() const { return mReports + mReportsSize; }
        
        bool isFull() const { return mFull; }
        
    private:
        
        char *getItemsPointer() { return mItems + mItemsSize; }
        
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
            const int strBufSize = 32;
            char charArray[strBufSize];
            snprintf(charArray, strBufSize, "%zu", number);
            return addItem(charArray);
        }
        
        bool addItem(long number)
        {
            const int strBufSize = 32;
            char charArray[strBufSize];
            snprintf(charArray, strBufSize, "%ld", number);
            return addItem(charArray);
        }
        
        bool addItem(int number)
        {
            return addItem(static_cast<long>(number));
        }
        
        bool addItem(double number)
        {
            const int strBufSize = 32;
            char charArray[strBufSize];
            snprintf(charArray, strBufSize, "%lf", number);
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
            char *ptr = getItemsPointer();
            
            if (!mFull && (mReportsSize < sReportArraySize) && addItems(args...))
            {
                size_t itemSize = getItemsPointer() - ptr;
                mReports[mReportsSize] = ErrorReport(source, reporter, error, ptr, itemSize, sizeof...(args));
                mReportsSize++;
            }
            else
                mFull = true;
        }
        
        void remove()
        {            
            if (mReportsSize)
            {
                mFull = false;
                mReportsSize--;
                mItemsSize -= mReports[mReportsSize].mItemSize;
                mReports[mReportsSize] = ErrorReport();
            }
        }
        
        // Data
        
        ErrorReport mReports[sReportArraySize];
        char mItems[sCharArraySize];
        size_t mReportsSize;
        size_t mItemsSize;
        bool mFull;
    };
    
    /**
     
     @class HostNotifier
     
     @brief a virtual struct used to supply a method for notifying the host of errors.
     
     */
    
    struct HostNotifier : public FrameLib_Proxy
    {
        virtual bool notify(const ErrorReport& report) = 0;
    };
    
    // Constructor
    
    FrameLib_ErrorReporter(HostNotifier *notifier) : mNotifier(notifier), mNotified (false), mReports(new ErrorList()) {}

    // Add and Retrieve Errors (list ownership is passed on retrieval)
    
    template<typename... Args>
    void operator()(ErrorSource source, FrameLib_Proxy *reporter, const char *error, Args... args)
    {
        FrameLib_SpinLockHolder lockHolder(&mLock);

        mReports->add(source, reporter, error, args...);
        
        if (mNotifier && !mNotified)
        {
            if (mNotifier->notify(*(mReports->last())))
                mReports->remove();
            else
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
