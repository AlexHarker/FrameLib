
#ifndef FRAMELIB_ATTRIBUTES_H
#define FRAMELIB_ATTRIBUTES_H

#include "FrameLib_Types.h"
#include "FrameLib_Memory.h"
#include <vector>
#include <cstring>
#include <cassert>
#include <limits>

// FrameLib_Attributes

// This class deals with attributes of an object

// FIX - add ability for constructor settings of array to resize the array
// FIX - allow setting of default on enums differently?
// FIX - consider various formatting possibilities for attribs
// FIX - allow attribute copying - consider going back to objects...
// FIX - add error reporting
// FIX - instantiation only attributes (with error checking - simply a marker)?
// FIx - consider adding descriptions (using const char * strings)

class FrameLib_Attributes
{

public:
    
    enum Type {kBool, kDouble, kEnum, kString, kArray, kVariableArray };
    
public:
    
    class Serial
    {
        
    public:
        
        static const size_t alignment = sizeof(double);
        static const size_t minGrowSize = 512;
        
    private:
        
        enum DataType { kDoubleArray, kString };
        
    public:
        
        Serial(BytePointer ptr, size_t size);
        Serial(size_t size);
        Serial();

        ~Serial();
        
        static size_t calcSize(Serial *serialised)          { return serialised != NULL ? serialised->mSize : 0; }
        static size_t calcSize(const char *tag, char *str)  { return sizeType() + sizeString(tag) + sizeString(str); }
        static size_t calcSize(const char *tag, size_t N)   { return sizeType() + sizeString(tag) + sizeArray(N); }
        
        void write(Serial *serialised);
        void write(const char *tag, char *str);
        void write(const char *tag, double *values, size_t N);
        
        void read(FrameLib_Attributes *attributes);
        
        size_t size()   { return mSize; }
        void clear()    { mSize = 0; }

    private:
        
        void alignmentChecks();
        
        static size_t align(size_t size)            { return (size + (alignment - 1)) & ~(alignment - 1); }
        static size_t sizeType()                    { return align(sizeof(DataType)); }
        static size_t sizeString(const char *str)   { return align(sizeof(size_t)) + align(strlen(str) + 1); }
        static size_t sizeArray(size_t N)           { return align(sizeof(size_t)) + align((N * sizeof(double))); }

        void writeType(DataType type);
        void writeSize(size_t size);
        void writeString(const char *str);
        void writeDoubles(double *ptr, size_t N);
        
        DataType readType(BytePointer *readPtr);
        void readSize(BytePointer *readPtr, size_t *size);
        void readDoubles(BytePointer *readPtr, double **values, size_t *N);
        void readString(BytePointer *readPtr, char **str);
        
        bool checkSize(size_t writeSize);
                
        BytePointer mPtr;
        size_t mSize;
        size_t mMaxSize;
        bool mOwner;
    };

    // ************************************************************************************** //

    class Attribute
    {
        
        // ************************************************************************************** //

