
#ifndef FRAMELIB_PARAMETERS_H
#define FRAMELIB_PARAMETERS_H

#include "FrameLib_Types.h"
#include "FrameLib_Errors.h"

#include <vector>
#include <cstring>
#include <cassert>
#include <limits>
#include <string>

// FrameLib_Parameters

// This class deals with parameters of an object

static const char *typeStringsDouble[] = {"double", "enum", "string", "fixed length double array", "variable length double array" };
static const char *typeStringsInteger[] = {"int", "enum", "string", "fixed length int array", "variable length int array" };
static const char *typeStringsBool[] = {"bool", "enum", "string", "fixed length bool array", "variable length bool array" };

class FrameLib_Parameters
{
    
    enum SetError { kSetSucceeded, kUnknownArgument, kUnknownParameter, kParameterNotSetByNumber, kParameterNotSetByString, kEnumUnknownIndex, kEnumUnknownString };
    
public:
    
    enum NumericType { kNumericBool, kNumericInteger, kNumericDouble, kNumericNone };
    enum Type { kValue, kEnum, kString, kArray, kVariableArray };
    enum ClipMode { kNone, kMin, kMax, kClip };
    
public:
    
    // A Serialised Set Of Tagged Parameter Values (no memory ownership)
    
    class Serial
    {
        
    public:
        
        // N.B. the assumption is that double is the largest type in use
        
        static const size_t alignment = sizeof(double);
                
        class Iterator
        {
            struct Entry
            {
                DataType mType;
                char *mTag;
                BytePointer mData;
                size_t mSize;
                
                template <class T> T *data() { return reinterpret_cast<T *>(mData); }
            };
            
        public:
            
            // Constructor
            
            Iterator(const Serial *serial, bool end) : mPtr(serial->mPtr + (end ? serial->mSize : 0)), mIndex(end ? serial->mNumTags : 0) {}
            
            // Operators
            
            bool operator == (const Iterator& it) const { return mPtr == it.mPtr; }
            bool operator != (const Iterator& it) const { return !(*this == it); }

            Iterator& operator ++ ();
            Iterator& operator ++ (int);
            
            // Getters
            
            DataType getType() const    { return *(reinterpret_cast<DataType *>(mPtr)); }
            char *getTag() const        { return reinterpret_cast<char *>(mPtr + sizeType() + sizeSize()); }
            const double *getVector(unsigned long *size) const;
            unsigned long getVectorSize() const;
            char *getString() const;
            size_t getSize() const;
            unsigned long getIndex() const { return mIndex; }

            // Match Tag
            
            bool matchTag(const char *tag) const    {return !strcmp(tag, getTag()); }
            
            // Reads

            void read(Serial *serial) const;
            void read(FrameLib_Parameters *parameters) const;
            size_t read(double *output, unsigned long size) const;
            
            // Aliases (read but aliasing to a new tag)
            
            void alias(Serial *serial, const char *tag) const;
            void alias(FrameLib_Parameters *serial, const char *tag) const;

        private:
            
            // Get Entry
            
            Entry getEntry() const;
            
            // Data
            
            BytePointer mPtr;
            unsigned long mIndex;
        };
        
    public:
        
        Iterator begin() const  { return Iterator(this, false); }
        Iterator end() const    { return Iterator(this, true); }
        
        // Constructors and Destructor
        
        Serial(BytePointer ptr, size_t size);
        Serial();
        
        // Size Calculations
        
        static size_t calcSize(const Serial *serialised)            { return serialised != nullptr ? serialised->mSize : 0; }
        static size_t calcSize(const FrameLib_Parameters *params);
        static size_t calcSize(const char *tag, const char *str)    { return sizeType() + sizeString(tag) + sizeString(str); }
        static size_t calcSize(const char *tag, size_t N)           { return sizeType() + sizeString(tag) + sizeArray(N); }
        
        // Get Sizes
        
        size_t getSize(const char *tag) const;
        size_t getVectorSize(const char *tag) const;
        
        // Writes
        
        void write(const Serial *serialised);
        void write(const FrameLib_Parameters *params);
        void write(const Serial::Iterator& it);
        void write(const char *tag, const char *str);
        void write(const char *tag, const double *values, size_t N);
        
        // Reads
        
