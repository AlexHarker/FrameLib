
#ifndef FRAMELIB_FILTER_TEMPLATE_H
#define FRAMELIB_FILTER_TEMPLATE_H

#include <array>
#include <cmath>
#include <limits>
#include <tuple>

#include "FrameLib_DSP.h"

template <class T, int NumModes, int NumParams>
struct FrameLib_FilterBase
{
    static constexpr double infinity() { return std::numeric_limits<double>::infinity(); }
    static constexpr double pi()       { return M_PI; }
    static constexpr double twopi()    { return 6.28318530717958647692; }
    
    struct Constraint
    {
        enum Type { kNone, kMin, kMax, kClip };
        
        Constraint(double min, double max) : mMin(min), mMax(max) {}
        
        bool hasMin() const { return mMin != -infinity(); }
        bool hasMax() const { return mMax != infinity(); }
        
        Type getType() const
        {
            return hasMin() ? (hasMax() ? kClip : kMin) : (hasMax() ? kMax : kNone);
        }
        
        double mMin;
        double mMax;
    };
    
    struct Clip : Constraint{ Clip(double min, double max) : Constraint(min, max) {} };
    struct Min : Constraint { Min(double min) : Constraint(min, infinity()) {} };
    struct Max : Constraint { Max(double max) : Constraint(-infinity(), max) {} };
    struct None : Constraint { None() : Constraint(-infinity(), infinity()) {} };
    
    struct Param
    {
        Param() : mName(""), mDefaultValue(0.0), mConstraint(None()) {}
        
        Param(const char *name, double defaultValue, Constraint constraint)
        : mName(name), mDefaultValue(defaultValue), mConstraint(constraint) {}
        
        const std::string mName;
        const double mDefaultValue;
        const Constraint mConstraint;
    };
    
    struct Mode
    {
        typedef double (T::*Method)(double);
        
        Mode() : mName(""), mMethod(nullptr) {}
        
        Mode(const char *name, Method method)
        : mName(name), mMethod(method) {}
        
        const std::string mName;
        const Method mMethod;
    };
    
    void operator()(double) {};
    
    using ModeType = std::array<Mode, NumModes>;
    using ParamType = std::array<Param, NumParams>;
};

template <class T, size_t I>
struct StaticRecursion
{
    template <typename... Args>
    void operator()(T& object, Args... args)
    {
        object.template processLoops<I-1>(args...);
    }
};

template <class T>
struct StaticRecursion<T, 0>
{
    template <typename... Args>
    void operator()(T& object, Args... args) {}
};

template <class T>
class FrameLib_Filter final : public FrameLib_Processor
{
    template <class, size_t>
    friend struct StaticRecursion;
    
    // Index sequence type functionality without C++14
    
    template<std::size_t... Ints>
    struct indices {};
    
    template<std::size_t M, std::size_t... Is>
    struct make_indices : make_indices<M-1, M-1, Is...> {};
    
    template<std::size_t... Is>
    struct make_indices<0, Is...> : indices<Is...> {};
    
    static constexpr size_t N = std::tuple_size<typename T::ParamType>::value;
    static constexpr size_t M = std::tuple_size<typename T::ModeType>::value;

    // Input structure
    
    struct Input
    {
        const double* mInput = nullptr;
        unsigned long mSize = 0;
    };
    
    using Param = typename T::Param;
    using Mode = typename T::Mode;
    using Constraint = typename T::Constraint;
    using ParameterIndices = make_indices<N>;
    using FilterInputs = std::array<Input, N>;
    using FilterParameters = std::array<double, N>;
    
    static constexpr unsigned long ModeIndex = N;
    static constexpr unsigned long ResetIndex = N + (M > 1 ? 1 : 0);
    static constexpr unsigned long DynamicIndex = ResetIndex + 1;
    static constexpr typename T::ParamType& ParamDescription = T::sParameters;
    static constexpr typename T::ModeType& ModeDescription = T::sModes;
    
    // Parameter info
    
    struct ParameterInfo : public FrameLib_Parameters::Info
    {
        ParameterInfo() {}
        
        // FIX
    };
    
public:
    
    // Constructor
    
