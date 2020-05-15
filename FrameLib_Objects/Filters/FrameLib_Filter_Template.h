
#ifndef FRAMELIB_FILTER_TEMPLATE_H
#define FRAMELIB_FILTER_TEMPLATE_H

#include <array>
#include <cmath>
#include <limits>
#include <tuple>

#include "FrameLib_DSP.h"

namespace FrameLib_Filters
{
    // Base class template for filter implementation objects
    // N.B. - set the number of coefficients only for filter coefficient calculators
    
    template <class T, size_t NumParams, size_t NumModes, size_t NumCoeffs = 0>
    struct FrameLib_FilterBase
    {
        // Construct a method suitable for the coefficient mode / number of coefficients
        
        template<std::size_t M, std::size_t N, std::size_t O, typename ...Args>
        struct make_mode_method : make_mode_method<M-1, N, O, Args..., double> {};
        
        template<std::size_t N, std::size_t O, typename ...Args>
        struct make_mode_method<0, N, O, Args...> : make_mode_method<0, N-1, O, Args..., double&> {};
        
        template<std::size_t O, typename ...Args>
        struct make_mode_method<0, 0, O, Args...> { using Method = void (T::*)(Args...); };
        
        template<typename ...Args>
        struct make_mode_method<0, 0, 0, Args...> { using Method = double (T::*)(double); };
        
        static constexpr size_t NumCoefficients = NumCoeffs;
        using ModeMethod = typename make_mode_method<NumParams + 1, NumCoeffs, NumCoeffs>::Method;
        
        // Convenience constant getters
        
        static constexpr double inf()   { return std::numeric_limits<double>::infinity(); }
        static constexpr double pi()    { return M_PI; }
        static constexpr double twopi() { return pi() * 2.0; }
        
        // Clip description for parameter constraints
        
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
        
        // Object description
        
        struct Description
        {
            constexpr Description(const char *description)
            : mDescription(description) {}
            
            const char *mDescription;
        };
        
        // Parameter description
        
        struct Param
        {
            constexpr Param(const char *name, const char *inputName, const char *info, double defaultValue, Clip clip)
            : mName(name), mInputName(inputName), mInfo(info), mDefaultValue(defaultValue), mClip(clip) {}
            
            const char *mName;
            const char *mInputName;
            const char *mInfo;
            const double mDefaultValue;
            const Clip mClip;
        };
        
        // Mode description
        
        struct Mode
        {
            constexpr Mode(const char *name, const char *outputName, const char *info, ModeMethod method)
            : mName(name), mOutputName(outputName), mInfo(info), mMethod(method) {}
            
            const char *mName;
            const char *mOutputName;
            const char *mInfo;
            const ModeMethod mMethod;
        };
        
        // Default operator for single mode filters
        
        void operator()(double) {}
        
        // Types for ease
        
        using ModeType = std::array<Mode, NumModes>;
        using ParamType = std::array<Param, NumParams>;
    };
};

// Main class template for FrameLib DSP objects

template <class T>
class FrameLib_Filter final : public FrameLib_Processor
{
    // Recursion helper
    
    template <class U, size_t Idx>
    struct StaticRecursion
    {
        template <typename... Args>
        void operator()(U& object, Args... args)
        {
            object.template modeSelect<Idx-1>(args...);
        }
    };
    
    template <class U>
    struct StaticRecursion<U, 0>
    {
        template <typename... Args>
        void operator()(U& object, Args... args) {}
    };

    template <size_t Idx>
    using ModeRecurse = StaticRecursion<FrameLib_Filter<T>, Idx>;
    
    // Index sequence type functionality without C++14
    
    template<std::size_t... Ints>
    struct indices {};
    
    template<std::size_t M, std::size_t... Is>
    struct make_indices : make_indices<M-1, M-1, Is...> {};
    
    template<std::size_t... Is>
    struct make_indices<0, Is...> : indices<Is...> {};
    
    // Input structure
    
    struct Input
    {
        const double* mInput = nullptr;
        unsigned long mSize = 0;
    };
    
