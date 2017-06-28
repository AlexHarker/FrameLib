
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
    
    enum Type {kBool, kDouble, kEnum, kString, kArray,  kVariableArray };
    
public:
    
    class Serial
    {
        
    public:
        
        static const size_t alignment = sizeof(double);
        static const size_t minGrowSize = 512;
        
    private:
        
        enum DataType { kDoubleArray, kString };
        
    public:
        
        Serial(BytePointer ptr, size_t size) : mPtr(ptr), mSize(0), mMaxSize(size), mOwner(false)
        {
            alignmentChecks();
        }
        
        Serial(size_t size) : mPtr(new Byte[size]), mSize(0), mMaxSize(size), mOwner(true)
        {
            alignmentChecks();
        }
        
        Serial() : mPtr(NULL), mSize(0), mMaxSize(0), mOwner(true)
        {
            alignmentChecks();
        }
        
        ~Serial()
        {
            if (mOwner)
                delete[] mPtr;
        }
        
    private:
        
        void alignmentChecks()
        {
            // Assume that alignment of a double is fine for all natural alignment needs (including this class)
            
            assert(Serial::alignment >= sizeof(DataType) && "alignment assumptions are incorrect for FrameLib_Attributes::Serial::DataType");
            assert(Serial::alignment >= sizeof(size_t) && "alignment assumptions are incorrect for FrameLib_Attributes::Serial");
            assert(Serial::alignment >= sizeof(char) && "alignment assumptions are incorrect for FrameLib_Attributes::Serial");
            assert(Serial::alignment >= sizeof(char *) && "alignment assumptions are incorrect for FrameLib_Attributes::Serial");
            // FIX - swapped the below inequality - check it is now correct and find an appropriate call
            //assert(Serial::alignment <= FrameLib_MainAllocator::getAlignment() && "alignment assumptions are incorrect for FrameLib_Attributes::Serial");
        }
        
        static size_t align(size_t size)
        {
            return (size + (alignment - 1)) & ~(alignment - 1);
        }
        
        void writeType(DataType type)
        {
            *((DataType *) (mPtr + mSize)) = type;
            mSize += align(sizeof(DataType));
        }
        
        void writeSize(size_t size)
        {
            * ((size_t *) (mPtr + mSize)) = size;
            mSize += align(sizeof(size_t));
        }
        
        void writeString(const char *str)
        {
            size_t N = strlen(str) + 1;
            writeSize(N);
            strcpy((char *) (mPtr + mSize), str);
            mSize += align(N);
        }
        
        // N.B. the assumption is that double is the largest type, and thus alignment is not necessary here
        
        void writeDoubles(double *ptr, size_t N)
        {
            size_t size = N * sizeof(double);
            writeSize(N);
            memcpy(mPtr + mSize, ptr, size);
            mSize += align(size);
        }
        
        static size_t sizeType()
        {
            return align(sizeof(DataType));
        }
        
        static size_t sizeString(const char *str)
        {
            return align(sizeof(size_t)) + align(strlen(str) + 1);
        }
        
        static size_t sizeArray(size_t N)
        {
            return align(sizeof(size_t)) + align((N * sizeof(double)));
        }
        
        DataType readType(BytePointer *readPtr)
        {
            DataType type = *((DataType *) *readPtr);
            *readPtr += align(sizeof(DataType));
            return type;
        }
        
        void readSize(BytePointer *readPtr, size_t *size)
        {
            *size = *((size_t *) *readPtr);
            *readPtr += align(sizeof(size_t));
        }
        
        void readDoubles(BytePointer *readPtr, double **values, size_t *N)
        {
            readSize(readPtr, N);
            *values = ((double *) *readPtr);
            *readPtr += align(*N * sizeof(double));
        }
        
        void readString(BytePointer *readPtr, char **str)
        {
            size_t size;
            readSize(readPtr, &size);
            *str = ((char *) *readPtr);
            *readPtr += align(size);
        }
        
        bool checkSize(size_t writeSize)
        {
            size_t growSize;
            
            if (mSize + writeSize <= mMaxSize)
                return true;
            
            if (!mOwner)
                return false;
            
            // Calculate grow size
            
            growSize = (mSize + writeSize) - mMaxSize;
            growSize = growSize < minGrowSize ? minGrowSize : growSize;
            
            // Allocate new memory and copy old data before deleting
            
            BytePointer newPtr = new Byte[mMaxSize + growSize];
            memcpy(newPtr, mPtr, mSize);
            delete[] mPtr;
            
            // Update
            
            mPtr = newPtr;
            mMaxSize += growSize;
            
            return true;
        }
        
    public:
        
        void write(Serial *serialised)
        {
            if (!serialised || !checkSize(serialised->mSize))
                return;
            
            memcpy(mPtr + mSize, serialised->mPtr, serialised->mSize);
            mSize += serialised->mSize;
        }
        
        void write(const char *tag, char *str)
        {
            if (!checkSize(calcSize(tag, str)))
                return;
            
            writeType(kString);
            writeString(tag);
            writeString(str);
        }
        
        void write(const char *tag, double *values, size_t N)
        {
            if (!N || !checkSize(calcSize(tag, N)))
                return;
            
            writeType(kDoubleArray);
            writeString(tag);
            writeDoubles(values, N);
        }
        
        static size_t calcSize(Serial *serialised)
        {
            return serialised != NULL ? serialised->mSize : 0;
        }
        
        static size_t calcSize(const char *tag, char *str)
        {
            return sizeType() + sizeString(tag) + sizeString(str);
        }
        
        static size_t calcSize(const char *tag, size_t N)
        {
            return sizeType() + sizeString(tag) + sizeArray(N);
        }
        
        void read(FrameLib_Attributes *attributes)
        {
            BytePointer readPtr = mPtr;
            char *tag, *str;
            double *values;
            size_t N;
            
            while (readPtr < mPtr + mSize)
            {
                switch (readType(&readPtr))
                {
                    case kDoubleArray:
                        readString(&readPtr, &tag);
                        readDoubles(&readPtr, &values, &N);
                        attributes->set(tag, values, N);
                        break;
                        
                    case kString:
                        readString(&readPtr, &tag);
                        readString(&readPtr, &str);
                        attributes->set(tag, str);
                        break;
                }
            }
        }
        
        size_t size()
        {
            return mSize;
        }
        
        void clear()
        {
            mSize = 0;
        }
        
    private:
        
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
        
        class Enum
        {
            
        public:
            
            Enum() : mValue(0) {}
            
            ~Enum()
            {
                for (unsigned long i = 0; i < mItems.size(); i++)
                    free(mItems[i]);
            }
            
            void addItem(const char *str)
            {
                mItems.push_back(strdup(str));
            }
            
            unsigned long size()
            {
                return mItems.size();
            }

            const char *getItemString(unsigned long item)
            {
                return mItems[item];
            }
            
            unsigned long getValue()
            {
                return mValue;
            }
            
            const char *getValueAsString()
            {
                return mItems[mValue];
            }
            
            void setValue(unsigned long value)
            {
                mValue = ((value >= mItems.size()) ? (mItems.size() - 1) : value);
            }
            
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
            
            String()
            {
                mCString[0] = 0;
            }
            
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
            
            const char *get()
            {
                return mCString;
            }
            
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
        addAttribute(index, new Attribute(kBool, name, argumentIdx, defaultValue));
    }
    
    void addDouble(unsigned long index, const char *name, double defaultValue = 0.0, long argumentIdx = -1)
    {
        addAttribute(index, new Attribute(kDouble, name, argumentIdx, defaultValue));
    }
    
    void addString(unsigned long index, const char *name, const char *str, long argumentIdx = -1)
    {
        addAttribute(index, new Attribute(kString, name, argumentIdx, 0.0));
        mAttributes.back()->set(str);
    }
    
    void addEnum(unsigned long index, const char *name, long argumentIdx = -1)
    {
       addAttribute(index, new Attribute(kEnum, name, argumentIdx));
    }
    
    void addEnumItem(unsigned long index, const char *str)
    {
        mAttributes.back()->addEnumItem(str);
    }
    
    void addArray(unsigned long index, const char *name, double defaultValue, size_t size, long argumentIdx = -1)
    {
        addAttribute(index, new Attribute(kArray, name, argumentIdx, defaultValue, size));
    }
    
    void addVariableArray(unsigned long index, const char *name, double defaultValue, size_t maxSize, size_t size, long argumentIdx = -1)
    {
        addAttribute(index, new Attribute(kVariableArray, name, argumentIdx, defaultValue, maxSize, size));
    }
    
    /////////////////////////////////////////////////////////////////////////////

public:
    
    void setMin(double min)
    {
        mAttributes.back()->setMin(min);
    }
    
    void setMax(double max)
    {
         mAttributes.back()->setMax(max);
    }
    
    void setClip(double min, double max)
    {
         mAttributes.back()->setClip(min, max);
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
    
    // N.B. Setters have sanity checks as the tags are set by the end-user
    
public:
    
    void set(Serial *serialised)
    {
        serialised->read(this);
    }
    
    void set(unsigned long idx, bool value)
    {
        double temp = value;
        set(idx, &temp, 1);
    }
    
    void set(const char *name, bool value)
    {
        double temp = value;
        set(name, &temp, 1);
    }
    
    void set(unsigned long idx, long value)
    {
        double temp = value;
        set(idx, &temp, 1);
    }
    
    void set(const char *name, long value)
    {
        double temp = value;
        set(name, &temp, 1);
    }

    void set(unsigned long idx, double value)
    {
        set(idx, &value, 1);
    }
    
    void set(const char *name, double value)
    {
        set(name, &value, 1);
    }

    void set(unsigned long idx, char *str)
    {
        mAttributes[idx]->set(str);
    }
    
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

    /////////////////////////////////////////////////////////////////////////////
    
    // N.B. - getters have no sanity checks, because they are the programmer's responsibility
    
public:
    
    double getValue(unsigned long idx) const
    {
        return mAttributes[idx]->getValue();
    }
    
    double getValue(const char *name) const
    {
        return getValue(getIdx(name));
    }
    
    long getInt(unsigned long idx) const
    {
        return (long) getValue(idx);
    }
    
    long getInt(const char *name) const
    {
        return getInt(getIdx(name));
    }
    
    long getBool(unsigned long idx) const
    {
        return (bool) getValue(idx);
    }
    
    bool getBool(const char *name) const
    {
        return (bool) getValue(getIdx(name));
    }
    
    const char *getString(unsigned long idx) const
    {
        return mAttributes[idx]->getString();
    }
    
    const char *getString(const char *name) const
    {
        return getString(getIdx(name));
    }
    
    double *getArray(unsigned long idx) const
    {
        return mAttributes[idx]->getArray();
    }
    
    double *getArray(const char *name) const
    {
        return getArray(getIdx(name));
    }
    
    double *getArray(unsigned long idx, size_t *size) const
    {
        return mAttributes[idx]->getArray(size);
    }
    
    double *getArray(const char *name, size_t *size) const
    {
        return getArray(getIdx(name), size);
    }
    
    size_t getArraySize(unsigned long idx) const
    {
        return mAttributes[idx]->getArraySize();
    }
    
    size_t getArraySize(const char *name) const
    {
        return getArraySize(getIdx(name));
    }
    
    bool changed(unsigned long idx)
    {
        return mAttributes[idx]->changed();
    }
    
    bool changed(const char *name)
    {
        return changed(getIdx(name));
    }
    
    /////////////////////////////////////////////////////////////////////////////

private:
    
    std::vector <Attribute *> mAttributes;
};

#endif