        void read(FrameLib_Parameters *parameters) const;
        size_t read(const char *tag, double *output, unsigned long size) const;
        bool read(const char *tag, FrameLib_Parameters *parameters) const;
        
        // Find Item
        
        Iterator find(const char *tag)  const;

        // Utility
        
        unsigned long numTags() const { return mNumTags; };
        size_t size() const     { return mSize; }
        void clear()            { mSize = 0; }
        
        static size_t alignSize(size_t size)                    { return (size + (alignment - 1)) & ~(alignment - 1); }
        static size_t inPlaceSize(size_t size)                  { return alignSize(sizeof(Serial)) + alignSize(size); }

        static Serial *newInPlace(void *ptr, size_t size)       { return new (ptr) Serial(((BytePointer) ptr) + alignSize(sizeof(Serial)), size); }

    protected:
        
        // Check Size
        
        bool checkSize(size_t writeSize);
        
    private:
        
        // Deleted
        
        Serial(const Serial&) = delete;
        Serial& operator=(const Serial&) = delete;
        
        // Debug
        
        void alignmentChecks() const;
        
        // Size Calculators
        
        static size_t sizeType()                    { return alignSize(sizeof(DataType)); }
        static size_t sizeSize()                    { return alignSize(sizeof(size_t)); }
        static size_t sizeString(const char *str)   { return sizeSize() + alignSize(strlen(str) + 1); }
        static size_t sizeArray(size_t N)           { return sizeSize() + alignSize((N * sizeof(double))); }

        // Write Item
        
        void writeType(DataType type);
        void writeSize(size_t size);
        void writeString(const char *str);
        void writeDoubles(const double *ptr, size_t N);
        
        // Read Item
        
        static DataType readType(BytePointer *readPtr);
        static void readSize(BytePointer *readPtr, size_t *size);
        static void readItem(BytePointer *readPtr, DataType type, BytePointer *data, size_t *size);

        // Skip Item
        
        static void skipItem(BytePointer *readPtr, DataType typ);
        
    protected:
        
        // Member Variables
        
        BytePointer mPtr;
        size_t mSize;
        size_t mMaxSize;
        unsigned long mNumTags;
    };

    // Extends Serial (with memory ownership)
    
    class AutoSerial : public Serial
    {
        static const size_t minGrowSize = 512;

    public:

        AutoSerial() {};
        AutoSerial(size_t size) : Serial(new Byte[size], size) {}
        AutoSerial(const Serial& serial) : Serial(new Byte[serial.size()], serial.size()) { write(&serial); }
        AutoSerial(const char *tag, const char *string) { write(tag, string); }
        AutoSerial(const char *tag, const double *values, size_t N)  { write(tag, values, N); }
        ~AutoSerial() { if (mPtr) delete[] mPtr; }
        
        // Write Items
        
        void write(const Serial *serialised)                            { if (checkSize(calcSize(serialised))) Serial::write(serialised); }
        void write(const char *tag, const char *str)                    { if (checkSize(calcSize(tag, str))) Serial::write(tag, str); }
        void write(const char *tag, const double *values, size_t N)     { if (checkSize(calcSize(tag, N))) Serial::write(tag, values, N); }
        
    private:
        
        bool checkSize(size_t writeSize);
    };
    
    // ************************************************************************************** //

    // Info Class for Passing in Info Strings
    
    class Info
    {
        
    public:
        
        void add(const char *str)               { mInfoStrings.push_back(str); }
        void add(const std::string &str)        { mInfoStrings.push_back(str); }
        const char *get(unsigned long idx)      { return (idx < mInfoStrings.size()) ? mInfoStrings[idx].c_str() : "No parameter info available"; }
        
    private:
        
        std::vector<std::string> mInfoStrings;
    };
    
    // ************************************************************************************** //

private:
    
    // Abstract Parameter Class
    
    class Parameter
    {
        
    public:
    
        enum Flags { kFlagInstantiation = 0x1, kFlagBool = 0x2, kFlagInteger = 0x4, kFlagNonNumeric = 0x8 };

        // Constructor / Destructor
        
        Parameter(const char *name, long argumentIdx);
        virtual ~Parameter() {};
       
        // Setters
        
        virtual void addEnumItem(const char *str);
        