    private:
        /*
        class Enum
        {
            
        public:
            
            Enum() : mValue(0) {}
            
            ~Enum()
            {
                for (unsigned long i = 0; i < mItems.size(); i++)
                    free(mItems[i]);
            }
            
            void addItem(const char *str)                   { mItems.push_back(strdup(str)); }
            
            unsigned long size()                            { return mItems.size(); }

            unsigned long getValue()                        { return mValue; }
            const char *getValueAsString()                  { return mItems[mValue]; }
            const char *getItemString(unsigned long item)   { return mItems[item]; }

            void setValue(unsigned long value)              { mValue = ((value >= mItems.size()) ? (mItems.size() - 1) : value); }
            
            void setValueFromString(const char *str)
            {
                for (unsigned long i = 0; i < mItems.size(); i++)
                {
                    if (strcmp(str, mItems[i]) == 0)
                    {
                        mValue = i;
                        return;
                    }
                }
            }
            
        private:
            
            std::vector <char *> mItems;
            unsigned long mValue;
        };

         // ************************************************************************************** //

        template <class T> class Array
        {
            enum clipMode {kNone, kMin, kMax, kClip};
            
        public:
            
            Array(T defaultValue, size_t maxSize, size_t size, bool variableSize) : mItems(new T[maxSize]), mMaxSize(maxSize), mVariableSize(variableSize)
            {
                mSize = size < mMaxSize ? size : mMaxSize;
                size = variableSize ? size : maxSize;
                mMode = kNone;
                
                for (size_t i = 0; i < mSize; i++)
                    mItems[i] = defaultValue;
                
                mDefaultValue = defaultValue;
            }
            
            ~Array()
            {
                delete[] mItems;
            }

            size_t size()
            {
                return mSize;
            }
            
            void setMin(T min)
            {
                mMode = kMin;
                mMin = min;
            }
            
            void setMax(T max)
            {
                mMode = kMax;
                mMax = max;
            }
            
            void setClip(T min, T max)
            {
                mMode = kClip;
                mMin = min;
                mMax = max;
            }
            
            void getRange(double *min, double *max)
            {
                *min = mMin;
                *max = mMax;
            }
            
            void set(T *values, size_t size)
            {
                size = size > mMaxSize ? mMaxSize : size;

                switch(mMode)
                {
                    case kNone:
                        for (size_t i = 0; i < size; i++)
                            mItems[i] = values[i];
                        break;
                    case kMin:
                        for (size_t i = 0; i < size; i++)
                            mItems[i] = values[i] < mMin ? mMin : values[i];
                        break;
                    case kMax:
                        for (size_t i = 0; i < size; i++)
                            mItems[i] = values[i] > mMax ? mMax : values[i];
                        break;
                    case kClip:
                        for (size_t i = 0; i < size; i++)
                            mItems[i] = values[i] < mMin ? mMin : (values[i] > mMax ? mMax : values[i]);
                        break;
                }

                if (!mVariableSize)
                    for (size_t i = size; i < mMaxSize; i++)
                        mItems[i] = mDefaultValue;
                else
                    mSize = size;
            }
            
            T *get()
            {
                return mItems;
            }
            
        private:
            
            clipMode mMode;
            
            T mDefaultValue;
            T mMin;
            T mMax;
            
            T *mItems;
            size_t mSize;
            
            const size_t mMaxSize;
            const bool mVariableSize;
        };
        
        // ************************************************************************************** //

         class String
        {
            const static size_t maxLen = 128;
            
        public:
            
            String() { mCString[0] = 0; }
            
            void set(const char *str)
            {
                size_t i = 0;
                
                if (str != NULL)
                {
                    for (i = 0; i < maxLen; i++)
                        if ((mCString[i] = str[i]) == 0)
                            break;                    
                }
                
                mCString[i] = 0;
            }
            
            const char *get() const     { return mCString; }
            
        private:
            
            char mCString[maxLen + 1];
        };
        // ************************************************************************************** //

    public:
        Attribute(Type type, const char *name, long argumentIdx, double defaultValue = 0.0, size_t maxSize = 1, size_t size = 1)
        {
            mType = type;
            mName = strdup(name);
            mArgumentIdx = argumentIdx;
            mChanged = FALSE;
            
            assert(size <= maxSize && "attributes cannot have a default size greater than the maximum size");
            
            switch(type)
            {
                case kBool:
                case kDouble:
                    assert(maxSize == 1 && "non-array attributes do not support sizes greater than 1");
                    mValue = defaultValue;
                    break;
                case kString:
                    assert(maxSize == 1 && "non-array attributes do not support sizes greater than 1");
                    mString = new String();
                    break;
                case kEnum:
                    assert(maxSize == 1 && "non-array attributes do not support sizes greater than 1");
                    mEnum = new Enum();
                    break;
                case kArray:
                    mArray = new Array <double> (defaultValue, maxSize, maxSize, FALSE);
                    break;
                case kVariableArray:
                    mArray = new Array <double> (defaultValue, maxSize, size, TRUE);
                    break;
            }
            
            mMin = -std::numeric_limits<double>::infinity();
            mMax = std::numeric_limits<double>::infinity();
        }
        
        ~Attribute()
        {
            free(mName);
            
            if (mType == kString)
                delete mString;
            if (mType == kEnum)
                delete mEnum;
            if (mType == kArray || mType == kVariableArray)
                delete mArray;
        }
        
        Type type()
        {
            return mType;
        }
        
        const char *name()
        {
            return mName;
        }
        
        long argumentIdx()
        {
            return mArgumentIdx;
        }
        
        void addEnumItem(const char *str)
        {
            assert(mType == kEnum && "cannot add enum items to non-enum attribute");
            mEnum->addItem(str);
        }
        
        const char *getEnumItemString(unsigned long item)
        {
            assert(mType == kEnum && "cannot get enum string for non-enum attribute");
            return mEnum->getItemString(item);
        }
        
        void setMin(double min)
        {
            switch (mType)
            {
                case kDouble:
                    mMin = min;
                    mMax = std::numeric_limits<double>::infinity();
                    break;
                case kArray:
                case kVariableArray:
                    mArray->setMin(min);
                    break;
                default:
                    assert(0 && "attribute type does not support minimum values");
            }
        }
        
        void setMax(double max)
        {
            switch (mType)
            {
                case kDouble:
                    mMin = -std::numeric_limits<double>::infinity();
                    mMax = max;
                    break;
                case kArray:
                case kVariableArray:
                    mArray->setMax(max);
                    break;
                default:
                    assert(0 && "attribute type does not support maximum values");
            }
        }
        
        void setClip(double min, double max)
        {
            switch (mType)
            {
                case kDouble:
                    mMin = min;
                    mMax = max;
                    break;
                case kArray:
                case kVariableArray:
                    mArray->setClip(min, max);
                    break;
                default:
                    assert(0 && "attribute type does not support clipping values");
            }
        }
        
        void getRange(double *min, double *max)
        {
            switch (mType)
            {
                case kBool:
                    *min = FALSE;
                    *max = TRUE;
                    break;
                case kDouble:
                    *min = mMin;
                    *max = mMax;
                    break;
                case kString:
                    *min = 0;
                    *max = 0;
                    break;
                case kEnum:
                    *min = 0;
                    *max = mEnum->size() - 1;
                    break;
                case kArray:
                case kVariableArray:
                    mArray->getRange(min, max);
            }
        }
        
        void set(const char *str)
        {
            switch (mType)
            {
                case kString:
                    mString->set(str);
                    mChanged = TRUE;
                    break;
                case kEnum:
                    mEnum->setValueFromString(str);
                    mChanged = TRUE;
                    break;
                default:
                    break;
            }
        }
        
        void set(double *values, size_t size)
        {
            switch (mType)
            {
                case kBool:
                    mValue = *values;
                    mChanged = TRUE;
                    break;
                case kDouble:
                    mValue = (*values < mMin) ? mMin : ((*values > mMax) ? mMax : *values);
                    mChanged = TRUE;
                    break;
                case kString:
                    break;
                case kEnum:
                    mEnum->setValue(*values < 0 ? 0 : *values);
                    mChanged = TRUE;
                    break;
                case kArray:
                case kVariableArray:
                    mArray->set(values, size);
                    mChanged = TRUE;
            }
        }
        
        double getValue()
        {
            switch (mType)
            {
                case kBool:
                case kDouble:
                    return mValue;
                case kEnum:
                    return mEnum->getValue();
                default:
                    return 0.0;
            }
        }
        
        const char *getString()
        {
            switch (mType)
            {
                case kString:
                    return mString->get();
                case kEnum:
                    return mEnum->getValueAsString();
                    break;
                default:
                    return NULL;
            }
        }
        
        double *getArray()
        {
            switch (mType)
            {
                case kArray:
                case kVariableArray:
                    return mArray->get();
                default:
                    return NULL;
            }

        }
        
        double *getArray(size_t *size)
        {
            *size = getArraySize();
            return getArray();
        }
        
        size_t getArraySize() const
        {
            switch (mType)
            {
                case kArray:
                case kVariableArray:
                    return mArray->size();
                default:
                    return 0;
            }
        }
        
        bool changed()
        {
            bool result = mChanged;
            mChanged = FALSE;
            return result;
        }
        
    private:
        
        Type mType;
        char *mName;
        long mArgumentIdx;
        bool mChanged;
        
        union
        {
            double mValue;
            String *mString;
            Enum *mEnum;
            Array <double> *mArray;
        };
        
        double mMin;
        double mMax;
    };
         */
        
