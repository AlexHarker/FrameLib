
#include "FrameLib_Parameters.h"

// Constructors / Destructor

FrameLib_Parameters::Serial::Serial(BytePointer ptr, size_t size) : mPtr(ptr), mSize(0), mMaxSize(size)
{
    alignmentChecks();
}

FrameLib_Parameters::Serial::Serial() : mPtr(NULL), mSize(0), mMaxSize(0)
{
    alignmentChecks();
}

// Public Writes

void FrameLib_Parameters::Serial::write(Serial *serialised)
{
    if (!serialised || !checkSize(serialised->mSize))
        return;
    
    memcpy(mPtr + mSize, serialised->mPtr, serialised->mSize);
    mSize += serialised->mSize;
}

void FrameLib_Parameters::Serial::write(const char *tag, const char *str)
{
    if (!checkSize(calcSize(tag, str)))
        return;
    
    writeType(kString);
    writeString(tag);
    writeString(str);
}

void FrameLib_Parameters::Serial::write(const char *tag, const double *values, size_t N)
{    
    if (!checkSize(calcSize(tag, N)))
        return;
    
    writeType(kDoubleArray);
    writeString(tag);
    writeDoubles(values, N);
}

// Public Read

void FrameLib_Parameters::Serial::read(FrameLib_Parameters *parameters) const
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
                parameters->set(tag, values, N);
                break;
                
            case kString:
                readString(&readPtr, &tag);
                readString(&readPtr, &str);
                parameters->set(tag, str);
                break;
        }
    }
}

// Implementation

void FrameLib_Parameters::Serial::alignmentChecks() const
{
    // Assume that alignment of a double is fine for all natural alignment needs (including this class)
    
    assert(Serial::alignment >= sizeof(DataType) && "alignment assumptions are incorrect for FrameLib_Parameters::Serial::DataType");
    assert(Serial::alignment >= sizeof(size_t) && "alignment assumptions are incorrect for FrameLib_Parameters::Serial");
    assert(Serial::alignment >= sizeof(char) && "alignment assumptions are incorrect for FrameLib_Parameters::Serial");
    assert(Serial::alignment >= sizeof(char *) && "alignment assumptions are incorrect for FrameLib_Parameters::Serial");
    assert(Serial::alignment <= FrameLib_GlobalAllocator::getAlignment() && "alignment assumptions are incorrect for FrameLib_Parameters::Serial");
}

// Writes (private)

void FrameLib_Parameters::Serial::writeType(DataType type)
{
    *((DataType *) (mPtr + mSize)) = type;
    mSize += alignSize(sizeof(DataType));
}

void FrameLib_Parameters::Serial::writeSize(size_t size)
{
    *((size_t *) (mPtr + mSize)) = size;
    mSize += alignSize(sizeof(size_t));
}

void FrameLib_Parameters::Serial::writeString(const char *str)
{
    size_t N = strlen(str) + 1;
    writeSize(N);
    strcpy((char *) (mPtr + mSize), str);
    mSize += alignSize(N);
}

void FrameLib_Parameters::Serial::writeDoubles(const double *ptr, size_t N)
{
    size_t size = N * sizeof(double);
    writeSize(N);
    memcpy(mPtr + mSize, ptr, size);
    mSize += alignSize(size);
}

// Reads (private)

FrameLib_Parameters::Serial::DataType FrameLib_Parameters::Serial::readType(BytePointer *readPtr) const
{
    DataType type = *((DataType *) *readPtr);
    *readPtr += alignSize(sizeof(DataType));
    return type;
}

void FrameLib_Parameters::Serial::readSize(BytePointer *readPtr, size_t *size) const
{
    *size = *((size_t *) *readPtr);
    *readPtr += alignSize(sizeof(size_t));
}

void FrameLib_Parameters::Serial::readDoubles(BytePointer *readPtr, double **values, size_t *N) const
{
    readSize(readPtr, N);
    *values = ((double *) *readPtr);
    *readPtr += alignSize(*N * sizeof(double));
}

void FrameLib_Parameters::Serial::readString(BytePointer *readPtr, char **str) const
{
    size_t size;
    readSize(readPtr, &size);
    *str = ((char *) *readPtr);
    *readPtr += alignSize(size);
}