        void setInstantiation()                         { mFlags |= kFlagInstantiation; }
        void setBoolOnly()                              { mFlags |= kFlagBool | kFlagInteger; }
        void setIntegerOnly()                           { mFlags |= kFlagInteger; }
        void setNonNumeric()                            { mFlags |= kFlagNonNumeric; }
        
        void setMin(double min);
        void setMax(double max);
        void setClip(double min, double max);
        
        virtual SetError set(const char *str) { return kParameterNotSetByString; }
        virtual SetError set(double value) { return kParameterNotSetByNumber; }
        virtual SetError set(double *values, size_t N);

        virtual void clear() = 0;
        
        // Getters
        
        // Setup
        
        virtual Type type() = 0;
        
        const char *name() const                        { return mName.c_str(); }
        long argumentIdx() const                        { return mArgumentIdx; }
        int flags() const                               { return mFlags; }
        
        ClipMode getClipMode() const;
        double getMin() const                           { return mMin; }
        double getMax()                                 { return mMax; }
        void getRange(double *min, double *max) const;
        
        virtual const char *getItemString(unsigned long item) const;

        // Values
        
        double getDefault() const                       { return mDefault; }

        virtual double getValue() const                 { return 0; }
        virtual const char *getString() const           { return nullptr; }
        virtual size_t getArraySize() const             { return 0; }
        virtual size_t getArrayMaxSize() const          { return 0; }
        virtual const double *getArray() const          { return nullptr; }
        const double *getArray(size_t *size) const;
        
        bool changed();
        
    protected:
        
        bool mChanged;
        int mFlags;
        
        double mDefault;
        double mMin;
        double mMax;
        
    private:
        
        std::string mName;
        long mArgumentIdx;
    };
    
    // ************************************************************************************** //

    // Enum Parameter Class

    class Enum : public Parameter
    {
        
    public:
        
        Enum(const char *name, long argumentIdx);
        
        // Setters
        
        void addEnumItem(const char *str);
        
        virtual SetError set(double value);
        virtual SetError set(double *values, size_t N);
        virtual SetError set(const char *str);
        
        virtual void clear() { Enum::set(0.0); };

        virtual Type type() { return kEnum; }
        
        // Getters
        
        virtual double getValue() const                                 { return mValue; }
        virtual const char *getString() const                           { return mItems[mValue].c_str(); }
        virtual const char *getItemString(unsigned long item) const     { return mItems[item].c_str(); }
        
    private:
        
        std::vector <std::string> mItems;
        unsigned long mValue;
    };
    
    // ************************************************************************************** //

    // Value Parameter Class

    class Value : public Parameter
    {
        
    public:
        
        Value(const char *name, long argumentIdx, double defaultValue) : Parameter(name, argumentIdx), mValue(defaultValue)
        { mDefault = defaultValue; }
        
        // Setters

        virtual SetError set(double value);
        virtual SetError set(double *values, size_t N);
        
        virtual void clear() { Value::set(mDefault); };

        // Getters

        virtual Type type() { return kValue; }
        
        virtual double getValue() const { return mValue; }
       
    private:
        
        double mValue;
    };
    
    // ************************************************************************************** //

    // String Parameter Class

    class String : public Parameter
    {
        const static size_t maxLen = 128;
        
    public:
        
        String(const char *name, long argumentIdx);
        
        // Setters
        
        virtual SetError set(const char *str);
        
        virtual void clear() { String::set(nullptr); };

        // Getters
        
        virtual Type type() { return kString; }
        
        virtual const char *getString() const   { return mCString; }

    private:
        
        char mCString[maxLen + 1];
    };

    // ************************************************************************************** //

    // Array Parameter Class

    class Array : public Parameter, private std::vector<double>
    {
    
    public:
        
        Array(const char *name, long argumentIdx, double defaultValue, size_t size);
        Array(const char *name, long argumentIdx, double defaultValue, size_t maxSize, size_t size);

        // Setters

        virtual SetError set(double *values, size_t N);

        virtual void clear() { Array::set(nullptr, 0); };

        // Getters

        virtual Type type() { return mVariableSize ? kVariableArray : kArray; }
        
        virtual size_t getArraySize() const         { return mSize; }
        virtual size_t getArrayMaxSize() const      { return mItems.size(); }
        virtual const double * getArray() const     { return mItems.data(); }

