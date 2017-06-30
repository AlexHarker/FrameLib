
#include "FrameLib_Attributes.h"

// Constructors / Destructor

FrameLib_Attributes::Serial::Serial(BytePointer ptr, size_t size) : mPtr(ptr), mSize(0), mMaxSize(size), mOwner(false)
{
    alignmentChecks();
}

FrameLib_Attributes::Serial::Serial(size_t size) : mPtr(new Byte[size]), mSize(0), mMaxSize(size), mOwner(true)
{
    alignmentChecks();
}

FrameLib_Attributes::Serial::Serial() : mPtr(NULL), mSize(0), mMaxSize(0), mOwner(true)
{
    alignmentChecks();
}

FrameLib_Attributes::Serial::~Serial()
{
    if (mOwner)
        delete[] mPtr;
}

// Public Writes

void FrameLib_Attributes::Serial::write(Serial *serialised)
{
    if (!serialised || !checkSize(serialised->mSize))
        return;
    
    memcpy(mPtr + mSize, serialised->mPtr, serialised->mSize);
    mSize += serialised->mSize;
}

void FrameLib_Attributes::Serial::write(const char *tag, char *str)
{
    if (!checkSize(calcSize(tag, str)))
        return;
    
    writeType(kString);
    writeString(tag);
    writeString(str);
}

void FrameLib_Attributes::Serial::write(const char *tag, double *values, size_t N)
{
    if (!N || !checkSize(calcSize(tag, N)))
        return;
    
    writeType(kDoubleArray);
    writeString(tag);
    writeDoubles(values, N);
}

// Public Read

void FrameLib_Attributes::Serial::read(FrameLib_Attributes *attributes)
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

// Implementation

void FrameLib_Attributes::Serial::alignmentChecks()
{
    // Assume that alignment of a double is fine for all natural alignment needs (including this class)
    
    assert(Serial::alignment >= sizeof(DataType) && "alignment assumptions are incorrect for FrameLib_Attributes::Serial::DataType");
    assert(Serial::alignment >= sizeof(size_t) && "alignment assumptions are incorrect for FrameLib_Attributes::Serial");
    assert(Serial::alignment >= sizeof(char) && "alignment assumptions are incorrect for FrameLib_Attributes::Serial");
    assert(Serial::alignment >= sizeof(char *) && "alignment assumptions are incorrect for FrameLib_Attributes::Serial");
    // FIX - swapped the below inequality - check it is now correct and find an appropriate call
    //assert(Serial::alignment <= FrameLib_MainAllocator::getAlignment() && "alignment assumptions are incorrect for FrameLib_Attributes::Serial");
}

void FrameLib_Attributes::Serial::writeType(DataType type)
{
    *((DataType *) (mPtr + mSize)) = type;
    mSize += align(sizeof(DataType));
}

void FrameLib_Attributes::Serial::writeSize(size_t size)
{
    * ((size_t *) (mPtr + mSize)) = size;
    mSize += align(sizeof(size_t));
}

void FrameLib_Attributes::Serial::writeString(const char *str)
{
    size_t N = strlen(str) + 1;
    writeSize(N);
    strcpy((char *) (mPtr + mSize), str);
    mSize += align(N);
}

// N.B. the assumption is that double is the largest type, and thus alignment is not necessary here

void FrameLib_Attributes::Serial::writeDoubles(double *ptr, size_t N)
{
    size_t size = N * sizeof(double);
    writeSize(N);
    memcpy(mPtr + mSize, ptr, size);
    mSize += align(size);
}

FrameLib_Attributes::Serial::DataType FrameLib_Attributes::Serial::readType(BytePointer *readPtr)
{
    DataType type = *((DataType *) *readPtr);
    *readPtr += align(sizeof(DataType));
    return type;
}

void FrameLib_Attributes::Serial::readSize(BytePointer *readPtr, size_t *size)
{
    *size = *((size_t *) *readPtr);
    *readPtr += align(sizeof(size_t));
}

void FrameLib_Attributes::Serial::readDoubles(BytePointer *readPtr, double **values, size_t *N)
{
    readSize(readPtr, N);
    *values = ((double *) *readPtr);
    *readPtr += align(*N * sizeof(double));
}

void FrameLib_Attributes::Serial::readString(BytePointer *readPtr, char **str)
{
    size_t size;
    readSize(readPtr, &size);
    *str = ((char *) *readPtr);
    *readPtr += align(size);
}

bool FrameLib_Attributes::Serial::checkSize(size_t writeSize)
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

// ************************************************************************************** //

FrameLib_Attributes::Attribute::Attribute(const char *name, long argumentIdx) : mChanged(false)
{
    mName = name;
    mArgumentIdx = argumentIdx;
}