    public:
        
        Attribute(const char *name, long argumentIdx) : mChanged(0)
        {
            mName = strdup(name);
            mArgumentIdx = argumentIdx;
        }
        
        virtual ~Attribute()
        {
            free(mName);
        }
        
        virtual Type type() = 0;
        
        const char *name()  { return mName; }
        long argumentIdx()  { return mArgumentIdx; }
        
        virtual void addEnumItem(const char *str)
        {
            assert(0 && "cannot add enum items to non-enum attribute");
        }
        
        virtual const char *getEnumItemString(unsigned long item) const
        {
            assert(0 == kEnum && "cannot get enum string for non-enum attribute");
        }
        
        virtual void setMin(double min)
        {
            assert(0 && "attribute type does not support minimum values");
        }
        
        virtual void setMax(double max)
        {
           assert(0 && "attribute type does not support maximum values");
        }
        
        virtual void setClip(double min, double max)
        {
            assert(0 && "attribute type does not support clipping values");
        }
        
        virtual  void getRange(double *min, double *max)
        {
            *min = 0;
            *max = 0;
        }
        
        virtual void set(const char *str)
        {
            assert(0 && "attribute type does not support setting by string");
        }
        
        virtual void set(double value)
        {
            assert(0 && "attribute type does not support setting by double");
        }
        