    // Types and constexpr values
    
    static constexpr size_t NumParams = std::tuple_size<typename T::ParamType>::value;
    static constexpr size_t NumModes = std::tuple_size<typename T::ModeType>::value;
    static constexpr size_t NumCoefficients = T::NumCoefficients;
    static constexpr bool HasModes = NumModes > 1;
    static constexpr bool DoesCoefficients = T::NumCoefficients > 0;
    
    using Clip = typename T::Clip;
    using ParameterIndices = make_indices<NumParams>;
    using CoefficientIndices = make_indices<NumCoefficients>;
    using ModeIndices = make_indices<NumModes>;
    using ParamInputs = std::array<Input, NumParams>;
    using FilterOutputs = std::array<double *, DoesCoefficients ? NumCoefficients : NumModes>;
    using FilterParameters = std::array<double, NumParams + 1 + (DoesCoefficients ? 1 : 0)>;
    
    template <class U>
    using ForCoefficients = typename std::enable_if<(U::NumCoefficients > 0), int>::type;
    
    template <class U>
    using ForFilters = typename std::enable_if<U::NumCoefficients == 0, int>::type;
    
    static constexpr unsigned long ModeIndex = NumParams;
    static constexpr unsigned long MultiIndex = NumParams + 1;
    static constexpr unsigned long DynamicIndex = NumParams + (HasModes ? DoesCoefficients ? 1 : 2 : 0);
    static constexpr unsigned long ResetIndex = DynamicIndex + 1;
    static constexpr const typename T::ParamType& ParameterList = T::sParameters;
    static constexpr const typename T::ModeType& ModeList = T::sModes;
    static constexpr const typename T::Description& ObjectDescription = T::sDescription;
    
    // Parameter info
    
    struct ParameterInfo : public FrameLib_Parameters::Info
    {
        ParameterInfo()
        {
            for (unsigned long i = 0; i < NumParams; i++)
                add(ParameterList[i].mInfo);
            
            if (HasModes)
            {
                std::string mode("Sets the filter mode when multi mode is off:");
                
                for (unsigned long i = 0; i < NumModes; i++)
                {
                    mode.append(" ");
                    mode.append(ModeList[i].mName);
                    mode.append(" - ");
                    mode.append(ModeList[i].mInfo);
                    mode.append(".");
                }
                
                add(mode);
                
                if (!DoesCoefficients)
                    add("Sets multi mode (in which all filter modes are output separately).");
            }
            
            add("Sets dynamic mode (which creates inputs for each settings of the filter).");
            if (!DoesCoefficients)
                add("Sets whether filter memories are reset before processing a new frame.");
        }
    };
    
public:
    
    // Constructor
    
