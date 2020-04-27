
#ifndef FRAMELIB_FILTER_TEMPLATE_H
#define FRAMELIB_FILTER_TEMPLATE_H

#include <array>
#include <cmath>
#include <limits>
#include <tuple>

#include "FrameLib_DSP.h"

template <class T, int NumParams, int NumModes>
struct FrameLib_FilterBase
{
    static constexpr double inf()   { return std::numeric_limits<double>::infinity(); }
    static constexpr double pi()    { return M_PI; }
    static constexpr double twopi() { return pi() * 2.0; }
    
    struct Clip
    {
        enum Type { kNone, kMin, kMax, kClip };
        
        constexpr Clip(double min, double max) : mMin(min), mMax(max) {}

        Type getType() const
        {
            return mMin != -inf() ? (mMax != inf() ? kClip : kMin) : (mMax != inf() ? kMax : kNone);
        }
        
        double mMin;
        double mMax;
    };
    
    struct Min : Clip   { constexpr Min(double min) : Clip(min, inf()) {} };
    struct Max : Clip   { constexpr Max(double max) : Clip(-inf(), max) {} };
    struct None : Clip  { constexpr None() : Clip(-inf(), inf()) {} };
    
    struct Param
    {        
        constexpr Param(const char *name, double defaultValue, Clip clip)
        : mName(name), mDefaultValue(defaultValue), mClip(clip) {}
        
        const char *mName;
        const double mDefaultValue;
        const Clip mClip;
    };
    
    struct Mode
    {
        typedef double (T::*Method)(double);
        
        constexpr Mode(const char *name, Method method)
        : mName(name), mMethod(method) {}
        
        const char *mName;
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
        object.template modeSelect<I-1>(args...);
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
    
    using Clip = typename T::Clip;
    using ParameterIndices = make_indices<N>;
    using ModeIndices = make_indices<M>;
    using FilterInputs = std::array<Input, N>;
    using FilterOutputs = std::array<double *, M>;
    using FilterParameters = std::array<double, N>;

    static constexpr unsigned long ModeIndex = N;
    static constexpr unsigned long MultiIndex = N + 1;
    static constexpr unsigned long ResetIndex = N + (M > 1 ? 2 : 0);
    static constexpr unsigned long DynamicIndex = ResetIndex + 1;
    static constexpr const typename T::ParamType& ParamDescription = T::sParameters;
    static constexpr const typename T::ModeType& ModeDescription = T::sModes;
    
    // Parameter info
    
    struct ParameterInfo : public FrameLib_Parameters::Info
    {
        ParameterInfo() {}
        
        // FIX
    };
    
public:
    
    // Constructor
    
    FrameLib_Filter(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
    : FrameLib_Processor(context, proxy, getParamInfo())
    {
        for (unsigned long i = 0; i < N; i++)
        {
            mParameters.addDouble(i, ParamDescription[i].mName, ParamDescription[i].mDefaultValue, i);
            const Clip &c = ParamDescription[i].mClip;
            
            switch (c.getType())
            {
                case Clip::kNone:                                           break;
                case Clip::kMin:    mParameters.setMin(c.mMin);             break;
                case Clip::kMax:    mParameters.setMax(c.mMax);             break;
                case Clip::kClip:   mParameters.setClip(c.mMin, c.mMax);    break;
            }
        }
        
        if (M > 1)
        {
            mParameters.addEnum(ModeIndex, "mode");
            for (unsigned long i = 0; i < M; i++)
                mParameters.addEnumItem(i, ModeDescription[i].mName);
            
            mParameters.addBool(MultiIndex, "multi", false);
            mParameters.setInstantiation();
        }
        
        mParameters.addBool(ResetIndex, "reset", true);
        
        mParameters.addBool(DynamicIndex, "dynamic", false);
        mParameters.setInstantiation();
        
        mParameters.set(serialisedParameters);

        unsigned long numIns = mParameters.getBool(DynamicIndex) ? N + 1 : 1;
        unsigned long numOuts = (M > 1 && mParameters.getBool(MultiIndex)) ? M : 1;

        setIO(numIns, numOuts);
        
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
        {
            const Clip &c = ParamDescription[Idx].mClip;

            const double val = inputs[Idx].mInput[std::min(i, inputs[Idx].mSize - 1)];
            
            switch (c.getType())
            {
                case Clip::kNone:   return val;
                case Clip::kMin:    return std::max(val, c.mMin);
                case Clip::kMax:    return std::min(val, c.mMax);
                case Clip::kClip:   return std::min(std::max(val, c.mMin), c.mMax);
            }
        }
        
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
    void calculateOutput(double *output, const double *input, unsigned long i)
    {
        output[i] = (mFilter.*ModeDescription[I].mMethod)(input[i]);
    }
    
    template <size_t I>
    void calculateOutput(double **outputs, const double *input, unsigned long i)
    {
        outputs[I][i] = (mFilter.*ModeDescription[I].mMethod)(input[i]);
    }
    
    template <typename O, size_t... Is>
    void processLoops(O outputs, const double *input, const FilterInputs& paramIns, unsigned long size, bool dynamic, indices<Is...>)
    {
        if (!dynamic)
        {
            updateCoefficients(paramIns, 0, ParameterIndices());
            
            for (unsigned long i = 0; i < size; i++)
            {
                mFilter(input[i]);
                (void) std::initializer_list<int>{ (calculateOutput<Is>(outputs, input, i), 0)... };
            }
        }
        else
        {
            for (unsigned long i = 0; i < size; i++)
            {
                updateCoefficients(paramIns, i, ParameterIndices());
                mFilter(input[i]);
                (void) std::initializer_list<int>{ (calculateOutput<Is>(outputs, input, i), 0)... };
            }
        }
    }
    
    template <size_t I>
    void modeSelect(double *output, const double *input, const FilterInputs& paramIns, unsigned long size, size_t mode, bool dynamic)
    {
        if (I != mode)
            StaticRecursion<FrameLib_Filter<T>, I>()(*this, output, input, paramIns, size, mode, dynamic);
        else
            processLoops(output, input, paramIns, size, dynamic, indices<I>());
    }
    
    // Process
    
    void process() override
    {
        // Get Input
        
        unsigned long sizeIn, sizeOut;
        unsigned long numOuts = getNumOuts();
        const double *input = getInput(0, &sizeIn);
        FilterInputs paramIns;
        FilterOutputs multiOuts;
        
        for (unsigned long i = 0; i < numOuts; i++)
            requestOutputSize(i, sizeIn);
        
        allocateOutputs();
        
        for (unsigned long i = 0; i < numOuts; i++)
            multiOuts[i] = getOutput(i, &sizeOut);
        
        size_t mode = M > 1 ? static_cast<size_t>(mParameters.getInt(ModeIndex)) : 0;
        bool dynamic = false;
    
        // Read in the inputs and check if they change
        
        if (mParameters.getBool(DynamicIndex))
        {
            for (unsigned long i = 0; i < N; i++)
            {
                paramIns[i].mInput = getInput(i + 1, &paramIns[i].mSize);
                dynamic = dynamic || paramIns[i].mSize > 1;
            }
        }
        
        // Do (optional) reset and DSP
        
        if (mParameters.getBool(ResetIndex))
            mFilter.reset();

        if (M > 1 ? mParameters.getBool(MultiIndex) : false)
            processLoops(multiOuts.data(), input, paramIns, sizeOut, dynamic, ModeIndices());
        else
            modeSelect<M-1>(multiOuts[0], input, paramIns, sizeOut, mode, dynamic);
    }
    
    // Data
    
    T mFilter;
    FilterParameters mFilterParameters;
};

#endif
