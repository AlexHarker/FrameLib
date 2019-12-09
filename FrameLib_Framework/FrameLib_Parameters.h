
#ifndef FRAMELIB_PARAMETERS_H
#define FRAMELIB_PARAMETERS_H

#include "FrameLib_Types.h"
#include "FrameLib_Errors.h"

#include <vector>
#include <cstring>
#include <cassert>
#include <limits>
#include <string>

/**
 
 @defgroup Parameters
 
 */

/**
 
 @class FrameLib_Parameters
 
 @ingroup Parameters
 
 @brief a set of parameters for a FrameLib object.
 
 */

class FrameLib_Parameters
{
    
    enum SetError { kSetSucceeded, kUnknownArgument, kUnknownParameter, kParameterNotSetByNumber, kParameterNotSetByString, kEnumUnknownIndex, kEnumUnknownString };
    
public:
    
    enum NumericType { kNumericBool, kNumericInteger, kNumericDouble, kNumericNone };
    enum Type { kValue, kEnum, kString, kArray, kVariableArray };
    enum ClipMode { kNone, kMin, kMax, kClip };
    
public:
    
    /**
     
     @class Serial
     
     @brief a set of tagged parameter values using external non-owned memory.
     
     */
    
    class Serial
    {
        
    public:
        
        // N.B. the assumption is that double is the largest type in use
        
        static const size_t alignment = sizeof(double);
        
        /**
         
         @class Iterator
         
         @brief an iterator for the items in a Serial.
         
         */
        
        class Iterator
        {
            /**
             
             @struct Entry
             
             @brief data for a single entry in a Serial.
             
             */
            
            struct Entry
            {
                DataType mType;
                char *mTag;
                BytePointer mData;
                unsigned long mSize;
                
                template <class T> T *data() { return reinterpret_cast<T *>(mData); }
            };
            
        public:
            
            // Constructor
            
            Iterator(const Serial *serial, bool end) : mPtr(serial->mPtr + (end ? serial->mSize : 0)), mIndex(end ? serial->mNumTags : 0) {}
            
            // Operators
            
            bool operator == (const Iterator& it) const { return mPtr == it.mPtr; }
            bool operator != (const Iterator& it) const { return !(*this == it); }
            
            Iterator& operator ++ ();
            Iterator operator ++ (int);
            
            // Getters
            
            DataType getType() const    { return *(reinterpret_cast<DataType *>(mPtr)); }
            char *getTag() const        { return reinterpret_cast<char *>(mPtr + sizeType() + sizeSize()); }
            const double *getVector(unsigned long *size) const;
            unsigned long getVectorSize() const;
            char *getString() const;
            unsigned long getSize() const;
            unsigned long getIndex() const { return mIndex; }
            
            // Match Tag
            
            bool matchTag(const char *tag) const    {return !strcmp(tag, getTag()); }
            
            // Reads
            
            void read(Serial *serial) const;
            void read(FrameLib_Parameters *parameters) const;
            unsigned long read(double *output, unsigned long size) const;
            
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
        
        Serial(BytePointer ptr, unsigned long size);
        Serial();
        
        // Non-copyable
        
        Serial(const Serial&) = delete;
        Serial& operator=(const Serial&) = delete;
        
        // Size Calculations
        
        static unsigned long calcSize(const Serial *serialised)             { return serialised != nullptr ? serialised->mSize : 0; }
        static unsigned long calcSize(const FrameLib_Parameters *params);
        static unsigned long calcSize(const char *tag, const char *str)     { return sizeType() + sizeString(tag) + sizeString(str); }
        static unsigned long calcSize(const char *tag, unsigned long N)     { return sizeType() + sizeString(tag) + sizeArray(N); }
        static unsigned long calcString(size_t tagLength, size_t strLength) { return sizeType() + sizeString(tagLength) + sizeString(strLength); }
        static unsigned long calcVector(size_t tagLength, unsigned long N)  { return sizeType() + sizeString(tagLength) + sizeArray(N); }
        
        // Get Sizes
        
        unsigned long getSize(const char *tag) const;
        unsigned long getVectorSize(const char *tag) const;
        
        // Writes
        