// Size Check

bool FrameLib_Parameters::Serial::checkSize(size_t writeSize)
{
    if (mSize + writeSize <= mMaxSize)
        return true;
    
    return false;
}

// ************************************************************************************** //

// AutoSerial Class (owning/resizing/allocating it's own memory using system routines - not suitable for audio thread use)

bool FrameLib_Parameters::AutoSerial::checkSize(size_t writeSize)
{
    size_t growSize;
    
    if (Serial::checkSize(writeSize))
        return true;
    
    // Calculate grow size
    
    growSize = (mSize + writeSize) - mMaxSize;
    growSize = growSize < minGrowSize ? minGrowSize : growSize;
    
    // Allocate required memory and copy old data before deleting the old pointer
    
    BytePointer newPtr = new Byte[mMaxSize + growSize];
    memcpy(newPtr, mPtr, mSize);
    if (mPtr) delete[] mPtr;
    
    // Update
    
    mPtr = newPtr;
    mMaxSize += growSize;
    
    return true;
}

// ************************************************************************************** //

bool checkMin(double min)
{
    return min == -std::numeric_limits<double>::infinity();
}

bool checkMax(double max)
{
    return max == std::numeric_limits<double>::infinity();
}

bool checkRange(double min, double max)
{
    return checkMin(min) && checkMax(max);
}

// Parameter Abstract Class

FrameLib_Parameters::Parameter::Parameter(const char *name, long argumentIdx)
: mChanged(false), mFlags(0), mDefault(0.0), mMin(-std::numeric_limits<double>::infinity()), mMax(std::numeric_limits<double>::infinity())
{
    mName = name;
    mArgumentIdx = argumentIdx;
}

void FrameLib_Parameters::Parameter::addEnumItem(const char *str)
{
    assert(0 && "cannot add enum items to non-enum parameter");
}

void FrameLib_Parameters::Parameter::setMin(double min)
{
    assert(checkRange(mMin, mMax) && "parameter range has already been set");
    mMin = min;
    mMax = std::numeric_limits<double>::infinity();
}

void FrameLib_Parameters::Parameter::setMax(double max)
{
    assert(checkRange(mMin, mMax) && "parameter range has already been set");
    mMin = -std::numeric_limits<double>::infinity();
    mMax = max;
}

void FrameLib_Parameters::Parameter::setClip(double min, double max)
{
    assert(checkRange(mMin, mMax) && "parameter range has already been set");
    mMin = min;
    mMax = max;
}

void FrameLib_Parameters::Parameter::set(double *values, size_t N)
{
    if (N)
        set(*values);
    else
        clear();
}

FrameLib_Parameters::ClipMode FrameLib_Parameters::Parameter::getClipMode() const
{
    bool maxCheck = checkMax(mMax);
    
    if (checkMin(mMin))
        return maxCheck ? kNone : kMax;
    else
        return maxCheck ? kMin : kClip;
}

void FrameLib_Parameters::Parameter::getRange(double *min, double *max) const
{
    *min = mMin;
    *max = mMax;
}

const char *FrameLib_Parameters::Parameter::getItemString(unsigned long item) const
{
    assert(0 && "cannot get enum string for non-enum parameter");
    return NULL;
}

const double *FrameLib_Parameters::Parameter::getArray(size_t *size) const
{
    *size = getArraySize();
    return getArray();
}

bool FrameLib_Parameters::Parameter::changed()
{
    bool result = mChanged;
    mChanged = false;
    return result;
}

// ************************************************************************************** //

// Enum Parameter Class

FrameLib_Parameters::Enum::Enum(const char *name, long argumentIdx) : Parameter(name, argumentIdx), mValue(0)
{
    mMin = 0.0;
    mMax = -1.0;
    setNonNumeric();
}

// Setters

void FrameLib_Parameters::Enum::addEnumItem(const char *str)
{
    mItems.push_back(str);
    mMax += 1.0;
}

void FrameLib_Parameters::Enum::set(double value)
{
    mValue = ((value >= mItems.size()) ? (mItems.size() - 1) : (value < 0 ? 0 : value));
    mChanged = true;
}