    FrameLib_Filter(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
    : FrameLib_Processor(context, proxy, getParamInfo(), 1, 1)
    {
        for (unsigned long i = 0; i < N; i++)
        {
            mParameters.addDouble(i, ParamDescription[i].mName.c_str(), ParamDescription[i].mDefaultValue, i);
            const Constraint &c = ParamDescription[i].mConstraint;
            
            switch (c.getType())
            {
                case Constraint::kNone:                                         break;
                case Constraint::kMin:  mParameters.setMin(c.mMin);             break;
                case Constraint::kMax:  mParameters.setMax(c.mMax);             break;
                case Constraint::kClip: mParameters.setClip(c.mMin, c.mMax);    break;
            }
        }
        
        if (M > 1)
        {
            mParameters.addEnum(ModeIndex, "mode");
            for (unsigned long i = 0; i < M; i++)
                mParameters.addEnumItem(i, ModeDescription[i].mName.c_str());
        }
        
        mParameters.addBool(ResetIndex, "reset", true);
        
        mParameters.addBool(DynamicIndex, "dynamic", false);
        mParameters.setInstantiation();
        
        mParameters.set(serialisedParameters);

        if (mParameters.getBool(DynamicIndex))
            setIO(N + 1, 1);
        
        addParameterInput();
    }
    
    // Info
    
    std::string objectInfo(bool verbose) override
    {
        // FIX ??
        return std::string();
    }
    
    std::string inputInfo(unsigned long idx, bool verbose) override
    {
        // FIX ??
        return std::string();
    }
    
    std::string outputInfo(unsigned long idx, bool verbose) override
    {
        return "Output";
    }
    
private:

    FrameLib_Parameters::Info *getParamInfo()
    {
        static ParameterInfo info;
        
        return &info;
    }
    
    // Helpers for updating the filter coefficients
    
    template <size_t Idx>
    double getFilterParameterValue(const FilterInputs& inputs, unsigned long i)
    {
        if (inputs[Idx].mInput && inputs[Idx].mSize)
            return inputs[Idx].mInput[std::min(i, inputs[Idx].mSize - 1)];
        
        return mParameters.getValue(Idx);
    }
    
    template <size_t... Is>
    void updateCoefficients(const FilterInputs& inputs, unsigned long i, indices<Is...>)
    {
        const FilterParameters parameters{ {getFilterParameterValue<Is>(inputs, i)...} };
        
        if (parameters != mFilterParameters)
        {
            mFilterParameters = parameters;
            mFilter.updateCoefficients(std::get<Is>(parameters)..., mSamplingRate);
        }
    }
    
    template <size_t I>
    void processLoops(double *output, const double *input, const FilterInputs& paramIns,
                      unsigned long size, size_t mode, bool dynamic)
    {
        if (I != mode)
        {
            StaticRecursion<FrameLib_Filter<T>, I>()(*this, output, input, paramIns, size, mode, dynamic);
            return;
        }
        
        if (!dynamic)
        {
            updateCoefficients(paramIns, 0, ParameterIndices());

            for (unsigned long i = 0; i < size; i++)
            {
                mFilter(input[i]);
                output[i] = (mFilter.*ModeDescription[I].mMethod)(input[i]);
            }
        }
        else
        {
            for (unsigned long i = 0; i < size; i++)
            {
                updateCoefficients(paramIns, i, ParameterIndices());
                mFilter(input[i]);
                output[i] = (mFilter.*ModeDescription[I].mMethod)(input[i]);
            }
        }
    }
    
    // Process
    
    void process() override
    {
        // Get Input
        
        unsigned long sizeIn, sizeOut;
        const double *input = getInput(0, &sizeIn);
        FilterInputs paramIns;
        
        requestOutputSize(0, sizeIn);
        allocateOutputs();
        
        double *output = getOutput(0, &sizeOut);
        
        size_t mode = M > 1 ? static_cast<size_t>(mParameters.getInt(ModeIndex)) : 0;
        bool dynamic = false;
    
        // Read in the inputs and check if they change
        
        if (mParameters.getBool(DynamicIndex))
        {
            for (unsigned long i = 0; i < N; i++)
            {
                paramIns[i].mInput = getInput(i + 1, &paramIns[i].mSize);

                if (paramIns[i].mSize > 1)
                    dynamic = true;
            }
        }
        
        // Do (optional) reset and DSP
        
        if (mParameters.getBool(ResetIndex))
            mFilter.reset();
        
        processLoops<M-1>(output, input, paramIns, sizeOut, mode, dynamic);
    }
    
    // Data
    
    FilterParameters mFilterParameters;
    T mFilter;
};

#endif
