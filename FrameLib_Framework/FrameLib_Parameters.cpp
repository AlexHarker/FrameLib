
#include "FrameLib_Parameters.h"
#include "FrameLib_Memory.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

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

const char *FrameLib_Parameters::Serial::Iterator::getString() const
{
    Entry entry = getEntry();
    
    return entry.mType == kSingleString ? entry.data<char>() : nullptr;
}

// Get Size

unsigned long FrameLib_Parameters::Serial::Iterator::getSize() const
{
    Entry entry = getEntry();
    
    switch (entry.mType)
    {
        case kVector:           return calcSize(entry.mTag, entry.mSize);
        case kSingleString:     return calcSize(entry.mTag, entry.data<char>());
    }
    
    return 0;
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

unsigned long FrameLib_Parameters::Serial::Iterator::read(double *output, unsigned long size) const
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

FrameLib_Parameters::Serial::Serial(BytePointer ptr, unsigned long size) : mPtr(ptr), mSize(0), mMaxSize(size), mNumTags(0)
{
    alignmentChecks();
}

FrameLib_Parameters::Serial::Serial() : mPtr(nullptr), mSize(0), mMaxSize(0), mNumTags(0)
{
    alignmentChecks();
}

// Size Calculation

unsigned long FrameLib_Parameters::Serial::calcSize(const FrameLib_Parameters *params)
{
    unsigned long size = 0;
    
    for (unsigned long i = 0; i < params->size(); i++)
    {
        Type type = params->getType(i);
        
        switch(type)
        {
            case kString:
                size += calcSize(params->getName(i), params->getString(i));
                break;
                
            case kValue:
            case kEnum:
                size += calcSize(params->getName(i), 1);
                break;
                
            case kArray:
            case kVariableArray:
                size += calcSize(params->getName(i), params->getArraySize(i));

        }
    }
    
    return size;
}

// Get Sizes

unsigned long FrameLib_Parameters::Serial::getSize(const char *tag) const
{
    auto it = find(tag);
    
    return (it != end()) ? it.getSize() : 0;
}

unsigned long FrameLib_Parameters::Serial::getVectorSize(const char *tag) const
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
                write(params->getName(i), params->getString(i));
                break;
                
            case kValue:
            case kEnum:
                value = params->getValue(i);
                write(params->getName(i), &value, 1);
                break;
                
            case kArray:
            case kVariableArray:
                write(params->getName(i), params->getArray(i), params->getArraySize(i));
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

void FrameLib_Parameters::Serial::write(const char *tag, const double *values, unsigned long N)
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

unsigned long FrameLib_Parameters::Serial::read(const char *tag, double *output, unsigned long size) const
{
    auto it = find(tag);
    
    return (it != end()) ? it.read(output, size) : 0;
}

// Find Item

FrameLib_Parameters::Serial::Iterator FrameLib_Parameters::Serial::find(const char *tag) const
{
    auto position = end();
    
    for (auto it = begin(); it != end(); it++)
        if (it.matchTag(tag))
            position = it;
    
    return position;
}

// Implementation

void FrameLib_Parameters::Serial::alignmentChecks() const
{
    // Assume that alignment of a double is fine for all natural alignment needs (including this class)
    
    assert(Serial::alignment >= sizeof(DataType) && "alignment assumptions are incorrect for FrameLib_Parameters::Serial::DataType");
    assert(Serial::alignment >= sizeof(unsigned long) && "alignment assumptions are incorrect for FrameLib_Parameters::Serial");
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

void FrameLib_Parameters::Serial::writeSize(unsigned long size)
{
    *((unsigned long *) (mPtr + mSize)) = size;
    mSize += sizeSize();
}

void FrameLib_Parameters::Serial::writeString(const char *str)
{
    unsigned long N = static_cast<unsigned long>(strlen(str) + 1);
    writeSize(N);
    std::copy(str, str + N, (char *) (mPtr + mSize));
    mSize += alignSize(N);
}

void FrameLib_Parameters::Serial::writeDoubles(const double *ptr, unsigned long N)
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

void FrameLib_Parameters::Serial::readSize(BytePointer *readPtr, unsigned long *size)
{
    *size = *reinterpret_cast<unsigned long *>(*readPtr);
    *readPtr += sizeSize();
}

void FrameLib_Parameters::Serial::readItem(BytePointer *readPtr, DataType type, BytePointer *data, unsigned long *size)
{
    readSize(readPtr, size);
    *data = *readPtr;
    *readPtr += alignSize(*size * (type == kVector ? sizeof(double) : sizeof(char)));
}

void FrameLib_Parameters::Serial::skipItem(BytePointer *readPtr, DataType type)
{
    unsigned long size;
    Serial::readSize(readPtr, &size);
    *readPtr += alignSize(size * (type == kVector ? sizeof(double) : sizeof(char)));
}

// Size Check

bool FrameLib_Parameters::Serial::checkSize(unsigned long writeSize)
{
    if (mSize + writeSize <= mMaxSize)
        return true;
    
    return false;
}

// ************************************************************************************** //

// AutoSerial Class (owning/resizing/allocating it's own memory using system routines - not suitable for audio thread use)

FrameLib_Parameters::AutoSerial::AutoSerial(AutoSerial&& other) : Serial(other.mPtr, other.mMaxSize)
{
    mSize = other.mSize;
    mNumTags = other.mNumTags;
    other.mPtr = nullptr;
}

FrameLib_Parameters::AutoSerial& FrameLib_Parameters::AutoSerial::operator=(AutoSerial&& other)
{
    mPtr = other.mPtr;
    mSize = other.mSize;
    mMaxSize = other.mMaxSize;
    mNumTags = other.mNumTags;
    other.mPtr = nullptr;
    
    return *this;
}

bool FrameLib_Parameters::AutoSerial::checkSize(unsigned long writeSize)
{
    unsigned long growSize;
    
    if (Serial::checkSize(writeSize))
        return true;
    
    // Calculate grow size
    
    growSize = (mSize + writeSize) - mMaxSize;
    growSize = growSize < minGrowSize ? minGrowSize : growSize;
    
    // Allocate required memory and copy old data before deleting the old pointer
    
    BytePointer newPtr = new Byte[mMaxSize + growSize];
    if (mPtr)
    {
        memcpy(newPtr, mPtr, mSize);
        delete[] mPtr;
    }
    
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
: mChanged(false)
, mFlags(0)
, mDefault(0.0)
, mMin(-std::numeric_limits<double>::infinity())
, mMax(std::numeric_limits<double>::infinity())
, mName(name)
{
    mArgumentIdx = argumentIdx;
}

void FrameLib_Parameters::Parameter::addEnumItem(unsigned long idx, const char *str, bool setAsDefault)
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

FrameLib_Parameters::SetError FrameLib_Parameters::Parameter::set(double *values, unsigned long N)
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
    if (checkMin(mMin))
        return checkMax(mMax) ? kNone : kMax;
    else
        return checkMax(mMax) ? kMin : kClip;
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

const double *FrameLib_Parameters::Parameter::getArray(unsigned long *size) const
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

void FrameLib_Parameters::Enum::addEnumItem(unsigned long idx, const char *str, bool setAsDefault)
{
    assert(idx == mItems.size() && "enum items added out of order");
    if (setAsDefault)
        mDefault = mValue = static_cast<unsigned long>(mItems.size());
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

FrameLib_Parameters::SetError FrameLib_Parameters::Enum::set(double *values, unsigned long N)
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

FrameLib_Parameters::SetError FrameLib_Parameters::Value::set(double *values, unsigned long N)
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

FrameLib_Parameters::Array::Array(const char *name, long argumentIdx, double defaultValue, unsigned long size)
: Parameter(name, argumentIdx), mSize(size), mVariableSize(false)
{
    mDefault = defaultValue;
    mItems.resize(size);
    
    for (unsigned long i = 0; i < mSize; i++)
        mItems[i] = defaultValue;
}

FrameLib_Parameters::Array::Array(const char *name, long argumentIdx, double defaultValue, unsigned long maxSize, unsigned long size)
: Parameter(name, argumentIdx), mVariableSize(true)
{
    mDefault = defaultValue;
    mItems.resize(maxSize);
    
    mSize = size < maxSize ? size : maxSize;
    
    for (unsigned long i = 0; i < mSize; i++)
        mItems[i] = defaultValue;
}

FrameLib_Parameters::SetError FrameLib_Parameters::Array::set(double *values, unsigned long N)
{
    N = std::min(N, static_cast<unsigned long>(mItems.size()));
    
    switch (getClipMode())
    {
        case kNone:
            for (unsigned long i = 0; i < N; i++)
                mItems[i] = values[i];
            break;
        case kMin:
            for (unsigned long i = 0; i < N; i++)
                mItems[i] = values[i] < mMin ? mMin : values[i];
            break;
        case kMax:
            for (unsigned long i = 0; i < N; i++)
                mItems[i] = values[i] > mMax ? mMax : values[i];
            break;
        case kClip:
            for (unsigned long i = 0; i < N; i++)
                mItems[i] = values[i] < mMin ? mMin : (values[i] > mMax ? mMax : values[i]);
            break;
    }
    
    if (!mVariableSize)
        for (unsigned long i = N; i < mItems.size(); i++)
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
    static const char *typeStringsDouble[] = {"double", "enum", "string", "fixed length double array", "variable length double array" };
    static const char *typeStringsInteger[] = {"int", "enum", "string", "fixed length int array", "variable length int array" };
    static const char *typeStringsBool[] = {"bool", "enum", "string", "fixed length bool array", "variable length bool array" };

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
    const int strBufSize = 64;
    char numericStr[strBufSize];

    if (type == kString)
        return "";
    else if (type == kEnum)
        return getItemString(idx, static_cast<unsigned long>(getDefault(idx)));
    else if (getNumericType(idx) == kNumericBool)
        return getDefault(idx) ? "true" : "false";
    
    snprintf(numericStr, strBufSize, "%lg", getDefault(idx));
    
    return numericStr;
}
