
#ifndef FRAMELIB_PARAMETERS_H
#define FRAMELIB_PARAMETERS_H

#include "FrameLib_Types.h"
#include "FrameLib_Memory.h"
#include <vector>
#include <cstring>
#include <cassert>
#include <limits>
#include <string>

// FrameLib_Parameters

// This class deals with parameters of an object

// FIX - add error reporting
// FIX - instantiation only parameters (with error checking - simply a marker)?
// FIX - consider adding descriptions (using const char * strings)

class FrameLib_Parameters
{

public:
    
    enum Type {kBool, kDouble, kEnum, kString, kArray, kVariableArray };
    
public:
    
    // A Serialised Set Of Tagged Parameter Values (no memory ownership)
    
    class Serial
    {
        
    public:
        
        // N.B. the assumption is that double is the largest type in use
        
        static const size_t alignment = sizeof(double);
        static const size_t minGrowSize = 512;
        
    private:
        
        enum DataType { kDoubleArray, kString };
        
    public:
        
        // Constructors and Destructor
        
        Serial(BytePointer ptr, size_t size);
        Serial();
        ~Serial() {};
        
        // Size Calculations
        
        static size_t calcSize(Serial *serialised)          { return serialised != NULL ? serialised->mSize : 0; }
        static size_t calcSize(const char *tag, char *str)  { return sizeType() + sizeString(tag) + sizeString(str); }
        static size_t calcSize(const char *tag, size_t N)   { return sizeType() + sizeString(tag) + sizeArray(N); }
        
        // Write Items
        
        void write(Serial *serialised);
        void write(const char *tag, char *str);
        void write(const char *tag, double *values, size_t N);
        
        // Read into Parameters
        
        void read(FrameLib_Parameters *parameters) const;
        
        // Utility
        
        size_t size() const     { return mSize; }
        void clear()            { mSize = 0; }
        
        static size_t alignSize(size_t size)                    { return (size + (alignment - 1)) & ~(alignment - 1); }
        static size_t inPlaceSize(size_t size)                  { return alignSize(sizeof(Serial)) + alignSize(size); }

        static Serial *newInPlace(void *ptr, size_t size)   { return new (ptr) Serial(((BytePointer) ptr) + alignSize(sizeof(Serial)), size); }

    protected:
        
        // Check Size
        
        bool checkSize(size_t writeSize);
        
    private:
        
        // Deleted
        
        Serial(const Serial&);
        Serial& operator=(const Serial&);
        
        // Debug
        
        void alignmentChecks() const;
        
        // Size Calculators
        
        static size_t sizeType()                    { return alignSize(sizeof(DataType)); }
        static size_t sizeString(const char *str)   { return alignSize(sizeof(size_t)) + alignSize(strlen(str) + 1); }
        static size_t sizeArray(size_t N)           { return alignSize(sizeof(size_t)) + alignSize((N * sizeof(double))); }

        // Write Item
        
        void writeType(DataType type);
        void writeSize(size_t size);
        void writeString(const char *str);
        void writeDoubles(double *ptr, size_t N);
        
        // Read Item
        
        DataType readType(BytePointer *readPtr) const;
        void readSize(BytePointer *readPtr, size_t *size) const;
        void readDoubles(BytePointer *readPtr, double **values, size_t *N) const;
        void readString(BytePointer *readPtr, char **str) const;
        
    protected:
        
        // Member Variables
        
        BytePointer mPtr;
        size_t mSize;
        size_t mMaxSize;
    };

    // Extends Serial (with memory ownership)
    
    class AutoSerial : public Serial
    {
        
    public:

        AutoSerial() {};
        AutoSerial(size_t size) : Serial(new Byte[size], size) {}
        ~AutoSerial() { if (mPtr) delete[] mPtr; }
        
        // Write Items
        
        void write(Serial *serialised)                          { if (checkSize(calcSize(serialised))) Serial::write(serialised); }
        void write(const char *tag, char *str)                  { if (checkSize(calcSize(tag, str))) Serial::write(tag, str); }
        void write(const char *tag, double *values, size_t N)   { if (checkSize(calcSize(tag, N))) Serial::write(tag, values, N); }
        
    private:
        
        bool checkSize(size_t writeSize);
    };
    
    // ************************************************************************************** //

private:
    
    // Abstract Parameter Class
    
    class Parameter
    {
        
    public:
      
        // Constructor / Destructor
        
        Parameter(const char *name, long argumentIdx);
        virtual ~Parameter() {};
       
        // Setters
        
        virtual void addEnumItem(const char *str);
        
        virtual void setMin(double min);
        virtual void setMax(double max);
        virtual void setClip(double min, double max);
        
        virtual void set(const char *str) {}
        virtual void set(double value) {}
        virtual void set(double *values, size_t size);

        virtual void clear() = 0;
        
        // Getters
        
        // Setup
        
        virtual Type type() = 0;
        
        const char *name()  { return mName.c_str(); }
        long argumentIdx()  { return mArgumentIdx; }
        
        virtual void getRange(double *min, double *max);
        virtual const char *getItemString(unsigned long item) const;

