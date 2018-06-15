
#include "FrameLib_Parameters.h"
#include "FrameLib_Memory.h"

#include <algorithm>

// Serial Iterator

// Operators

FrameLib_Parameters::Serial::Iterator& FrameLib_Parameters::Serial::Iterator::operator ++ ()
{
    DataType type = Serial::readType(&mPtr);
    Serial::skipItem(&mPtr, kSingleString);
    Serial::skipItem(&mPtr, type);
    mIndex++;
    
    return *this;
}

FrameLib_Parameters::Serial::Iterator FrameLib_Parameters::Serial::Iterator::operator ++ (int)
{
    Iterator result = *this;
    operator ++();
    return result;
}
// Get Underlying data

const double *FrameLib_Parameters::Serial::Iterator::getVector(unsigned long *size) const
{
    Entry entry = getEntry();
    
    if (entry.mType == kVector)
    {
        *size = entry.mSize;
        return entry.data<double>();
    }
        
    *size = 0;
    return nullptr;
}

unsigned long FrameLib_Parameters::Serial::Iterator::getVectorSize() const
{
    unsigned long size;
    
    getVector(&size);
    
    return size;
}

char *FrameLib_Parameters::Serial::Iterator::getString() const
{
    Entry entry = getEntry();
    
    return entry.mType == kSingleString ? entry.data<char>() : nullptr;
}

// Get Size

size_t FrameLib_Parameters::Serial::Iterator::getSize() const
{
    Entry entry = getEntry();
    
    switch (entry.mType)
    {
        case kVector:           return calcSize(entry.mTag, entry.mSize);
        case kSingleString:     return calcSize(entry.mTag, entry.data<char>());
    }
}

// Reads

void FrameLib_Parameters::Serial::Iterator::read(Serial *serial) const
{
    Entry entry = getEntry();
    
    switch (entry.mType)
    {
        case kVector:           serial->write(entry.mTag, entry.data<double>(), entry.mSize);       break;
        case kSingleString:     serial->write(entry.mTag, entry.data<char>());                      break;
    }
}

void FrameLib_Parameters::Serial::Iterator::read(FrameLib_Parameters *parameters) const
{
    Entry entry = getEntry();
    
    switch (entry.mType)
    {
        case kVector:           parameters->set(entry.mTag, entry.data<double>(), entry.mSize);     break;
        case kSingleString:     parameters->set(entry.mTag, entry.data<char>());                    break;
    }
}

size_t FrameLib_Parameters::Serial::Iterator::read(double *output, unsigned long size) const
{
    Entry entry = getEntry();
    
    if (entry.mType == kVector)
    {
        size = std::min(entry.mSize, size);
        std::copy(entry.data<double>(), entry.data<double>() + entry.mSize, output);
        return size;
    }
    
    return 0;
}

// Aliases (read but aliasing to a new tag)

void FrameLib_Parameters::Serial::Iterator::alias(Serial *serial, const char *tag) const
{
    Entry entry = getEntry();
    
    switch (entry.mType)
    {
        case kVector:           serial->write(tag, entry.data<double>(), entry.mSize);       break;
        case kSingleString:     serial->write(tag, entry.data<char>());                      break;
    }
}

void FrameLib_Parameters::Serial::Iterator::alias(FrameLib_Parameters *parameters, const char *tag) const
{
    Entry entry = getEntry();
    
    switch (entry.mType)
    {
        case kVector:           parameters->set(tag, entry.data<double>(), entry.mSize);     break;
        case kSingleString:     parameters->set(tag, entry.data<char>());                    break;
    }
}

// Get Entry

FrameLib_Parameters::Serial::Iterator::Entry FrameLib_Parameters::Serial::Iterator::getEntry() const
{
    Entry entry;
    BytePointer ptr = mPtr;
    BytePointer tagRaw;
    
    entry.mType = Serial::readType(&ptr);
    Serial::readItem(&ptr, kSingleString, &tagRaw, &entry.mSize);
    Serial::readItem(&ptr, entry.mType, &entry.mData, &entry.mSize);
    
    entry.mTag = reinterpret_cast<char *>(tagRaw);
    
    return entry;
}