void FrameLib_Attributes::Attribute::addEnumItem(const char *str)
{
    assert(0 && "cannot add enum items to non-enum attribute");
}

void FrameLib_Attributes::Attribute::setMin(double min)
{
    assert(0 && "attribute type does not support minimum values");
}

void FrameLib_Attributes::Attribute::setMax(double max)
{
    assert(0 && "attribute type does not support maximum values");
}

void FrameLib_Attributes::Attribute::setClip(double min, double max)
{
    assert(0 && "attribute type does not support clipping values");
}

void FrameLib_Attributes::Attribute::set(const char *str)
{
    assert(0 && "attribute type does not support setting by string");
}

void FrameLib_Attributes::Attribute::set(double value)
{
    assert(0 && "attribute type does not support setting by double");
}

void FrameLib_Attributes::Attribute::set(double *values, size_t size)
{
    set(*values);
}

void FrameLib_Attributes::Attribute::getRange(double *min, double *max)
{
    *min = 0;
    *max = 0;
}

const char *FrameLib_Attributes::Attribute::getItemString(unsigned long item) const
{
    assert(0 && "cannot get enum string for non-enum attribute");
}

const double *FrameLib_Attributes::Attribute::getArray(size_t *size) const
{
    *size = getArraySize();
    return getArray();
}

bool FrameLib_Attributes::Attribute::changed()
{
    bool result = mChanged;
    mChanged = false;
    return result;
}

// ************************************************************************************** //

void FrameLib_Attributes::Bool::set(double value)
{
    mValue = value ? true : false;
    mChanged = true;
}

void FrameLib_Attributes::Bool::getRange(double *min, double *max)
{
    *min = false;
    *max = true;
}

// ************************************************************************************** //

void FrameLib_Attributes::Enum::set(double value)
{
    mValue = ((value >= mItems.size()) ? (mItems.size() - 1) : (value < 0 ? 0 : value));
    mChanged = true;
}

void FrameLib_Attributes::Enum::set(const char *str)
{
    for (unsigned long i = 0; i < mItems.size(); i++)
    {
        if (strcmp(str, mItems[i].c_str()) == 0)
        {
            mValue = i;
            mChanged = true;
            return;
        }
    }
}

void FrameLib_Attributes::Enum::getRange(double *min, double *max)
{
    *min = 0;
    *max = mItems.size() - 1;
}

// ************************************************************************************** //

void FrameLib_Attributes::Double::set(double value)
{
    mValue = (value < mMin) ? mMin : ((value > mMax) ? mMax : value);
    mChanged = true;
}
    
void FrameLib_Attributes::Double::getRange(double *min, double *max)
{
    *min = mMin;
    *max = mMax;
}

void FrameLib_Attributes::Double::setMin(double min)
{
    mMin = min;
    mMax = std::numeric_limits<double>::infinity();
}

void FrameLib_Attributes::Double::setMax(double max)
{
    mMin = -std::numeric_limits<double>::infinity();
    mMax = max;
}

void FrameLib_Attributes::Double::setClip(double min, double max)
{
    mMin = min;
    mMax = max;
}

// ************************************************************************************** //

FrameLib_Attributes::String::String(const char *name, long argumentIdx) : Attribute(name, argumentIdx)
{
    mCString[0] = 0;
}

void FrameLib_Attributes::String::set(const char *str)
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

// ************************************************************************************** //

FrameLib_Attributes::Array::Array(const char *name, long argumentIdx, double defaultValue, size_t size)
: Attribute(name, argumentIdx), mMode(kNone), mDefaultValue(defaultValue), mSize(size), mVariableSize(false)
{
    mItems.resize(size);
    
    for (size_t i = 0; i < mSize; i++)
        mItems[i] = defaultValue;
}

FrameLib_Attributes::Array::Array(const char *name, long argumentIdx, double defaultValue, size_t maxSize, size_t size)
: Attribute(name, argumentIdx), mMode(kNone), mDefaultValue(defaultValue), mVariableSize(true)
{
    mItems.resize(maxSize);
    
    mSize = size < maxSize ? size : maxSize;
    
    for (size_t i = 0; i < mSize; i++)
        mItems[i] = defaultValue;
}

void FrameLib_Attributes::Array::setMin(double min)
{
    mMode = kMin;
    mMin = min;
}

void FrameLib_Attributes::Array::setMax(double max)
{
    mMode = kMax;
    mMax = max;
}

void FrameLib_Attributes::Array::setClip(double min, double max)
{
    mMode = kClip;
    mMin = min;
    mMax = max;
}

void FrameLib_Attributes::Array::set(double *values, size_t size)
{
    size = size > mItems.size() ? mItems.size() : size;
    
    switch (mMode)
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

void FrameLib_Attributes::Array::getRange(double *min, double *max)
{
    *min = mMin;
    *max = mMax;
}
