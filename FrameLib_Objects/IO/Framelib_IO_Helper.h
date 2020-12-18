
#ifndef FRAMELIB_IO_HELPERS_H
#define FRAMELIB_IO_HELPERS_H

#include "FrameLib_DSP.h"
#include "../../FrameLib_Dependencies/TableReader.hpp"

class FrameLib_IO_Helper
{
public:
    
    enum Units { kSamples, kMS, kSeconds };

    FrameLib_IO_Helper(FrameLib_DSP& owner) : mOwner(owner) {}

    template <class T>
    static void interpolate(T fetch, double *output, unsigned long size, double offset, InterpType interpType)
    {
        for (unsigned long i = 0; i < size; i++, offset += 1.0)
            output[i] = offset;
        
        table_read(fetch, output, output, size, 1.0, interpType);
    }

    template <class T>
    static void interpolate_zeropad(T fetch, double *output, unsigned long size, double offset, InterpType interpType)
    {
        interpolate(table_fetcher_zeropad<T>(fetch), output, size, offset, interpType);
    }
    
    size_t limitBufferSize(size_t size, double samplingRate)
    {
        if ((size / samplingRate) > 1000.0)
        {
            mOwner.getReporter()(kErrorObject, mOwner.getProxy(), "buffer size is limited to 10,000 seconds");
            return static_cast<size_t>(round(10000 * samplingRate));
        }
        
        return size;
    }
    
    bool checkFrameTime(FrameLib_TimeFormat frameTime, FrameLib_TimeFormat blockStartTime, FrameLib_TimeFormat blockEndTime)
    {
        return frameTime >= blockStartTime && frameTime < blockEndTime;
    }
    
    bool checkOutput(unsigned long size, FrameLib_TimeFormat delay, unsigned long bufferSize, unsigned long extra)
    {
        extra += static_cast<unsigned long>(delay.intVal()) + (delay.fracVal() ? 1UL : 0UL);
        
        return (size && size <= (bufferSize - extra));
    }
    
    double getBufferSizeDefault(const FrameLib_Parameters::Serial *serialisedParameters, long samples, long seconds)
    {
        FrameLib_Parameters parameters(mOwner.getReporter(), nullptr, nullptr);
        
        parameters.setErrorReportingEnabled(false);
        
        unsigned long unitsIdx = 0;
        
        parameters.addEnum(unitsIdx, "units");
        parameters.addEnumItem(kSamples, "samples");
        parameters.addEnumItem(kMS, "ms");
        parameters.addEnumItem(kSeconds, "seconds");
        
        parameters.set(serialisedParameters);
        
        Units units = parameters.getEnum<Units>(unitsIdx);

        switch (units)
        {
            case kSamples:      return samples;
            case kMS:           return static_cast<double>(seconds) / 1000.0;
            case kSeconds:      return seconds;
        }
    }
    
private:
    
    FrameLib_DSP& mOwner;
};

#endif 