// ************************************************************************************** //

// Serial Class

// Constructors / Destructor

FrameLib_Parameters::Serial::Serial(BytePointer ptr, size_t size) : mPtr(ptr), mSize(0), mMaxSize(size), mNumTags(0)
{
    alignmentChecks();
}

FrameLib_Parameters::Serial::Serial() : mPtr(nullptr), mSize(0), mMaxSize(0), mNumTags(0)
{
    alignmentChecks();
}

// Size Calculation

size_t FrameLib_Parameters::Serial::calcSize(const FrameLib_Parameters *params)
{
    size_t size = 0;
    
    for (unsigned long i = 0; i < params->size(); i++)
    {
        Type type = params->getType(i);
        
        switch(type)
        {
            case kString:
                size += calcSize(params->getName(i).c_str(), params->getString(i));
                break;
                
            case kValue:
            case kEnum:
                size += calcSize(params->getName(i).c_str(), 1);
                break;
                
            case kArray:
            case kVariableArray:
                size += calcSize(params->getName(i).c_str(), params->getArraySize(i));

        }
    }
    
    return size;
}

// Get Sizes

size_t FrameLib_Parameters::Serial::getSize(const char *tag) const
{
    auto it = find(tag);
    
    return (it != end()) ? it.getSize() : 0;
}

size_t FrameLib_Parameters::Serial::getVectorSize(const char *tag) const
{
    auto it = find(tag);
    
    return (it != end()) ? it.getVectorSize() : 0;
}

// Public Writes

void FrameLib_Parameters::Serial::write(const Serial *serialised)
{
    if (!serialised || !checkSize(serialised->mSize))
        return;
    
    memcpy(mPtr + mSize, serialised->mPtr, serialised->mSize);
    mSize += serialised->mSize;
    mNumTags += serialised->mNumTags;
}

void FrameLib_Parameters::Serial::write(const FrameLib_Parameters *params)
{
    double value;
    
    for (unsigned long i = 0; i < params->size(); i++)
    {
        Type type = params->getType(i);
        
        switch(type)
        {
            case kString:
                write(params->getName(i).c_str(), params->getString(i));
                break;
                
            case kValue:
            case kEnum:
                value = params->getValue(i);
                write(params->getName(i).c_str(), &value, 1);
                break;
                
            case kArray:
            case kVariableArray:
                write(params->getName(i).c_str(), params->getArray(i), params->getArraySize(i));
        }
    }
}

void FrameLib_Parameters::Serial::write(const Serial::Iterator& it)
{
    it.read(this);
}

void FrameLib_Parameters::Serial::write(const char *tag, const char *str)
{
    if (!checkSize(calcSize(tag, str)))
        return;
    
    writeType(kSingleString);
    writeString(tag);
    writeString(str);
    mNumTags++;
}

void FrameLib_Parameters::Serial::write(const char *tag, const double *values, size_t N)
{    
    if (!checkSize(calcSize(tag, N)))
        return;
    
    writeType(kVector);
    writeString(tag);
    writeDoubles(values, N);
    mNumTags++;
}

// Public Reads

void FrameLib_Parameters::Serial::read(FrameLib_Parameters *parameters) const
{
    for (auto it = begin(); it != end(); it++)
        it.read(parameters);
}

bool FrameLib_Parameters::Serial::read(const char *tag, FrameLib_Parameters *parameters) const
{
    auto it = find(tag);
    
    if (it != end())
        it.read(parameters);
    
    return it != end();
}

size_t FrameLib_Parameters::Serial::read(const char *tag, double *output, unsigned long size) const
{
    auto it = find(tag);
    
    return (it != end()) ? it.read(output, size) : 0;
}

// Find Item