    FrameLib_Filter(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
    : FrameLib_Processor(context, proxy, getParamInfo())
    {
        for (unsigned long i = 0; i < NumParams; i++)
        {
            mParameters.addDouble(i, ParameterList[i].mName, ParameterList[i].mDefaultValue, i);
            const Clip &c = ParameterList[i].mClip;
            
            switch (c.getType())
            {
                case Clip::kNone:                                           break;
                case Clip::kMin:    mParameters.setMin(c.mMin);             break;
                case Clip::kMax:    mParameters.setMax(c.mMax);             break;
                case Clip::kClip:   mParameters.setClip(c.mMin, c.mMax);    break;
            }
        }
        
        if (HasModes)
        {
            mParameters.addEnum(ModeIndex, "mode", ModeIndex);
            for (unsigned long i = 0; i < NumModes; i++)
                mParameters.addEnumItem(i, ModeList[i].mName);
            
            if (!DoesCoefficients)
            {
                mParameters.addBool(MultiIndex, "multi", false);
                mParameters.setInstantiation();
            }
        }
        
        mParameters.addBool(DynamicIndex, "dynamic", false);
        mParameters.setInstantiation();
        
        if (!DoesCoefficients)
            mParameters.addBool(ResetIndex, "reset", true);
        
        mParameters.set(serialisedParameters);
        
        unsigned long numIns = mParameters.getBool(DynamicIndex) ? NumParams + 1 : 1;
        unsigned long numOuts = 0;
        
        if (DoesCoefficients)
            numOuts = NumCoefficients;
        else
            numOuts = (HasModes && mParameters.getBool(MultiIndex)) ? NumModes : 1;
        
        setIO(numIns, numOuts);
        
        if (mParameters.getBool(DynamicIndex))
        {
            for (unsigned long i = 1; i <= NumParams; i++)
                setInputMode(i, false, false, false);
        }
        
        addParameterInput();
    }
    
    // Info
    
    std::string objectInfo(bool verbose) override
    {
        const char *description = ObjectDescription.mDescription;
        
        if (verbose)
        {
            std::string info(description);
            
            info.append(": The size of the output is equal to the input. ");
            
            if (HasModes)
            {
                if (DoesCoefficients)
                    info.append("The filter mode is set by the mode parameter. ");
                else
                    info.append("The filter can be set to output a single mode at a time (set with the mode parameter) or all modes simulatanously (set with the multi parameter). ");
            }
            
            info.append("Filter settings may be updated either as parameters, or, when the dynamic parameter is set on, on a per sample basis via dedicated inputs. "
                        "When in dynamic mode the parameter values are used if an input is disconnected, or empty. "
                        "Thus you can mix dynamic and static settings.");
            
            if (!DoesCoefficients)
                info.append(" If you wish to process streams (rather than individual frames) then you can set the reset parameter off (which will not clear the filter memories between frames.");
            
            return info;
        }
        return formatInfo("#.", description);
    }
    
    std::string inputInfo(unsigned long idx, bool verbose) override
    {
        if (!idx)
            return "Input";
        
        if (idx >= getNumIns() - 1)
            return parameterInputInfo(verbose);
        
        return formatInfo("# - values per sample", "#", ParameterList[idx - 1].mInputName, verbose);
    }
    
    std::string outputInfo(unsigned long idx, bool verbose) override
    {
        if (!HasModes || !mParameters.getBool(MultiIndex))
            return "Output";
        
        return ModeList[idx].mOutputName;
    }
    
private:
    
    FrameLib_Parameters::Info *getParamInfo()
    {
        static ParameterInfo info;
        return &info;
    }
    
    // Helpers for updating the filter coefficients
    
    template <size_t Idx>
    double getFilterParameterValue(const ParamInputs& inputs, unsigned long i)
    {
        if (inputs[Idx].mInput && inputs[Idx].mSize)
        {
            const Clip &c = ParameterList[Idx].mClip;
            
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
    void updateCoefficients(const ParamInputs& inputs, unsigned long i, indices<Is...>)
    {
        const FilterParameters parameters{ {getFilterParameterValue<Is>(inputs, i)..., mSamplingRate} };
        
        if (parameters != mFilterParameters)
        {
            mFilterParameters = parameters;
            mFilter.updateCoefficients(std::get<Is>(parameters)..., mSamplingRate);
        }
    }
    
    template <size_t Idx, size_t... Is, size_t... Js>
    void updateCoefficients(double **outputs, const ParamInputs& inputs, unsigned long i, indices<Is...>, indices<Js...>)
    {
        const FilterParameters parameters{ {getFilterParameterValue<Is>(inputs, i)..., Idx, mSamplingRate} };
        
        if (!i || parameters != mFilterParameters)
        {
            mFilterParameters = parameters;
            (mFilter.*ModeList[Idx].mMethod)(std::get<Is>(parameters)..., mSamplingRate, outputs[Js][i]...);
        }
        else
            (void) std::initializer_list<double>{ outputs[Js][i] = outputs[Js][i - 1]... };
    }
    
    // Output calculations
    
    template <size_t Idx>
    void calculateOutput(double *output, const double *input, unsigned long i)
    {
        output[i] = (mFilter.*ModeList[Idx].mMethod)(input[i]);
    }
    
    template <size_t Idx>
    void calculateOutput(double **outputs, const double *input, unsigned long i)
    {
        outputs[Idx][i] = (mFilter.*ModeList[Idx].mMethod)(input[i]);
    }
    
    // Main DSP loops
    
    template <typename O, size_t... Is>
    void processLoops(O outputs, const double *input, const ParamInputs& paramIns, unsigned long size, bool dynamic, indices<Is...>)
    {
        if (dynamic)
        {
            for (unsigned long i = 0; i < size; i++)
            {
                updateCoefficients(paramIns, i, ParameterIndices());
                mFilter(input[i]);
                (void) std::initializer_list<int>{ (calculateOutput<Is>(outputs, input, i), 0)... };
            }
        }
        else
        {
            updateCoefficients(paramIns, 0, ParameterIndices());
            
            for (unsigned long i = 0; i < size; i++)
            {
                mFilter(input[i]);
                (void) std::initializer_list<int>{ (calculateOutput<Is>(outputs, input, i), 0)... };
            }
        }
    }
    
    // Mode selection
    
    template <size_t Idx>
    void modeSelect(double *output, const double *input, const ParamInputs& paramIns, unsigned long size, size_t mode, bool dynamic)
    {
        if (Idx == mode)
            processLoops(output, input, paramIns, size, dynamic, indices<Idx>());
        else
            ModeRecurse<Idx>()(*this, output, input, paramIns, size, mode, dynamic);
    }
    
    template <size_t Idx>
    void modeSelect(double **outputs, const ParamInputs& paramIns, unsigned long size, size_t mode)
    {
        if (Idx == mode)
        {
            for (unsigned long i = 0; i < size; i++)
                updateCoefficients<Idx>(outputs, paramIns, size, ParameterIndices(), CoefficientIndices());
        }
        else
            ModeRecurse<Idx>()(*this, outputs, paramIns, size, mode);
    }
    
    // Main calculation
    
    template <class U, ForFilters<U> = 0>
    void calculate(FilterOutputs& multiOuts, const double *input, ParamInputs& paramIns, unsigned long size, size_t mode, bool dynamic)
    {
        // Do (optional) reset and DSP
        
        if (mParameters.getBool(ResetIndex))
            mFilter.reset();
        
        if (HasModes ? mParameters.getBool(MultiIndex) : false)
            processLoops(multiOuts.data(), input, paramIns, size, dynamic, ModeIndices());
        else
            modeSelect<NumModes-1>(multiOuts[0], input, paramIns, size, mode, dynamic);
    }
    
    template <class U, ForCoefficients<U> = 0>
    void calculate(FilterOutputs& multiOuts, const double *input, ParamInputs& paramIns, unsigned long size, size_t mode, bool dynamic)
    {
        modeSelect<NumModes-1>(multiOuts.data(), paramIns, size, mode);
    }
    
    // Process
    
    void process() override
    {
        ParamInputs paramIns;
        FilterOutputs multiOuts;
        
        // Get Input
        
        unsigned long sizeIn, sizeOut;
        unsigned long numOuts = getNumOuts();
        const double *input = getInput(0, &sizeIn);
        
        // Setup outputs
        
        for (unsigned long i = 0; i < numOuts; i++)
            requestOutputSize(i, sizeIn);
        
        allocateOutputs();
        
        for (unsigned long i = 0; i < numOuts; i++)
            multiOuts[i] = getOutput(i, &sizeOut);
        
        size_t mode = HasModes ? static_cast<size_t>(mParameters.getInt(ModeIndex)) : 0;
        bool dynamic = false;
        
        // Read in the inputs and check if they change
        
        if (mParameters.getBool(DynamicIndex))
        {
            for (unsigned long i = 0; i < NumParams; i++)
            {
                paramIns[i].mInput = getInput(i + 1, &paramIns[i].mSize);
                dynamic = dynamic || paramIns[i].mSize > 1;
            }
        }
        
        // DSP
        
        calculate<T>(multiOuts, input, paramIns, sizeOut, mode, dynamic);
    }
    
    // Data
    
    T mFilter;
    FilterParameters mFilterParameters;
};

#endif