        // Values

        virtual double getValue() const             { return 0; }
        virtual const char *getString() const       { return NULL; }
        virtual size_t getArraySize() const         { return 0; }
        virtual const double *getArray() const      { return NULL; }
        const double *getArray(size_t *size) const;
        
        bool changed();
        
    protected:
        
        bool mChanged;

    private:
        
        std::string mName;
        long mArgumentIdx;
    };
    
    // ************************************************************************************** //
    
    // Bool Parameter Class

    class Bool : public Parameter
    {
        
    public:
        
        Bool(const char *name, long argumentIdx, bool defaultValue)
        : Parameter(name, argumentIdx), mValue(defaultValue), mDefault(defaultValue) {}

        // Setters
        
        virtual void set(double value);
        virtual void set(double *values, size_t size);

        virtual void clear() { Bool::set(mDefault); };

        // Getters

        virtual Type type() { return kBool; }

        virtual void getRange(double *min, double *max);

        virtual double getValue() const { return mValue; }
        
    private:
        
        bool mValue;
        bool mDefault;
    };
    
    // ************************************************************************************** //

    // Enum Parameter Class

    class Enum : public Parameter
    {
        
    public:
        
        Enum(const char *name, long argumentIdx)
        : Parameter(name, argumentIdx), mValue(0) {}
        
        // Setters
        
        void addEnumItem(const char *str) { mItems.push_back(str); }
        
        virtual void set(double value);
        virtual void set(double *values, size_t size);
        virtual void set(const char *str);
        
        virtual void clear() { Enum::set(0.0); };

        virtual Type type() { return kEnum; }
        
        // Getters
        
        virtual void getRange(double *min, double *max);
        
        virtual double getValue() const                                 { return mValue; }
        virtual const char *getString() const                           { return mItems[mValue].c_str(); }
        virtual const char *getItemString(unsigned long item) const     { return mItems[item].c_str(); }
        
    private:
        
        std::vector <std::string> mItems;
        unsigned long mValue;
    };
    
    // ************************************************************************************** //

    // Double Parameter Class

    class Double : public Parameter
    {
        
    public:
        
        Double(const char *name, long argumentIdx, double defaultValue)
        : Parameter(name, argumentIdx), mValue(defaultValue), mDefault(defaultValue), mMin(-std::numeric_limits<double>::infinity()), mMax(std::numeric_limits<double>::infinity()) {}
        
        // Setters

        virtual void setMin(double min);
        virtual void setMax(double max);
        virtual void setClip(double min, double max);

        virtual void set(double value);
        virtual void set(double *values, size_t size);
        
        virtual void clear() { Double::set(mDefault); };

        // Getters

        virtual Type type() { return kDouble; }

        virtual void getRange(double *min, double *max);
        
        virtual double getValue() const { return mValue; }
       
    private:
        
        double mValue;
        double mDefault;
        double mMin;
        double mMax;
    };
    
    // ************************************************************************************** //

    // String Parameter Class

    class String : public Parameter
    {
        const static size_t maxLen = 128;
        
    public:
        
        String(const char *name, const char *str, long argumentIdx);
        
        // Setters
        
        virtual void set(const char *str);
        
        virtual void clear() { String::set(NULL); };

        // Getters
        
        virtual Type type() { return kString; }
        
        virtual const char *getString() const { return mCString; }
        
    private:
        
        char mCString[maxLen + 1];
    };

    // ************************************************************************************** //

    // Array Parameter Class

    class Array : public Parameter, private std::vector<double>
    {
        enum clipMode {kNone, kMin, kMax, kClip};
        
    public:
        
        Array(const char *name, long argumentIdx, double defaultValue, size_t size);
        Array(const char *name, long argumentIdx, double defaultValue, size_t maxSize, size_t size);

        // Setters

        virtual void setMin(double min);
        virtual void setMax(double max);
        virtual void setClip(double min, double max);

        virtual void set(double *values, size_t size);

        virtual void clear() { Array::set(NULL, 0); };

        // Getters

        virtual Type type() { return mVariableSize ? kVariableArray : kArray; }
        
        virtual void getRange(double *min, double *max);
        
        virtual size_t getArraySize() const         { return mSize; }
        virtual const double * getArray() const     { return &mItems[0]; }
        
    private:
        
        clipMode mMode;
        
        double mDefaultValue;
        double mMin;
        double mMax;
        
        std::vector<double> mItems;
        size_t mSize;
        
        const bool mVariableSize;
    };
    
    // ************************************************************************************** //

public:
    
    // Constructor
    
    FrameLib_Parameters() {}
    
    // Destructor
    
    ~FrameLib_Parameters()
    {
        for (std::vector <Parameter *>::iterator it = mParameters.begin(); it != mParameters.end(); it++)
            delete *it;
    }
    
    // Size and Index
    
    unsigned long size() const                  { return mParameters.size(); }
    
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
        