FrameLib_Parameters::Serial::Iterator FrameLib_Parameters::Serial::find(const char *tag) const
{
    for (auto it = begin(); it != end(); it++)
        if (it.matchTag(tag))
            return it;
    
    return end();
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
    mSize += sizeType();
}

void FrameLib_Parameters::Serial::writeSize(size_t size)
{
    *((size_t *) (mPtr + mSize)) = size;
    mSize += sizeSize();
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

DataType FrameLib_Parameters::Serial::readType(BytePointer *readPtr)
{
    DataType type = *reinterpret_cast<DataType *>(*readPtr);
    *readPtr += sizeType();
    return type;
}

void FrameLib_Parameters::Serial::readSize(BytePointer *readPtr, size_t *size)
{
    *size = *reinterpret_cast<size_t *>(*readPtr);
    *readPtr += sizeSize();
}

void FrameLib_Parameters::Serial::readItem(BytePointer *readPtr, DataType type, BytePointer *data, size_t *size)
{
    readSize(readPtr, size);
    *data = *readPtr;
    *readPtr += alignSize(*size * (type == kVector ? sizeof(double) : sizeof(char)));
}

void FrameLib_Parameters::Serial::skipItem(BytePointer *readPtr, DataType type)
{
    size_t size;
    Serial::readSize(readPtr, &size);
    *readPtr += alignSize(size * (type == kVector ? sizeof(double) : sizeof(char)));
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

FrameLib_Parameters::SetError FrameLib_Parameters::Parameter::set(double *values, size_t N)
{
    if (N)
        return set(*values);
    else
    {
        clear();
        return kSetSucceeded;
    }
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
    return nullptr;
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

FrameLib_Parameters::SetError FrameLib_Parameters::Enum::set(double value)
{
    bool outOfRange = value >= mItems.size();
    mValue = static_cast<unsigned long>(outOfRange ? (mItems.size() - 1) : (value < 0.0 ? 0.0 : value));
    mChanged = true;
    
    return outOfRange ? kEnumUnknownIndex : kSetSucceeded;
}

FrameLib_Parameters::SetError FrameLib_Parameters::Enum::set(const char *str)
{
    for (unsigned long i = 0; i < mItems.size(); i++)
    {
        if (strcmp(str, mItems[i].c_str()) == 0)
        {
            mValue = i;
            mChanged = true;
            return kSetSucceeded;
        }
    }
    
    return kEnumUnknownString;
}

FrameLib_Parameters::SetError FrameLib_Parameters::Enum::set(double *values, size_t N)
{
    if (N)
    {
        return Enum::set(*values);
    }
    else
    {
        Enum::clear();
        return kSetSucceeded;
    }
}

// ************************************************************************************** //

// Value Parameter Class

FrameLib_Parameters::SetError FrameLib_Parameters::Value::set(double value)
{
    mValue = (value < mMin) ? mMin : ((value > mMax) ? mMax : value);
    mChanged = true;
    
    return kSetSucceeded;
}

FrameLib_Parameters::SetError FrameLib_Parameters::Value::set(double *values, size_t N)
{
    if (N)
        Value::set(*values);
    else
        Value::clear();
    
    return kSetSucceeded;
}

// ************************************************************************************** //

// String Parameter Class

FrameLib_Parameters::String::String(const char *name, long argumentIdx) : Parameter(name, argumentIdx)
{
    setNonNumeric();
    String::clear();
    mMin = mMax = 0.0;
}

FrameLib_Parameters::SetError FrameLib_Parameters::String::set(const char *str)
{
    size_t i = 0;
    
    if (str != nullptr)
    {
        for (i = 0; i < maxLen; i++)
            if ((mCString[i] = str[i]) == 0)
                break;
    }
    
    mCString[i] = 0;
    mChanged = true;
    
    return kSetSucceeded;
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

FrameLib_Parameters::SetError FrameLib_Parameters::Array::set(double *values, size_t N)
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
    
    return kSetSucceeded;
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
