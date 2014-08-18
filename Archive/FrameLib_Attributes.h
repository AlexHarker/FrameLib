
#ifndef FRAMELIB_ATTRIBUTES_H
#define FRAMELIB_ATTRIBUTES_H

#include "FrameLib_Types.h"
#include <vector>
#include <cstring>
#include <limits>


// FrameLib_Attributes

// This class deals with attributes of an object

class FrameLib_Attributes
{
    enum AttrType {kBool, kInt, kDouble, kEnum, kIntArray, kDoubleArray,  kVarIntArray, kVarDoubleArray};
    
    struct Attribute
    {
        class AttrEnum
        {
            
        public:
            
            AttrEnum() : mValue(0){}
            
            ~AttrEnum()
            {
                for (unsigned long i = 0; i < mItems.size(); i++)
                    free(mItems[i]);
            }
            
            void add(char *string)
            {
                mItems.push_back(strdup(string));
            }
            
            unsigned long size()
            {
                return mItems.size();
            }

            char *getItem(unsigned long item)
            {
                return mItems[item];
            }
            
            FL_SInt64 get()
            {
                return (FL_SInt64) mValue;
            }
            
            char *get(char **value)
            {
                return mItems[mValue];
            }
            
            void set(FL_SInt64 value)
            {
                mValue = (unsigned long) ((value >= mItems.size()) ? (mItems.size() - 1) : ((value < 0) ? 0 : value));
            }
            