        virtual void set(double *values, size_t size)
        {
            assert(0 && "attribute type does not support setting by array");
        }
        
        virtual double getValue() const             { return 0; }
        virtual const char *getString() const       { return NULL; }
        virtual size_t getArraySize() const         { return 0; }
        virtual const double *getArray() const      { return NULL; }
        
        const double *getArray(size_t *size) const
        {
            *size = getArraySize();
            return getArray();
        }
        
        bool changed()
        {
            bool result = mChanged;
            mChanged = FALSE;
            return result;
        }
        
    protected:
        bool mChanged;

    private:
        
        char *mName;
        long mArgumentIdx;
    };
    
    class String : public Attribute
    {
        const static size_t maxLen = 128;
        
    public:
        
        String(const char *name, long argumentIdx) : Attribute(name, argumentIdx)
        {
            mCString[0] = 0;
        }
        
        virtual Type type() { return kString; }

        virtual void set(const char *str)
        {
            size_t i = 0;
            
            if (str != NULL)
            {
                for (i = 0; i < maxLen; i++)
                    if ((mCString[i] = str[i]) == 0)
                        break;
            }
            
            mCString[i] = 0;
            mChanged = true;
        }
        
        virtual const char *getString() const     { return mCString; }
        
    private:
        
        char mCString[maxLen + 1];
    };
    
    class Enum : public Attribute
    {
        
    public:
        
        virtual Type type() { return kEnum; }

        Enum(const char *name, long argumentIdx) : Attribute(name, argumentIdx), mValue(0) {}
        
        ~Enum()
        {
            for (unsigned long i = 0; i < mItems.size(); i++)
                free(mItems[i]);
        }
        
        void addEnumItem(const char *str)                                   { mItems.push_back(strdup(str)); }
        
        unsigned long size() const                                          { return mItems.size(); }
        
        virtual double getValue() const                                     { return mValue; }
        virtual const char *getString() const                               { return mItems[mValue]; }
        virtual const char *getEnumItemString(unsigned long item) const     { return mItems[item]; }
        // FIX - review type and implications here...
        virtual void set(double value)
        {
            mValue = ((value >= mItems.size()) ? (mItems.size() - 1) : value);
            mChanged = true;
        }
        
        virtual void set(const char *str)
        {
            for (unsigned long i = 0; i < mItems.size(); i++)
            {
                if (strcmp(str, mItems[i]) == 0)
                {
                    mValue = i;
                    mChanged = true;
                    return;
                }
            }
        }
        
        virtual  void getRange(double *min, double *max)
        {
            *min = 0;
            *max = size() - 1;
        }
        
    private:
        
        std::vector <char *> mItems;
        unsigned long mValue;
    };
    
    class Bool : public Attribute
    {
        
    public:
        
        virtual Type type() { return kBool; }
        
        Bool(const char *name, long argumentIdx, bool defaultValue) : Attribute(name, argumentIdx), mValue(defaultValue) {}
        
        virtual double getValue() const                { return mValue; }
        
        virtual void set(double value)
        {
            mValue = value ? true : false;
            mChanged = true;
        }
        
        virtual void getRange(double *min, double *max)
        {
            *min = false;
            *max = true;
        }
        
    private:
        
        bool mValue;
    };
    
    class Double : public Attribute
    {
        
    public:
        
        virtual Type type() { return kDouble; }
        
        Double(const char *name, long argumentIdx, double defaultValue) : Attribute(name, argumentIdx), mValue(defaultValue)
        {
            mMin = -std::numeric_limits<double>::infinity();
            mMax = std::numeric_limits<double>::infinity();
        }
        
        virtual double getValue() const                    { return mValue; }
        
        virtual void set(double value)
        {
            mValue = (value < mMin) ? mMin : ((value > mMax) ? mMax : value);
            mChanged = true;
        }
        
        virtual  void getRange(double *min, double *max)
        {
            *min = mMin;
            *max = mMax;
        }
        