    private:
        
        std::vector<double> mItems;
        size_t mSize;
    
        const bool mVariableSize;
    };
    
    // ************************************************************************************** //

public:
    
    // Constructor
    
    FrameLib_Parameters(FrameLib_ErrorReporter& errorReporter, FrameLib_Proxy *proxy, Info *info)
    : mErrorReporter(errorReporter), mProxy(proxy), mReportErrors(true), mParameterInfo(info) {}
    
    // Destructor
    
    ~FrameLib_Parameters()
    {
        for (auto it = mParameters.begin(); it != mParameters.end(); it++)
            delete *it;
    }
    
    // Enable/Disable Error Reporting (enabled by default)
    
    void setErrorReportingEnabled(bool enable)  { mReportErrors = enable; }
    
    // Size and Index
    
    unsigned long size() const { return mParameters.size(); }
    
    long getIdx(const char *name) const
    {
        for (unsigned long i = 0; i < mParameters.size(); i++)
            if (strcmp(name, mParameters[i]->name()) == 0)
                return i;
        
        long argumentIdx = convertToNumber(name);
        
        if (argumentIdx >= 0)
            for (unsigned long i = 0; i < mParameters.size(); i++)
                if (argumentIdx == mParameters[i]->argumentIdx())
                    return i;
        
        if (argumentIdx >= 0)
            handleError(kUnknownArgument, argumentIdx, argumentIdx);
        else
            handleError(kUnknownParameter, -1, name);
        
        return -1;
    }

    long maxArgument() const
    {
        long argument = -1;
        
        for (unsigned long i = 0; i < mParameters.size(); i++)
            argument = std::max(mParameters[i]->argumentIdx(), argument);
        
        return argument;
    }
    
    // Add Parameters
    
    void addBool(unsigned long index, const char *name, bool defaultValue = false, long argumentIdx = -1)
    {
        addParameter(index, new Value(name, argumentIdx, defaultValue));
        mParameters.back()->setClip(false, true);
        mParameters.back()->setBoolOnly();
    }
    
    void addDouble(unsigned long index, const char *name, double defaultValue = 0.0, long argumentIdx = -1)
    {
        addParameter(index, new Value(name, argumentIdx, defaultValue));
    }
    
    void addInt(unsigned long index, const char *name, long defaultValue = 0, long argumentIdx = -1)
    {
        addParameter(index, new Value(name, argumentIdx, defaultValue));
        mParameters.back()->setIntegerOnly();
    }
    
    void addString(unsigned long index, const char *name, long argumentIdx = -1)
    {
        addParameter(index, new String(name, argumentIdx));
    }
    
    void addEnum(unsigned long index, const char *name, long argumentIdx = -1)
    {
       addParameter(index, new Enum(name, argumentIdx));
    }
    
    void addEnumItem(unsigned long index, const char *str)
    {
        mParameters.back()->addEnumItem(str);
    }
    
    void addBoolArray(unsigned long index, const char *name, bool defaultValue, size_t size, long argumentIdx = -1)
    {
        addParameter(index, new Array(name, argumentIdx, defaultValue, size));
        mParameters.back()->setBoolOnly();
    }
    
    void addIntArray(unsigned long index, const char *name, long defaultValue, size_t size, long argumentIdx = -1)
    {
        addParameter(index, new Array(name, argumentIdx, defaultValue, size));
        mParameters.back()->setIntegerOnly();
    }
    
    void addDoubleArray(unsigned long index, const char *name, double defaultValue, size_t size, long argumentIdx = -1)
    {
        addParameter(index, new Array(name, argumentIdx, defaultValue, size));
    }
    
    void addVariableBoolArray(unsigned long index, const char *name, long defaultValue, size_t maxSize, size_t size, long argumentIdx = -1)
    {
        addParameter(index, new Array(name, argumentIdx, defaultValue, maxSize, size));
        mParameters.back()->setBoolOnly();
    }
    
    void addVariableIntArray(unsigned long index, const char *name, long defaultValue, size_t maxSize, size_t size, long argumentIdx = -1)
    {
        addParameter(index, new Array(name, argumentIdx, defaultValue, maxSize, size));
        mParameters.back()->setIntegerOnly();
    }
    