            void set(char *string)
            {
                for (unsigned long i = 0; i < mItems.size(); i++)
                {
                    if (strcmp(string, mItems[i]) == 0)
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
        
        template <class T> class AttrArray
        {
            enum clipMode {kNone, kMin, kMax, kClip};
            
        public:
            
            AttrArray(T defaultValue, FL_UInt64 maxSize) : mItems(new T[maxSize]), mMaxSize(maxSize), mVariableSize(FALSE)
            {
                mSize = mMaxSize;
                mMode = kNone;
                init(defaultValue);
            }
            
            AttrArray(T defaultValue, FL_UInt64 maxSize, FL_UInt64 size) : mItems(new T[maxSize]), mMaxSize(maxSize), mVariableSize(TRUE)
            {
                mSize = size < mMaxSize ? size : mMaxSize;
                mMode = kNone;
                init(defaultValue);
            }
            
            ~AttrArray()
            {
                delete mItems;
            }
            
            void init(T defaultValue)
            {
                for (FL_UInt64 i = 0; i < mSize; i++)
                    mItems[i] = defaultValue;
                
                mDefaultValue = defaultValue;
            }

            FL_UInt64 size()
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
            
            void set(T *values, FL_UInt64 size)
            {
                size = size > mMaxSize ? mMaxSize : size;

                switch(mMode)
                {
                    case kNone:
                        for (FL_UInt64 i = 0; i < size; i++)
                            mItems[i] = values[i];
                    case kMin:
                        for (FL_UInt64 i = 0; i < size; i++)
                            mItems[i] = values[i] < mMin ? mMin : values[i];
                        break;
                    case kMax:
                        for (FL_UInt64 i = 0; i < size; i++)
                            mItems[i] = values[i] > mMax ? mMax : values[i];
                        break;
                    case kClip:
                        for (FL_UInt64 i = 0; i < size; i++)
                            mItems[i] = values[i] < mMin ? mMin : (values[i] > mMax ? mMax : values[i]);
                        break;
                }
                
                if (!mVariableSize)
                    for (FL_UInt64 i = size; i < mMaxSize; i++)
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
            FL_UInt64 mSize;
            
            const FL_UInt64 mMaxSize;
            const bool mVariableSize;
        };
        
        Attribute(char *name, bool value)
        {
            mType = kBool;
            mName = strdup(name);
            mBool = value;
        }
        
        Attribute(char *name, FL_SInt64 value)
        {
            mType = kInt;
            mName = strdup(name);
            mInt = value;
            mLimitsInt[0] = std::numeric_limits<FL_SInt64>::min();
            mLimitsInt[1] = std::numeric_limits<FL_SInt64>::max();
        }
        
        Attribute(char *name)
        {
            mType = kEnum;
            mName = strdup(name);
            mEnum = new AttrEnum();
        }
        
        Attribute(char *name, double value)
        {
            mType = kDouble;
            mName = strdup(name);
            mDouble = value;
            mLimitsDouble[0] = -std::numeric_limits<double>::infinity();
            mLimitsDouble[1] = std::numeric_limits<double>::infinity();
        }
        
        Attribute(char *name, FL_SInt64 defaultValue, FL_UInt64 maxSize)
        {
            mType = kIntArray;
            mName = strdup(name);
            mIntArray = new AttrArray <FL_SInt64> (defaultValue, maxSize);
        }
        
        Attribute(char *name, double defaultValue, FL_UInt64 maxSize)
        {
            mType = kDoubleArray;
            mName = strdup(name);
            mDoubleArray = new AttrArray <double> (defaultValue, maxSize);
        }
        
        Attribute(char *name, FL_SInt64 defaultValue, FL_UInt64 maxSize, FL_UInt64 size)
        {
            mType = kVarIntArray;
            mName = strdup(name);
            mIntArray = new AttrArray <FL_SInt64> (defaultValue, maxSize, size);
        }
        
        Attribute(char *name, double defaultValue, FL_UInt64 maxSize, FL_UInt64 size)
        {
            mType = kVarDoubleArray;
            mName = strdup(name);
            mDoubleArray = new AttrArray <double> (defaultValue, maxSize, size);
        }

        ~Attribute()
        {
            free(mName);
            
            if (mType == kEnum)
                delete mEnum;
            if (mType == kIntArray || mType == kVarIntArray)
                delete mIntArray;
            if (mType == kDoubleArray || mType == kVarDoubleArray)
                delete mDoubleArray;
        }
        
        AttrType mType;
        
        char *mName;
        
        union
        {
            bool mBool;
            FL_SInt64 mInt;
            double mDouble;
            AttrEnum *mEnum;
            AttrArray <FL_SInt64> *mIntArray;
            AttrArray <double> *mDoubleArray;
            
        };
        
        union
        {
            FL_SInt64 mLimitsInt[2];
            double mLimitsDouble[2];
        };
    };
    
public:
    
    void add(char *name, bool value)
    {
        mAttributes.push_back(Attribute(name, value));
    }
    
    void add(char *name, FL_SInt64 value)
    {
        mAttributes.push_back(Attribute(name, value));
    }
    
    void add(char *name, double value)
    {
        mAttributes.push_back(Attribute(name, value));
    }
    
    void add(char *name)
    {
        mAttributes.push_back(Attribute(name));
    }
    
    void addEnum(char *string)
    {
        if (mAttributes.back().mType == kEnum)
            mAttributes.back().mEnum->add(string);
    }
    
    void add(char *name, FL_SInt64 defaultValue, FL_UInt64 maxSize)
    {
        mAttributes.push_back(Attribute(name, defaultValue, maxSize));
    }
    
    void add(char *name, double defaultValue, FL_UInt64 maxSize)
    {
        mAttributes.push_back(Attribute(name, defaultValue, maxSize));
    }
    
    void add(char *name, FL_SInt64 defaultValue, FL_UInt64 maxSize, FL_UInt64 size)
    {
        mAttributes.push_back(Attribute(name, defaultValue, maxSize, size));
    }
    
    void add(char *name, double defaultValue, FL_UInt64 maxSize, FL_UInt64 size)
    {
        mAttributes.push_back(Attribute(name, defaultValue, maxSize, size));
    }
    
    void setMin(FL_SInt64 min)
    {
        AttrType type = mAttributes.back().mType;
        
        if (type == kInt)
            mAttributes.back().mLimitsInt[0] = min;
        if (type == kIntArray || type == kVarIntArray)
            mAttributes.back().mIntArray->setMin(min);
        if (type == kDouble)
            mAttributes.back().mLimitsDouble[0] = (double) min;
        if (type == kDoubleArray || type == kVarDoubleArray)
            mAttributes.back().mDoubleArray->setMin((double) min);
    }
    
    void setMax(FL_SInt64 max)
    {
        AttrType type = mAttributes.back().mType;
        
        if (type == kInt)
            mAttributes.back().mLimitsInt[1] = max;
        if (type == kIntArray || type == kVarIntArray)
            mAttributes.back().mIntArray->setMax(max);
        if (type == kDouble)
            mAttributes.back().mLimitsDouble[1] = (double) max;
        if (type == kDoubleArray || type == kVarDoubleArray)
            mAttributes.back().mDoubleArray->setMax((double) max);
    }
    
    void setClip(FL_SInt64 min, FL_SInt64 max)
    {
        AttrType type = mAttributes.back().mType;
        
        if (type == kInt)
        {
            mAttributes.back().mLimitsInt[0] = min;
            mAttributes.back().mLimitsInt[1] = max;
        }
        
        if (type == kIntArray || type == kVarIntArray)
            mAttributes.back().mIntArray->setClip(min, max);
        
        if (type == kDouble)
        {
            mAttributes.back().mLimitsDouble[0] = (double) min;
            mAttributes.back().mLimitsDouble[1] = (double) max;
        }
        
        if (type == kDoubleArray || type == kVarDoubleArray)
            mAttributes.back().mDoubleArray->setClip((double) min, (double) max);
    }
    
    void setMin(double min)
    {
        AttrType type = mAttributes.back().mType;
        
        if (type == kInt)
            mAttributes.back().mLimitsInt[0] = (FL_SInt64) min;
        if (type == kIntArray || type == kVarIntArray)
            mAttributes.back().mIntArray->setMin((FL_SInt64) min);
        if (type == kDouble)
            mAttributes.back().mLimitsDouble[0] = min;
        if (type == kDoubleArray || type == kVarDoubleArray)
            mAttributes.back().mDoubleArray->setMin(min);
    }
    
    void setMax(double max)
    {
        AttrType type = mAttributes.back().mType;
        
        if (type == kInt)
            mAttributes.back().mLimitsInt[1] = (FL_SInt64) max;
        if (type == kIntArray || type == kVarIntArray)
            mAttributes.back().mIntArray->setMax((FL_SInt64) max);
        if (type == kDouble)
            mAttributes.back().mLimitsDouble[1] = max;
        if (type == kDoubleArray || type == kVarDoubleArray)
            mAttributes.back().mDoubleArray->setMax(max);
    }
    
    void setClip(double min, double max)
    {
        AttrType type = mAttributes.back().mType;
        
        if (type == kInt)
        {
            mAttributes.back().mLimitsInt[0] = (FL_SInt64) min;
            mAttributes.back().mLimitsInt[1] = (FL_SInt64) max;
        }
        
        if (type == kIntArray || type == kVarIntArray)
            mAttributes.back().mIntArray->setClip((FL_SInt64) min, (FL_SInt64) max);
        
        if (type == kDouble)
        {
            mAttributes.back().mLimitsDouble[0] = min;
            mAttributes.back().mLimitsDouble[1] = max;
        }
        
        if (type == kDoubleArray || type == kVarDoubleArray)
            mAttributes.back().mDoubleArray->setClip(min, max);

    }
    
    unsigned long size()
    {
        return mAttributes.size();
    }
    
    long getIdx(char *name)
    {
        for (unsigned long i = 0; i < mAttributes.size(); i++)
            if (strcmp(name, mAttributes[i].mName) == 0)
                return i;
        
        return -1;
    }
    
    AttrType getType(unsigned long idx)
    {
        return getType(idx);
    }
    
    AttrType getType(char *name)
    {
        return getType(getIdx(name));
    }
    
    unsigned long getEnumSize(unsigned long idx)
    {
        if (getType(idx) == kEnum)
            return mAttributes[idx].mEnum->size();
            
        return 0;
    }
    
    char *getEnumItem(unsigned long idx, unsigned long item)
    {
        if (getType(idx) == kEnum)
            return mAttributes[idx].mEnum->getItem(item);
        
        return NULL;
    }
    
    unsigned long getEnumSize(char *name)
    {
        return getEnumSize(getIdx(name));
    }
    
    char *getEnumItem(char *name, unsigned long item)
    {
        return getEnumItem(getIdx(name), item);;
    }
    
    void set(unsigned long idx, bool value)
    {
        if (getType(idx) == kBool)
            mAttributes[idx].mBool = value;
    }
    
    void set(unsigned long idx, FL_SInt64 value)
    {
        FL_SInt64 min = mAttributes[idx].mLimitsInt[0];
        FL_SInt64 max = mAttributes[idx].mLimitsInt[1];
        
        if (getType(idx) == kInt)
            mAttributes[idx].mInt = (value < min) ? min : ((value > max) ? max : value);
        if (getType(idx) == kEnum)
            mAttributes[idx].mEnum->set(value);
    }
    
    void set(unsigned long idx, double value)
    {
        double min = mAttributes[idx].mLimitsDouble[0];
        double max = mAttributes[idx].mLimitsDouble[1];
        
        if (getType(idx) == kDouble)
            mAttributes[idx].mInt = (value < min) ? min : ((value > max) ? max : value);
    }
    
    void set(unsigned long idx, char *string)
    {
        if (getType(idx) == kEnum)
            mAttributes[idx].mEnum->set(string);
    }
    
    void set(unsigned long idx, FL_SInt64 *values, FL_UInt64 size)
    {
        if (getType(idx) == kIntArray || getType(idx) == kVarIntArray)
            mAttributes[idx].mIntArray->set(values, size);
    }
    
    void set(unsigned long idx, double *values, FL_UInt64 size)
    {
        if (getType(idx) == kDoubleArray || getType(idx) == kVarDoubleArray)
            mAttributes[idx].mDoubleArray->set(values, size);
    }
    
    void set(char *name, bool value)
    {
       set(getIdx(name), value);
    }
    
    void set(char *name, FL_SInt64 value)
    {
        set(getIdx(name), value);
    }
    
    void set(char *name, char *string)
    {
        set(getIdx(name), string);
    }
    
    void set(char *name, double value)
    {
        set(getIdx(name), value);
    }
    
    void set(char *name, FL_SInt64 *values, FL_UInt64 size)
    {
        set(getIdx(name), values, size);
    }
    
    void set(char *name, double *values, FL_UInt64 size)
    {
        set(getIdx(name), values, size);
    }

    void get(unsigned long idx, bool *value)
    {
        if (getType(idx) == kBool)
            *value = mAttributes[idx].mBool;
    }
    
    void get(unsigned long idx, FL_SInt64 *value)
    {
        if (getType(idx) == kInt)
            *value = mAttributes[idx].mInt;
        if (getType(idx) == kEnum)
            *value = mAttributes[idx].mEnum->get();
    }
    
    void get(unsigned long idx, char **value)
    {
        if (getType(idx) == kEnum)
            mAttributes[idx].mEnum->get(value);
    }
    
    void get(unsigned long idx, double *value)
    {
        if (getType(idx) == kDouble)
            *value = mAttributes[idx].mDouble;
    }

    void get(unsigned long idx, FL_SInt64 **values, FL_UInt64 *size)
    {
        if (getType(idx) == kIntArray || getType(idx) == kVarIntArray)
        {
            *values = mAttributes[idx].mIntArray->get();
            *size = mAttributes[idx].mIntArray->size();
        }
    }
    
    void get(unsigned long idx, double **values, FL_UInt64 *size)
    {
        if (getType(idx) == kDoubleArray || getType(idx) == kVarDoubleArray)
        {
            *values = mAttributes[idx].mDoubleArray->get();
            *size = mAttributes[idx].mDoubleArray->size();
        }
    }
    
    void get(char *name, bool *value)
    {
        get(getIdx(name), value);
    }
    
    void get(char *name, FL_SInt64 *value)
    {
        get(getIdx(name), value);
    }
    
    void get(char *name, char **value)
    {
        get(getIdx(name), value);
    }
    
    void get(char *name, double *value)
    {
        get(getIdx(name), value);
    }
    
    void get(char *name, FL_SInt64 **values, FL_UInt64 *size)
    {
        get(getIdx(name), values, size);
    }
    
    void get(char *name, double **values, FL_UInt64 *size)
    {
        get(getIdx(name), values, size);
    }
    
private:
    
    std::vector <Attribute> mAttributes;
};

#endif