        virtual void setMin(double min)
        {
            mMin = min;
        }
        
        virtual void setMax(double max)
        {
            mMax = max;
        }
        
        virtual void setClip(double min, double max)
        {
            setMin(min);
            setMax(max);
        }

    private:
        
        double mValue;
        double mMin;
        double mMax;
    };

    class Array : public Attribute, private std::vector<double>
    {
        enum clipMode {kNone, kMin, kMax, kClip};
        
    public:
        
        Array(const char *name, long argumentIdx, double defaultValue, size_t size)
        : Attribute(name, argumentIdx), mMode(kNone), mDefaultValue(defaultValue), mSize(size), mVariableSize(false)
        {
            mItems.resize(size);
            
            for (size_t i = 0; i < mSize; i++)
                mItems[i] = defaultValue;
        }
        
        Array(const char *name, long argumentIdx, double defaultValue, size_t maxSize, size_t size)
        : Attribute(name, argumentIdx), mMode(kNone), mDefaultValue(defaultValue), mVariableSize(true)
        {
            mItems.resize(maxSize);

            mSize = size < maxSize ? size : maxSize;
            
            for (size_t i = 0; i < mSize; i++)
                mItems[i] = defaultValue;
        }

        
        virtual Type type() { return mVariableSize ? kVariableArray : kArray; }

        virtual void setMin(double min)
        {
            mMode = kMin;
            mMin = min;
        }
        
        virtual void setMax(double max)
        {
            mMode = kMax;
            mMax = max;
        }
        
        virtual void setClip(double min, double max)
        {
            mMode = kClip;
            mMin = min;
            mMax = max;
        }
        
        void getRange(double *min, double *max)
        {
            *min = mMin;
            *max = mMax;
        }
        
        void set(double *values, size_t size)
        {
            size = size > mItems.size() ? mItems.size() : size;
            
            switch(mMode)
            {
                case kNone:
                    for (size_t i = 0; i < size; i++)
                        mItems[i] = values[i];
                    break;
                case kMin:
                    for (size_t i = 0; i < size; i++)
                        mItems[i] = values[i] < mMin ? mMin : values[i];
                    break;
                case kMax:
                    for (size_t i = 0; i < size; i++)
                        mItems[i] = values[i] > mMax ? mMax : values[i];
                    break;
                case kClip:
                    for (size_t i = 0; i < size; i++)
                        mItems[i] = values[i] < mMin ? mMin : (values[i] > mMax ? mMax : values[i]);
                    break;
            }
            
            if (!mVariableSize)
                for (size_t i = size; i < mItems.size(); i++)
                    mItems[i] = mDefaultValue;
            else
                mSize = size;
            
            mChanged = true;
        }
        
        virtual size_t getArraySize() const
        {
            return mSize;
        }
        
        virtual const double * getArray() const
        {
            return &mItems[0];
        }
        
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
    
    ~FrameLib_Attributes()
    {
        for (std::vector <Attribute *>::iterator it = mAttributes.begin(); it != mAttributes.end(); it++)
            delete *it;
    }
    
private:
    
    void addAttribute(unsigned long index, Attribute *attr)
    {
        assert((index == mAttributes.size()) && "attributes must be added in order");
        mAttributes.push_back(attr);
    }
    
public:
    
    void addBool(unsigned long index, const char *name, bool defaultValue = FALSE, long argumentIdx = -1)
    {
        addAttribute(index, new Bool(name, argumentIdx, defaultValue));
    }
    
    void addDouble(unsigned long index, const char *name, double defaultValue = 0.0, long argumentIdx = -1)
    {
        addAttribute(index, new Double(name, argumentIdx, defaultValue));
    }
    
    void addString(unsigned long index, const char *name, const char *str, long argumentIdx = -1)
    {
        addAttribute(index, new String (name, argumentIdx));
        mAttributes.back()->set(str);
    }
    
    void addEnum(unsigned long index, const char *name, long argumentIdx = -1)
    {
       addAttribute(index, new Enum(name, argumentIdx));
    }
    
    void addEnumItem(unsigned long index, const char *str)
    {
        mAttributes.back()->addEnumItem(str);
    }
    
    void addArray(unsigned long index, const char *name, double defaultValue, size_t size, long argumentIdx = -1)
    {
        addAttribute(index, new Array(name, argumentIdx, defaultValue, size));
    }
    
    void addVariableArray(unsigned long index, const char *name, double defaultValue, size_t maxSize, size_t size, long argumentIdx = -1)
    {
        addAttribute(index, new Array(name, argumentIdx, defaultValue, maxSize, size));
    }
    