void FrameLib_Parameters::Enum::set(const char *str)
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

void FrameLib_Parameters::Enum::set(double *values, size_t N)
{
    if (N)
        Enum::set(*values);
    else
        Enum::clear();
}

// ************************************************************************************** //

// Value Parameter Class

void FrameLib_Parameters::Value::set(double value)
{
    mValue = (value < mMin) ? mMin : ((value > mMax) ? mMax : value);
    mChanged = true;
}

void FrameLib_Parameters::Value::set(double *values, size_t N)
{
    if (N)
        Value::set(*values);
    else
        Value::clear();
}

// ************************************************************************************** //

// String Parameter Class

FrameLib_Parameters::String::String(const char *name, long argumentIdx) : Parameter(name, argumentIdx)
{
    setNonNumeric();
    String::clear();
    mMin = mMax = 0.0;
}

void FrameLib_Parameters::String::set(const char *str)
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

// Array Parameter Class

FrameLib_Parameters::Array::Array(const char *name, long argumentIdx, double defaultValue, size_t size)
: Parameter(name, argumentIdx), mSize(size), mVariableSize(false)
{
    mDefault = defaultValue;
    mItems.resize(size);
    
    for (size_t i = 0; i < mSize; i++)
        mItems[i] = defaultValue;
}

FrameLib_Parameters::Array::Array(const char *name, long argumentIdx, double defaultValue, size_t maxSize, size_t size)
: Parameter(name, argumentIdx), mVariableSize(true)
{
    mDefault = defaultValue;
    mItems.resize(maxSize);
    
    mSize = size < maxSize ? size : maxSize;
    
    for (size_t i = 0; i < mSize; i++)
        mItems[i] = defaultValue;
}

void FrameLib_Parameters::Array::set(double *values, size_t N)
{
    N = N > mItems.size() ? mItems.size() : N;
    
    switch (getClipMode())
    {
        case kNone:
            for (size_t i = 0; i < N; i++)
                mItems[i] = values[i];
            break;
        case kMin:
            for (size_t i = 0; i < N; i++)
                mItems[i] = values[i] < mMin ? mMin : values[i];
            break;
        case kMax:
            for (size_t i = 0; i < N; i++)
                mItems[i] = values[i] > mMax ? mMax : values[i];
            break;
        case kClip:
            for (size_t i = 0; i < N; i++)
                mItems[i] = values[i] < mMin ? mMin : (values[i] > mMax ? mMax : values[i]);
            break;
    }
    
    if (!mVariableSize)
        for (size_t i = N; i < mItems.size(); i++)
            mItems[i] = mDefault;
    else
        mSize = N;
    
    mChanged = true;
}

// ************************************************************************************** //

// Getters

FrameLib_Parameters::NumericType FrameLib_Parameters::getNumericType(unsigned long idx) const
{
    int flags = mParameters[idx]->flags();
    
    if (flags & Parameter::kFlagNonNumeric) return kNumericNone;
    else if (flags & Parameter::kFlagBool) return kNumericBool;
    else if (flags & Parameter::kFlagInteger) return kNumericInteger;
    
    return kNumericDouble;
}

std::string FrameLib_Parameters::getTypeString(unsigned long idx) const
{
    const char **typeStrings = typeStringsDouble;
    int flags = mParameters[idx]->flags();
    std::string str("");
    
    if (flags & Parameter::kFlagInstantiation)
        str = "instantiation ";
    
    if (flags & Parameter::kFlagBool)
        typeStrings = typeStringsBool;
    else if (flags & Parameter::kFlagInteger)
        typeStrings = typeStringsInteger;
    
    str += typeStrings[mParameters[idx]->type()];
    
    return str;
}

std::string FrameLib_Parameters::getDefaultString(unsigned long idx) const
{
    Type type = getType(idx);
    char numericStr[64];

    if (type == kString)
        return "";
    else if (type == kEnum)
        return getItemString(idx, 0);
    else if (getNumericType(idx) == kNumericBool)
        return getDefault(idx) ? "true" : "false";
    
    sprintf(numericStr, "%lg", getDefault(idx));
    
    return numericStr;
}