        void write(const Serial *serialised);
        void write(const FrameLib_Parameters *params);
        void write(const Serial::Iterator& it);
        void write(const char *tag, const char *str);
        void write(const char *tag, const double *values, unsigned long N);
        
        // Reads
        
        void read(FrameLib_Parameters *parameters) const;
        unsigned long read(const char *tag, double *output, unsigned long size) const;
        bool read(const char *tag, FrameLib_Parameters *parameters) const;
        
        // Find Item
        
        Iterator find(const char *tag)  const;
        
        // Utility
        
        unsigned long numTags() const       { return mNumTags; };
        unsigned long size() const          { return mSize; }
        void clear()                        { mSize = 0; }
        
        static unsigned long alignSize(size_t size)
        {
            return static_cast<unsigned long>((size + (alignment - 1)) & ~(alignment - 1));
        }
        
        static size_t inPlaceSize(unsigned long size)
        {
            return alignSize(sizeof(Serial)) + alignSize(size);
        }
        
        static Serial *newInPlace(void *ptr, unsigned long size)
        {
            return new (ptr) Serial(((BytePointer) ptr) + alignSize(sizeof(Serial)), size);
        }
        
    protected:
        
        // Check Size
        
        bool checkSize(unsigned long writeSize);
        
    private:
        
        // Debug
        
        void alignmentChecks() const;
        
        // Size Calculators
        
        static unsigned long sizeType()                    { return alignSize(sizeof(DataType)); }
        static unsigned long sizeSize()                    { return alignSize(sizeof(unsigned long)); }
        static unsigned long sizeString(const char *str)   { return sizeSize() + alignSize(strlen(str) + 1); }
        static unsigned long sizeString(size_t length)     { return sizeSize() + alignSize(length + 1); }
        static unsigned long sizeArray(unsigned long N)    { return sizeSize() + alignSize((N * sizeof(double))); }
        
        // Write Item
        
        void writeType(DataType type);
        void writeSize(unsigned long size);
        void writeString(const char *str);
        void writeDoubles(const double *ptr, unsigned long N);
        
        // Read Item
        
        static DataType readType(BytePointer *readPtr);
        static void readSize(BytePointer *readPtr, unsigned long *size);
        static void readItem(BytePointer *readPtr, DataType type, BytePointer *data, unsigned long *size);
        
        // Skip Item
        
        static void skipItem(BytePointer *readPtr, DataType typ);
        
    protected:
        
        // Member Variables
        
        BytePointer mPtr;
        unsigned long mSize;
        unsigned long mMaxSize;
        unsigned long mNumTags;
    };
    
    
    /**
     
     @class AutoSerial
     
     @brief an extension of Serial that manages its own resizable memory.
     
     */
    
    class AutoSerial : public Serial
    {
        static const unsigned long minGrowSize = 512;
        
    public:
        
        AutoSerial() {};
        AutoSerial(unsigned long size) : Serial(new Byte[size], size) {}
        AutoSerial(const Serial& serial) : Serial(new Byte[serial.size()], serial.size()) { write(&serial); }
        AutoSerial(const char *tag, const char *string) { write(tag, string); }
        AutoSerial(const char *tag, const double *values, unsigned long N)  { write(tag, values, N); }
        ~AutoSerial() { if (mPtr) delete[] mPtr; }
        
        // Write Items
        
        void write(const Serial *serialised)                                { if (checkSize(calcSize(serialised))) Serial::write(serialised); }
        void write(const char *tag, const char *str)                        { if (checkSize(calcSize(tag, str))) Serial::write(tag, str); }
        void write(const char *tag, const double *values, unsigned long N)  { if (checkSize(calcSize(tag, N))) Serial::write(tag, values, N); }
        
    private:
        
        bool checkSize(unsigned long writeSize);
    };
    
    /**
     
     @class Info
     
     @brief a class for passing info strings to FrameLib_Parameters.
     
     */
    
    class Info
    {
        
    public:
        
        void add(const char *str)               { mInfoStrings.push_back(str); }
        void add(const std::string &str)        { mInfoStrings.push_back(str); }
        const char *get(unsigned long idx)      { return (idx < mInfoStrings.size()) ? mInfoStrings[idx].c_str() : "No parameter info available"; }
        