    void addVariableDoubleArray(unsigned long index, const char *name, double defaultValue, size_t maxSize, size_t size, long argumentIdx = -1)
    {
        addParameter(index, new Array(name, argumentIdx, defaultValue, maxSize, size));
    }

    // Setters (N.B. - setters have sanity checks as the tags are set by the end-user)
    
    // Set as Instantiation Only
    
    void setInstantiation()                     { mParameters.back()->setInstantiation(); }

    // Set Range
    
    void setMin(double min)                     { mParameters.back()->setMin(min); }
    void setMax(double max)                     { mParameters.back()->setMax(max); }
    void setClip(double min, double max)        { mParameters.back()->setClip(min, max); }
   
    // Error handling
    
    template <typename T>
    void handleError(SetError error, long idx, T arg) const
    {
        if (error && mReportErrors)
        {
            switch (error)
            {
                case kUnknownArgument:
                    mErrorReporter.reportError(kErrorParameter, mProxy, "argument # out of range", idx);
                    break;
                case kUnknownParameter:
                    mErrorReporter.reportError(kErrorParameter, mProxy, "no parameter named '#'", arg);
                    break;
                case kParameterNotSetByNumber:
                    mErrorReporter.reportError(kErrorParameter, mProxy, "parameter '#' cannot be set by a number", mParameters[idx]->name());
                    break;
                case kParameterNotSetByString:
                    mErrorReporter.reportError(kErrorParameter, mProxy, "parameter '#' cannot be set by a string", mParameters[idx]->name());
                    break;
                case kEnumUnknownIndex:
                    mErrorReporter.reportError(kErrorParameter, mProxy, "enum parameter '#' does not contain an item numbered #", mParameters[idx]->name(), arg);
                    break;
                case kEnumUnknownString:
                    mErrorReporter.reportError(kErrorParameter, mProxy, "enum parameter '#' does not contain an item named '#'", mParameters[idx]->name(), arg);
                    break;
                default:
                    break;
            }
        }
    }
    
    // Set Value
    
    void set(Serial *serialised)                                { if (serialised) serialised->read(this); }

    void set(unsigned long idx, bool value)                     { set(idx, (double) value); }
    void set(const char *name, bool value)                      { set(name, (double) value); }
    
    void set(unsigned long idx, long value)                     { set(idx, (double) value); }
    void set(const char *name, long value)                      { set(name, (double) value); }

    void set(unsigned long idx, double value)                   { if (idx < size()) handleError(mParameters[idx]->set(value), idx, value); }
    void set(const char *name, double value)                    { set(getIdx(name), value); }
    
    void set(unsigned long idx, char *str)                      { if (idx < size()) handleError(mParameters[idx]->set(str), idx, str); }
    void set(const char *name, char *str)                       { set(getIdx(name), str); }
    
    void set(unsigned long idx, double *values, size_t N)       { if (idx < size()) handleError(mParameters[idx]->set(values, N), idx, *values); }
    void set(const char *name, double *values, size_t N)        { set(getIdx(name), values, N); }

    void clear(unsigned long idx)                               { if (idx < size()) mParameters[idx]->clear(); }
    void clear(const char *name)                                { clear(getIdx(name)); }

    // Getters (N.B. - getters have no sanity checks, because they are the programmer's responsibility)
    
    // Get Name
    
    std::string getName(unsigned long idx) const                            { return mParameters[idx]->name(); }
   
    long getArgumentIdx(unsigned long idx) const                            { return mParameters[idx]->argumentIdx(); }
    long getArgumentIdx(const char *name) const                             { return mParameters[getIdx(name)]->argumentIdx(); }
    
    // Get Type
    
    Type getType(unsigned long idx) const                                   { return mParameters[idx]->type(); }
    Type getType(const char *name) const                                    { return getType(getIdx(name)); }
    
    NumericType getNumericType(unsigned long idx) const;
    NumericType getNumericType(const char *name) const                      { return getNumericType(getIdx(name)); }
    
    // N.B. the type string includes details of numeric type / instantion only
    
    std::string getTypeString(unsigned long idx) const;
    std::string getTypeString(const char *name) const                       { return getTypeString(getIdx(name)); }

    // Get Range
    