        return -1;
    }

    // Add Parameters
    
    void addBool(unsigned long index, const char *name, bool defaultValue = false, long argumentIdx = -1)
    {
        addParameter(index, new Bool(name, argumentIdx, defaultValue));
    }
    
    void addDouble(unsigned long index, const char *name, double defaultValue = 0.0, long argumentIdx = -1)
    {
        addParameter(index, new Double(name, argumentIdx, defaultValue));
    }
    
    void addString(unsigned long index, const char *name, const char *str, long argumentIdx = -1)
    {
        addParameter(index, new String(name, str, argumentIdx));
    }
    
    void addEnum(unsigned long index, const char *name, long argumentIdx = -1)
    {
       addParameter(index, new Enum(name, argumentIdx));
    }
    
    void addEnumItem(unsigned long index, const char *str)
    {
        mParameters.back()->addEnumItem(str);
    }
    
    void addArray(unsigned long index, const char *name, double defaultValue, size_t size, long argumentIdx = -1)
    {
        addParameter(index, new Array(name, argumentIdx, defaultValue, size));
    }
    
    void addVariableArray(unsigned long index, const char *name, double defaultValue, size_t maxSize, size_t size, long argumentIdx = -1)
    {
        addParameter(index, new Array(name, argumentIdx, defaultValue, maxSize, size));
    }

    // Setters (N.B. - setters have sanity checks as the tags are set by the end-user)

    // Set Range
    
    void setMin(double min)                     { mParameters.back()->setMin(min); }
    void setMax(double max)                     { mParameters.back()->setMax(max); }
    void setClip(double min, double max)        { mParameters.back()->setClip(min, max); }
   
    // Set Value
    
    void set(Serial *serialised)                { serialised->read(this); }

    void set(unsigned long idx, bool value)     { set(idx, (double) value); }
    void set(const char *name, bool value)      { set(name, (double) value); }
    
    void set(unsigned long idx, long value)     { set(idx, (double) value); }
    void set(const char *name, long value)      { set(name, (double) value); }

    void set(unsigned long idx, double value)   { mParameters[idx]->set(value); }

    void set(const char *name, double value)
    {
        long idx = getIdx(name);
        
        if (idx >= 0)
            set(idx, value);
    }
    
    void set(unsigned long idx, char *str)      { mParameters[idx]->set(str); }
    
    void set(const char *name, char *str)
    {
        long idx = getIdx(name);
        
        if (idx >= 0)
            set(idx, str);
    }
    
    void set(unsigned long idx, double *values, size_t size)
    {
        mParameters[idx]->set(values, size);
    }
    
    void set(const char *name, double *values, size_t size)
    {
        long idx = getIdx(name);
        
        if (idx >= 0)
            set(idx, values, size);
    }

    // Getters (N.B. - getters have no sanity checks, because they are the programmer's responsibility)
    
    // Get Type
    
    Type getType(unsigned long idx) const                                   { return mParameters[idx]->type(); }
    Type getType(const char *name) const                                    { return getType(getIdx(name)); }
    
    // Get Range
    
    void getRange(unsigned long idx, double *min, double *max) const        { return mParameters[idx]->getRange(min, max); }
    void getRange(const char *name, double *min, double *max) const         { return getRange(getIdx(name), min, max); }
    
    // Get Item Strings
    
    const char *getItemString(unsigned long idx, unsigned long item) const  { return mParameters[idx]->getItemString(item); }
    const char *getItemString(const char *name, unsigned long item) const   { return getItemString(getIdx(name), item); }
    
    // Get Value
    
    double getValue(unsigned long idx) const                        { return mParameters[idx]->getValue(); }
    double getValue(const char *name) const                         { return getValue(getIdx(name)); }
    
    long getInt(unsigned long idx) const                            { return (long) getValue(idx); }
    long getInt(const char *name) const                             { return getInt(getIdx(name)); }
    
    long getBool(unsigned long idx) const                           { return (bool) getValue(idx); }
    bool getBool(const char *name) const                            { return (bool) getValue(getIdx(name)); }
    
    const char *getString(unsigned long idx) const                  { return mParameters[idx]->getString(); }
    const char *getString(const char *name) const                   { return getString(getIdx(name)); }
    
    const double *getArray(unsigned long idx) const                 { return mParameters[idx]->getArray(); }
    const double *getArray(const char *name) const                  { return getArray(getIdx(name)); }
    const double *getArray(unsigned long idx, size_t *size) const   { return mParameters[idx]->getArray(size); }
    const double *getArray(const char *name, size_t *size) const    { return getArray(getIdx(name), size); }
    
    size_t getArraySize(unsigned long idx) const                    { return mParameters[idx]->getArraySize(); }
    size_t getArraySize(const char *name) const                     { return getArraySize(getIdx(name)); }
    
    bool changed(unsigned long idx)                                 { return mParameters[idx]->changed(); }
    bool changed(const char *name)                                  { return changed(getIdx(name)); }
    
private:
    
    // Deleted
    
    FrameLib_Parameters(const FrameLib_Parameters&);
    FrameLib_Parameters& operator=(const FrameLib_Parameters&);
    
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
    
    std::vector <Parameter *> mParameters;
};

#endif