    private:
        
        std::vector<std::string> mInfoStrings;
    };
    
private:
    
    /**
     
     @class Parameter
     
     @brief an abstract class representing a parameter.
     
     */
    
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
        virtual SetError set(double *values, unsigned long N);
        
        virtual void clear() = 0;
        
        // Getters
        
        // Setup
        
        virtual Type type() = 0;
        
        const char *name() const                            { return mName.c_str(); }
        long argumentIdx() const                            { return mArgumentIdx; }
        int flags() const                                   { return mFlags; }
        
        ClipMode getClipMode() const;
        double getMin() const                               { return mMin; }
        double getMax()                                     { return mMax; }
        void getRange(double *min, double *max) const;
        
        virtual const char *getItemString(unsigned long item) const;
        
        // Values
        
        double getDefault() const                           { return mDefault; }
        
        virtual double getValue() const                     { return 0; }
        virtual const char *getString() const               { return nullptr; }
        virtual unsigned long getArraySize() const          { return 0; }
        virtual unsigned long getArrayMaxSize() const       { return 0; }
        virtual const double *getArray() const              { return nullptr; }
        const double *getArray(unsigned long *size) const;
        
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
    
    /**
     
     @class Parameter
     
     @brief an enumerated parameter class.
     
     */
    
    class Enum final : public Parameter
    {
        
    public:
        
        Enum(const char *name, long argumentIdx);
        
        // Setters
        
        void addEnumItem(const char *str) override;
        
        SetError set(double value) override;
        SetError set(double *values, unsigned long N) override;
        virtual SetError set(const char *str) override;
        
        void clear() override { Enum::set(0.0); }
        
        virtual Type type() override { return kEnum; }
        
        // Getters
        
        virtual double getValue() const override                                { return mValue; }
        virtual const char *getString() const override                          { return mItems[mValue].c_str(); }
        virtual const char *getItemString(unsigned long item) const override    { return mItems[item].c_str(); }
        
    private:
        
        std::vector<std::string> mItems;
        unsigned long mValue;
    };
    
    /**
     
     @class Value
     
     @brief a numeric parameter class storing a single value as a double.
     
     */
    
    class Value final : public Parameter
    {
        
    public:
        
        Value(const char *name, long argumentIdx, double defaultValue) : Parameter(name, argumentIdx), mValue(defaultValue)
        { mDefault = defaultValue; }
        
        // Setters
        
        SetError set(double value) override;
        SetError set(double *values, unsigned long N) override;
        
        void clear() override { Value::set(mDefault); };
        
        // Getters
        
        Type type() override { return kValue; }
        
        double getValue() const override { return mValue; }
        
    private:
        
        double mValue;
    };
    
    /**
     
     @class String
     
     @brief a string parameter class.
     
     */
    
    class String final : public Parameter
    {
        const static size_t maxLen = 128;
        
    public:
        
        String(const char *name, long argumentIdx);
        
        // Setters
        
        SetError set(const char *str) override;
        
        void clear() override { String::set(nullptr); };
        
        // Getters
        
        Type type() override { return kString; }
        
        const char *getString() const override   { return mCString; }
        
    private:
        
        char mCString[maxLen + 1];
    };
    
    /**
     
     @class Array
     
     @brief a numeric parameter class storing an array of values.
     
     */
    
    // Array Parameter Class
    
    class Array final : public Parameter, private std::vector<double>
    {
        
    public:
        
        Array(const char *name, long argumentIdx, double defaultValue, unsigned long size);
        Array(const char *name, long argumentIdx, double defaultValue, unsigned long maxSize, unsigned long size);
        
        // Setters
        
        SetError set(double *values, unsigned long N) override;
        
        void clear() override { Array::set(nullptr, 0); };
        
        // Getters
        
        Type type() override { return mVariableSize ? kVariableArray : kArray; }
        
        unsigned long getArraySize() const override         { return mSize; }
        unsigned long getArrayMaxSize() const override      { return static_cast<unsigned long>(mItems.size()); }
        const double * getArray() const override            { return mItems.data(); }
        
