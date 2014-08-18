
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

// FIX - instantion only attributes (with error checking)?
// FIX - check alignment assumptions
// FIX - add checks for memory overwrite to the Serial subclass ??
// FIX - add error reporting
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
        
    private:
        
        enum DataType { kDoubleArray, kString };
        
    public:
        
        Serial(BytePointer ptr) : mPtr(ptr), mSize(0)
        {
            // Assume that alignment of a double is fine for all natural alignment needs (including this class)
            
            assert(Serial::alignment >= sizeof(DataType) && "alignment assumptions are incorrect for FrameLib_Attributes::Serial::DataType");
            assert(Serial::alignment >= sizeof(size_t) && "alignment assumptions are incorrect for FrameLib_Attributes::Serial");
            assert(Serial::alignment >= sizeof(char) && "alignment assumptions are incorrect for FrameLib_Attributes::Serial");
            assert(Serial::alignment >= sizeof(char *) && "alignment assumptions are incorrect for FrameLib_Attributes::Serial");
            assert(Serial::alignment >= FrameLib_Memory::getAlignment() && "alignment assumptions are incorrect for FrameLib_Attributes::Serial");
        }
        
    private:
        
        size_t align(size_t size)
        {
            return (size + (alignment - 1)) & ~(alignment - 1);
        }
        
        void writeType(DataType type)
        {
            * ((DataType *) (mPtr + mSize)) = type;
            mSize += align(sizeof(DataType));
        }
        
        void writeSize(size_t size)
        {
            * ((size_t *) (mPtr + mSize)) = size;
            mSize += align(sizeof(size_t));
        }
        
        void writeString(char *str)
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
            mSize += size;
        }
        
        static size_t sizeType()
        {
            return sizeof(DataType);
        }
        
        static size_t sizeString(char *str)
        {
            return sizeof(size_t) + strlen(str) + 1;
        }
        
        static size_t sizeArray(size_t N)
        {
            return sizeof(size_t) + (N * sizeof(double));
        }
        
        DataType readType(BytePointer *readPtr)
        {
            DataType type = *((DataType *) *readPtr);
            *readPtr += sizeof(DataType);
            return type;
        }
        
        void readSize(BytePointer *readPtr, size_t *size)
        {
            *size = *((size_t *) *readPtr);
            *readPtr += sizeof(size_t);
        }
        
        void readDoubles(BytePointer *readPtr, double **values, size_t *N)
        {
            readSize(readPtr, N);
            *values = ((double *) *readPtr);
            readPtr += *N * sizeof(double);
        }
        
        void readString(BytePointer *readPtr, char **str)
        {
            size_t size;
            readSize(readPtr, &size);
            *str = ((char *) *readPtr);
            readPtr += size;
        }
        
    public:
        
        void write(Serial *serialised)
        {
            memcpy(mPtr + mSize, serialised->mPtr, serialised->mSize);
            mSize += serialised->mSize;
        }
        
        void write(char *tag, char *str)
        {
            writeType(kString);
            writeString(tag);
            writeString(str);
        }
        
        void write(char *tag, double *values, size_t N)
        {
            writeType(kDoubleArray);
            writeString(tag);
            writeDoubles(values, N);
        }
        
        static size_t calcSize(Serial *serialised)
        {
            return serialised->mSize;
        }
        
        static size_t calcSize(char *tag, char *str)
        {
            return sizeType() + sizeString(tag) + sizeString(str);
        }
        
        static size_t calcSize(char *tag, size_t N)
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
    };

    class Attribute
    {
        
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
            
            void addItem(char *str)
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
            
            void setValueFromString(char *str)
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
        
        template <class T> class Array
        {
            enum clipMode {kNone, kMin, kMax, kClip};
            
        public:
            
            Array(T defaultValue, size_t maxSize, size_t size, bool variableSize) : mItems(new T[maxSize]), mMaxSize(maxSize), mVariableSize(variableSize)
            {
                mSize = size < mMaxSize ? size : mMaxSize;
                mMode = kNone;
                
                for (size_t i = 0; i < mSize; i++)
                    mItems[i] = defaultValue;
                
                mDefaultValue = defaultValue;
            }
            
            ~Array()
            {
                delete mItems;
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
                size_t i;
                
                for (i = 0; i < maxLen; i++)
                    if ((mCString[i] = str[i]) == 0)
                        break;
                
                mCString[i] = 0;
            }
            
            const char *get()
            {
                return mCString;
            }
            
        private:
            
            char mCString[maxLen + 1];
        };
        
    public:
        
        Attribute(Type type, char *name, double defaultValue = 0.0, size_t maxSize = 1, size_t size = 1)
        {
            mType = type;
            mName = strdup(name);
            mChanged = FALSE;
            
            assert(size <= maxSize && "attributes cannot have a default size greater than the maximum size");
            
            switch(type)
            {
                case kString:
                    assert(maxSize == 1 && "non-array attributes do not support sizes greater than 1");
                    mString = new String();
                    break;
                case kEnum:
                    assert(maxSize == 1 && "non-array attributes do not support sizes greater than 1");
                    mEnum = new Enum();
                    break;
                case kArray:
                    mArray = new Array <double> (defaultValue, maxSize, size, FALSE);
                    break;
                case kVariableArray:
                    mArray = new Array <double> (defaultValue, maxSize, size, TRUE);
                    break;
                default:
                    assert(maxSize == 1 && "non-array attributes do not support sizes greater than 1");
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
        
        void addEnumItem(char *str)
        {
            assert(mType == kEnum() && "cannot add enum items to non-enum attribute");
            mEnum->addItem(str);
        }
        
        const char *getEnumItemString(unsigned long item)
        {
            assert(mType == kEnum() && "cannot get enum string for non-enum attribute");
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
        
        void set(char *str)
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
        
        size_t getArraySize()
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
    
    /////////////////////////////////////////////////////////////////////////////

public:
    
    void addBool(char *name, bool value = FALSE)
    {
        mAttributes.push_back(Attribute(kBool, name, value));
    }
    
    void addDouble(char *name, double value = 0.0)
    {
        mAttributes.push_back(Attribute(kDouble, name, value));
    }
    
    void addString(char *name, char *str)
    {
        mAttributes.push_back(Attribute(kString, name, 0.0));
        mAttributes.back().set(str);
    }
    
    void addEnum(char *name)
    {
        mAttributes.push_back(Attribute(kEnum, name));
    }
    
    void addEnumItem(char *str)
    {
        mAttributes.back().addEnumItem(str);
    }
    
    void addArray(char *name, double defaultValue, size_t maxSize)
    {
        mAttributes.push_back(Attribute(kArray, name, defaultValue, maxSize));
    }
    
    void addVariableArray(char *name, double defaultValue, size_t maxSize, size_t size)
    {
        mAttributes.push_back(Attribute(kVariableArray, name, defaultValue, maxSize, size));
    }
    
    /////////////////////////////////////////////////////////////////////////////

public:
    
    void setMin(double min)
    {
        mAttributes.back().setMin(min);
    }
    
    void setMax(double max)
    {
         mAttributes.back().setMax(max);
    }
    
    void setClip(double min, double max)
    {
         mAttributes.back().setClip(min, max);
    }
    
    /////////////////////////////////////////////////////////////////////////////

public:
    
    unsigned long size()
    {
        return mAttributes.size();
    }
    
    long getIdx(const char *name)
    {
        for (unsigned long i = 0; i < mAttributes.size(); i++)
            if (strcmp(name, mAttributes[i].name()) == 0)
                return i;
        
        return -1;
    }
    
    Type getType(unsigned long idx)
    {
        return mAttributes[idx].type();
    }
    
    Type getType(const char *name)
    {
        return getType(getIdx(name));
    }

    void getRange(unsigned long idx, double *min, double *max)
    {
        return mAttributes[idx].getRange(min, max);
    }
    
    void getRange(const char *name, double *min, double *max)
    {
        return getRange(getIdx(name), min, max);
    }
    
    const char *getEnumItemString(unsigned long idx, unsigned long item)
    {
        return mAttributes[idx].getEnumItemString(item);
    }
    
    const char *getEnumItemString(const char *name, unsigned long item)
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
        mAttributes[idx].set(str);
    }
    
    void set(const char *name, char *str)
    {
        long idx = getIdx(name);
        
        if (idx >= 0)
            set(idx, str);
    }
    
    void set(unsigned long idx, double *values, size_t size)
    {
        mAttributes[idx].set(values, size);
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
    
    double getValue(unsigned long idx)
    {
        return mAttributes[idx].getValue();
    }
    
    double getValue(const char *name)
    {
        return getValue(getIdx(name));
    }
    
    long getInt(unsigned long idx)
    {
        return (long) getValue(idx);
    }
    
    long getInt(const char *name)
    {
        return getInt(getIdx(name));
    }
    
    const char *getString(unsigned long idx)
    {
        return mAttributes[idx].getString();
    }
    
    const char *getString(const char *name)
    {
        return getString(getIdx(name));
    }
    
    double *getArray(unsigned long idx)
    {
        return mAttributes[idx].getArray();
    }
    
    double *getArray(const char *name)
    {
        return getArray(getIdx(name));
    }
    
    size_t getArraySize(unsigned long idx)
    {
        return mAttributes[idx].getArraySize();
    }
    
    size_t getArraySize(const char *name)
    {
        return getArraySize(getIdx(name));
    }
    
    bool changed(unsigned long idx)
    {
        return mAttributes[idx].changed();
    }
    
    bool changed(const char *name)
    {
        return changed(getIdx(name));
    }
    
    /////////////////////////////////////////////////////////////////////////////

private:
    
    std::vector <Attribute> mAttributes;
};

#endif