    ClipMode getClipMode(unsigned long idx) const                           { return mParameters[idx]->getClipMode(); }
    ClipMode getClipMode(const char *name) const                            { return getClipMode(getIdx(name)); }

    double getMin(unsigned long idx) const                                  { return mParameters[idx]->getMin(); }
    double getMin(const char *name) const                                   { return getMin(getIdx(name)); }
    
    double getMax(unsigned long idx) const                                  { return mParameters[idx]->getMax(); }
    double getMax(const char *name) const                                   { return getMax(getIdx(name)); }
    
    void getRange(unsigned long idx, double *min, double *max) const        { return mParameters[idx]->getRange(min, max); }
    void getRange(const char *name, double *min, double *max) const         { return getRange(getIdx(name), min, max); }
        
    // Get Item Strings
    
    std::string getItemString(unsigned long idx, unsigned long item) const  { return mParameters[idx]->getItemString(item); }
    std::string getItemString(const char *name, unsigned long item) const   { return getItemString(getIdx(name), item); }
    
    // Get Info
    
    std::string getInfo(unsigned long idx) const                            { return mParameterInfo ? mParameterInfo->get(idx) : "No parameter info available"; }
    std::string getInfo(const char *name) const                             { return getInfo(getIdx(name)); }

    // Default Values
    
    double getDefault(unsigned long idx) const                              { return mParameters[idx]->getDefault(); }
    double getDefault(const char *name) const                               { return getDefault(getIdx(name)); }

    std::string getDefaultString(unsigned long idx) const;
    std::string getDefaultString(const char *name) const                    { return getDefaultString(getIdx(name)); }

    // Get Value
    
    double getValue(unsigned long idx) const                                { return mParameters[idx]->getValue(); }
    double getValue(const char *name) const                                 { return getValue(getIdx(name)); }
    
    long getInt(unsigned long idx) const                                    { return (long) getValue(idx); }
    long getInt(const char *name) const                                     { return getInt(getIdx(name)); }
    
    long getBool(unsigned long idx) const                                   { return (bool) getValue(idx); }
    bool getBool(const char *name) const                                    { return (bool) getValue(getIdx(name)); }
    
    const char *getString(unsigned long idx) const                          { return mParameters[idx]->getString(); }
    const char *getString(const char *name) const                           { return getString(getIdx(name)); }
    
    const double *getArray(unsigned long idx) const                         { return mParameters[idx]->getArray(); }
    const double *getArray(const char *name) const                          { return getArray(getIdx(name)); }
    const double *getArray(unsigned long idx, size_t *size) const           { return mParameters[idx]->getArray(size); }
    const double *getArray(const char *name, size_t *size) const            { return getArray(getIdx(name), size); }
    
    size_t getArraySize(unsigned long idx) const                            { return mParameters[idx]->getArraySize(); }
    size_t getArraySize(const char *name) const                             { return getArraySize(getIdx(name)); }

    size_t getArrayMaxSize(unsigned long idx) const                         { return mParameters[idx]->getArrayMaxSize(); }
    size_t getArrayMaxSize(const char *name) const                          { return getArrayMaxSize(getIdx(name)); }

    bool changed(unsigned long idx)                                         { return mParameters[idx]->changed(); }
    bool changed(const char *name)                                          { return changed(getIdx(name)); }
    
private:
    
    // Deleted
    
    FrameLib_Parameters(const FrameLib_Parameters&) = delete;
    FrameLib_Parameters& operator=(const FrameLib_Parameters&) = delete;
    
    // Utility
    
    void addParameter(unsigned long index, Parameter *attr)
    {
        assert((index == mParameters.size()) && "parameters must be added in order");
        mParameters.push_back(attr);
    }
    
    static long convertToNumber(const char *name)
    {
        long result = 0;
        
        for (unsigned long i = 0; ; i++)
        {
            long current = name[i];
            
            if (current == 0 && i)
                return result;
            
            if (current < '0' || current > '9')
                return -1;
            
            result = (result * 10) + (current - '0');
        }
    }
    
    // Data
    
    FrameLib_ErrorReporter& mErrorReporter;
    FrameLib_Proxy *mProxy;
    bool mReportErrors;
    
    std::vector <Parameter *> mParameters;
    Info *mParameterInfo;
};

#endif