    private:
        
        std::vector<double> mItems;
        unsigned long mSize;
        
        const bool mVariableSize;
    };
    
public:
    
    // Constructor
    
    FrameLib_Parameters(FrameLib_ErrorReporter& errorReporter, FrameLib_Proxy *proxy, Info *info)
    : mErrorReporter(errorReporter), mProxy(proxy), mReportErrors(true), mParameterInfo(info) {}
    
    // Non-copyable
    
    FrameLib_Parameters(const FrameLib_Parameters&) = delete;
    FrameLib_Parameters& operator=(const FrameLib_Parameters&) = delete;
    
    // Enable/Disable Error Reporting (enabled by default)
    
    void setErrorReportingEnabled(bool enable)  { mReportErrors = enable; }
    
    // Size and Index
    
    unsigned long size() const { return static_cast<unsigned long>(mParameters.size()); }
    
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
    
    void addBoolArray(unsigned long index, const char *name, bool defaultValue, unsigned long size, long argumentIdx = -1)
    {
        addParameter(index, new Array(name, argumentIdx, defaultValue, size));
        mParameters.back()->setBoolOnly();
    }
    
    void addIntArray(unsigned long index, const char *name, long defaultValue, unsigned long size, long argumentIdx = -1)
    {
        addParameter(index, new Array(name, argumentIdx, defaultValue, size));
        mParameters.back()->setIntegerOnly();
    }
    
    void addDoubleArray(unsigned long index, const char *name, double defaultValue, unsigned long size, long argumentIdx = -1)
    {
        addParameter(index, new Array(name, argumentIdx, defaultValue, size));
    }
    
    void addVariableBoolArray(unsigned long index, const char *name, long defaultValue, unsigned long maxSize, unsigned long size, long argumentIdx = -1)
    {
        addParameter(index, new Array(name, argumentIdx, defaultValue, maxSize, size));
        mParameters.back()->setBoolOnly();
    }
    
    void addVariableIntArray(unsigned long index, const char *name, long defaultValue, unsigned long maxSize, unsigned long size, long argumentIdx = -1)
    {
        addParameter(index, new Array(name, argumentIdx, defaultValue, maxSize, size));
        mParameters.back()->setIntegerOnly();
    }
    
    void addVariableDoubleArray(unsigned long index, const char *name, double defaultValue, unsigned long maxSize, unsigned long size, long argumentIdx = -1)
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
    
    void set(unsigned long idx, double *values, unsigned long N)
    {
        if (idx < size())
            handleError(mParameters[idx]->set(values, N), idx, *values);
    }
    
    void set(const char *name, double *values, unsigned long N)
    {
        set(getIdx(name), values, N);
    }
    
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
    const double *getArray(unsigned long idx, unsigned long *size) const    { return mParameters[idx]->getArray(size); }
    const double *getArray(const char *name, unsigned long *size) const        { return getArray(getIdx(name), size); }
    
    unsigned long getArraySize(unsigned long idx) const                     { return mParameters[idx]->getArraySize(); }
    unsigned long getArraySize(const char *name) const                      { return getArraySize(getIdx(name)); }
    
    unsigned long getArrayMaxSize(unsigned long idx) const                  { return mParameters[idx]->getArrayMaxSize(); }
    unsigned long getArrayMaxSize(const char *name) const                   { return getArrayMaxSize(getIdx(name)); }
    
    bool changed(unsigned long idx)                                         { return mParameters[idx]->changed(); }
    bool changed(const char *name)                                          { return changed(getIdx(name)); }
    
private:
    
    // Error handling
    
    template <typename T>
    void handleError(SetError error, long idx, T arg) const
    {
        if (error && mReportErrors)
        {
            switch (error)
            {
                case kUnknownArgument:
                    mErrorReporter.reportError(kErrorParameter, mProxy, "argument # out of range", idx + 1);
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
    
    // Utility
    
    void addParameter(unsigned long index, Parameter *parameter)
    {
        assert((index == mParameters.size()) && "parameters must be added in order");
        mParameters.add(parameter);
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
    
    FrameLib_OwnedList<Parameter> mParameters;
    Info *mParameterInfo;
};

#endif