    /////////////////////////////////////////////////////////////////////////////

private:
    
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
    
public:
    
    unsigned long size() const
    {
        return mAttributes.size();
    }
    
    long getIdx(const char *name) const
    {
        for (unsigned long i = 0; i < mAttributes.size(); i++)
            if (strcmp(name, mAttributes[i]->name()) == 0)
                return i;
        
        long argumentIdx = convertToNumber(name);
        
        if (argumentIdx >= 0)
            for (unsigned long i = 0; i < mAttributes.size(); i++)
                if (argumentIdx == mAttributes[i]->argumentIdx())
                    return i;
        
        return -1;
    }
    
    Type getType(unsigned long idx) const
    {
        return mAttributes[idx]->type();
    }
    
    Type getType(const char *name) const
    {
        return getType(getIdx(name));
    }

    void getRange(unsigned long idx, double *min, double *max) const
    {
        return mAttributes[idx]->getRange(min, max);
    }
    
    void getRange(const char *name, double *min, double *max) const
    {
        return getRange(getIdx(name), min, max);
    }
    
    const char *getEnumItemString(unsigned long idx, unsigned long item) const
    {
        return mAttributes[idx]->getEnumItemString(item);
    }
    
    const char *getEnumItemString(const char *name, unsigned long item) const
    {
        return getEnumItemString(getIdx(name), item);
    }
    
    /////////////////////////////////////////////////////////////////////////////
    

    
public:
   
    // Range
    
    void setMin(double min)                     { mAttributes.back()->setMin(min); }
    void setMax(double max)                     { mAttributes.back()->setMax(max); }
    void setClip(double min, double max)        { mAttributes.back()->setClip(min, max); }
   
    // Setters (N.B. - setters have sanity checks as the tags are set by the end-user)

    void set(Serial *serialised)                { serialised->read(this); }

    void set(unsigned long idx, bool value)     { set(idx, (double) value); }
    void set(const char *name, bool value)      { set(name, (double) value); }
    
    void set(unsigned long idx, long value)     { set(idx, (double) value); }
    void set(const char *name, long value)      { set(name, (double) value); }

    void set(unsigned long idx, double value)   { set(idx, &value, 1); }
    void set(const char *name, double value)    { set(name, &value, 1); }

    void set(unsigned long idx, char *str)      { mAttributes[idx]->set(str); }
    
    void set(const char *name, char *str)
    {
        long idx = getIdx(name);
        
        if (idx >= 0)
            set(idx, str);
    }
    
    void set(unsigned long idx, double *values, size_t size)
    {
        mAttributes[idx]->set(values, size);
    }
    
    void set(const char *name, double *values, size_t size)
    {
        long idx = getIdx(name);
        
        if (idx >= 0)
            set(idx, values, size);
    }

    // Getters (N.B. - getters have no sanity checks, because they are the programmer's responsibility)
    
    double getValue(unsigned long idx) const                        { return mAttributes[idx]->getValue(); }
    double getValue(const char *name) const                         { return getValue(getIdx(name)); }
    
    long getInt(unsigned long idx) const                            { return (long) getValue(idx); }
    long getInt(const char *name) const                             { return getInt(getIdx(name)); }
    
    long getBool(unsigned long idx) const                           { return (bool) getValue(idx); }
    bool getBool(const char *name) const                            { return (bool) getValue(getIdx(name)); }
    
    const char *getString(unsigned long idx) const                  { return mAttributes[idx]->getString(); }
    const char *getString(const char *name) const                   { return getString(getIdx(name)); }
    
    const double *getArray(unsigned long idx) const                 { return mAttributes[idx]->getArray(); }
    const double *getArray(const char *name) const                  { return getArray(getIdx(name)); }
    const double *getArray(unsigned long idx, size_t *size) const   { return mAttributes[idx]->getArray(size); }
    const double *getArray(const char *name, size_t *size) const    { return getArray(getIdx(name), size); }
    
    size_t getArraySize(unsigned long idx) const                    { return mAttributes[idx]->getArraySize(); }
    size_t getArraySize(const char *name) const                     { return getArraySize(getIdx(name)); }
    
    bool changed(unsigned long idx)                                 { return mAttributes[idx]->changed(); }
    bool changed(const char *name)                                  { return changed(getIdx(name)); }
    
private:
    
    std::vector <Attribute *> mAttributes;
};

#endif

