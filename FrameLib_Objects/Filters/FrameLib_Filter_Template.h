
#ifndef FRAMELIB_FILTER_TEMPLATE_H
#define FRAMELIB_FILTER_TEMPLATE_H

#include <array>
#include <cmath>
#include <limits>
#include <tuple>
#include <type_traits>

#include "FrameLib_DSP.h"

namespace FrameLib_Filters
{
    // Base class template for filter implementations  (N.B. - set NumCoeffs for coefficient calculators)
    
    template <class T, size_t NumParams, size_t NumModes, size_t NumCoeffs, bool DoesMulti>
    struct Base
    {
        // Construct a method suitable for the coefficient mode / number of coefficients
        
        template<std::size_t M, std::size_t N, std::size_t O, typename ...Args>
        struct MakeModeMethod : MakeModeMethod<M-1, N, O, Args..., double> {};
        
        template<std::size_t N, std::size_t O, typename ...Args>
        struct MakeModeMethod<0, N, O, Args...> : MakeModeMethod<0, N-1, O, Args..., double&> {};
        
        template<std::size_t O, typename ...Args>
        struct MakeModeMethod<0, 0, O, Args...> { using Method = void (T::*)(Args...); };
        
        template<typename ...Args>
        struct MakeModeMethod<0, 0, 0, Args...> { using Method = double (T::*)(double); };
        
        using ModeMethod = typename MakeModeMethod<NumParams + 1, NumCoeffs, NumCoeffs>::Method;
        
        // Mode description template and specialisation
        
        template <class U, bool Coeff>
        struct ModeTemplate
        {
            constexpr ModeTemplate(const char *name, const char *info, ModeMethod method)
            : mName(name), mInfo(info), mMethod(method) {}
            
            const char *mName;
            const char *mInfo;
            const ModeMethod mMethod;
        };
        
        template <class U>
        struct ModeTemplate <U, false>
        {
            constexpr ModeTemplate(const char *name, const char *outputName, const char *info, ModeMethod method)
            : mName(name), mOutputName(outputName), mInfo(info), mMethod(method) {}
            
            const char *mName;
            const char *mOutputName;
            const char *mInfo;
            const ModeMethod mMethod;
        };
        
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
        
        // Coefficient description
        
        struct Coeff
        {
            constexpr Coeff(const char *name, const char *outputName)
            : mName(name), mOutputName(outputName) {}
            
            const char *mName;
            const char *mOutputName;
        };
        
        // Mode description
        
        using Mode = ModeTemplate<T, NumCoeffs != 0>;
        
        // Default operator for single mode filters
        
        void operator()(double) {}
        
        // Types
        
        using ModeType = std::array<Mode, NumModes>;
        using ParamType = std::array<Param, NumParams>;
        using CoeffType = std::array<Coeff, NumCoeffs>;
        using MultiType = std::integral_constant<bool, DoesMulti>;
    };
    
    template <class T, size_t NumParams, size_t NumModes, bool DoesMulti>
    using Filter = Base<T, NumParams, NumModes, 0, DoesMulti>;
    
    template <class T, size_t NumParams, size_t NumModes, size_t NumCoeff>
    using Coefficients = Base<T, NumParams, NumModes, NumCoeff, false>;
}

// Main class template for FrameLib DSP objects

template <class T>
class FrameLib_Filter final : public FrameLib_Processor
{
    // Mode recursion helper
    
    template <class U, size_t Idx>
    struct ModeSelector
    {
        template <typename... Args>
        void operator()(U& object, FrameLib_Filter& obj, size_t mode, Args... args)
        {
            object.template modeSelect<Idx-1>(obj, mode, args...);
        }
    };
    
    template <class U>
    struct ModeSelector<U, 0>
    {
        template <typename... Args>
        void operator()(U& object, FrameLib_Filter& obj, size_t mode, Args... args) {}
    };
    
    // Index sequence type functionality without C++14
    
    template<std::size_t... Ints>
    struct Indices {};
    
    template<std::size_t M, std::size_t... Is>
    struct MakeIndices : MakeIndices<M-1, M-1, Is...> {};
    
    template<std::size_t... Is>
    struct MakeIndices<0, Is...> : Indices<Is...> {};
    
    // Input structure
    
    struct Input
    {
        const double* mInput = nullptr;
        unsigned long mSize = 0;
    };
    
    // Parameter Mode
    
    enum ParamMode { kStatic, kDynamic, kTagged };
    
    // Types and constexpr values
    
    static constexpr size_t NumParams = std::tuple_size<typename T::ParamType>::value;
    static constexpr size_t NumModes = std::tuple_size<typename T::ModeType>::value;
    static constexpr size_t NumCoeffs = std::tuple_size<typename T::CoeffType>::value;
    
    static constexpr bool DoesModes = NumModes > 1;
    static constexpr bool DoesCoefficients = NumCoeffs != 0;
    static constexpr bool DoesMulti = T::MultiType::value && DoesModes && !DoesCoefficients;
    
    static constexpr unsigned long ModeIndex = NumParams;
    static constexpr unsigned long MultiIndex = NumParams + 1;
    static constexpr unsigned long ParamModeIndex = NumParams + (DoesModes ? DoesMulti ? 2 : 1 : 0);
    static constexpr unsigned long ResetIndex = ParamModeIndex + 1;
    
    static constexpr const typename T::ParamType& ParamList = T::sParameters;
    static constexpr const typename T::ModeType& ModeList = T::sModes;
    
    using Clip = typename T::Clip;
    using ParamInputs = std::array<Input, NumParams>;
    using FilterParams = std::array<double, NumParams + 1 + (DoesCoefficients ? 1 : 0)>;
    using ParamIndices = MakeIndices<NumParams>;
    using CoeffIndices = MakeIndices<NumCoeffs>;
    using ModeIndices = MakeIndices<NumModes>;
    
    // Implementation class for a filter (specialised when Coeff is true below)
    
    template <class U, bool Coeff>
    struct Implementation
    {
        template <size_t Idx>
        using ModeRecurse = ModeSelector<Implementation, Idx>;

        void setup(FrameLib_Filter& obj) {}
        
        std::string outputInfo(FrameLib_Filter& obj, unsigned long idx, bool verbose)
        {
            if (!obj.isMulti())
                return "Output";
            
            return ModeList[idx].mOutputName;
        }
        
        template <size_t... Is>
        void updateCoefficients(FrameLib_Filter& obj, ParamInputs& inputs, unsigned long i, Indices<Is...>)
        {
            const FilterParams parameters { obj.getFilterParamVal<Is>(inputs, i)..., obj.mSamplingRate };
            
            if (parameters != mFilterParameters)
            {
                mFilterParameters = parameters;
                mFilter.updateCoefficients(std::get<Is>(parameters)..., obj.mSamplingRate);
            }
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
        void process(FrameLib_Filter& obj, O outputs, const double *input, unsigned long size, Indices<Is...>)
        {
            ParamInputs paramIns;
            bool dynamic = false;

            // Read in the parameter inputs and check if they change
            
            if (obj.isDynamic())
            {
                for (unsigned long i = 0; i < NumParams; i++)
                {
                    paramIns[i].mInput = obj.getInput(i + 1, &paramIns[i].mSize);
                    dynamic = dynamic || paramIns[i].mSize > 1;
                }
            }
            
            if (dynamic)
            {
                for (unsigned long i = 0; i < size; i++)
                {
                    updateCoefficients(obj, paramIns, i, ParamIndices());
                    mFilter(input[i]);
                    (void) std::initializer_list<int>{ (calculateOutput<Is>(outputs, input, i), 0)... };
                }
            }
            else
            {
                updateCoefficients(obj, paramIns, 0, ParamIndices());
                
                for (unsigned long i = 0; i < size; i++)
                {
                    mFilter(input[i]);
                    (void) std::initializer_list<int>{ (calculateOutput<Is>(outputs, input, i), 0)... };
                }
            }
        }
        
        // Mode selection
        
        template <size_t Idx, typename...Args>
        void modeSelect(FrameLib_Filter& obj, size_t mode, Args...args)
        {
            if (Idx == mode)
                process(obj, args..., Indices<Idx>());
            else
                ModeRecurse<Idx>()(*this, obj, mode, args...);
        }
        
        // Process
        
        void process(FrameLib_Filter& obj)
        {
            double *outputs[NumModes];
            
            // Get Input
            
            unsigned long sizeIn, sizeOut;
            unsigned long numOuts = obj.getNumOuts();
            const double *input = obj.getInput(0, &sizeIn);
            
            // Setup outputs
            
            for (unsigned long i = 0; i < numOuts; i++)
                obj.requestOutputSize(i, sizeIn);
            
            obj.allocateOutputs();
            
            for (unsigned long i = 0; i < numOuts; i++)
                outputs[i] = obj.getOutput(i, &sizeOut);
            
            // Do (optional) reset and DSP
            
            if (obj.getReset())
                mFilter.reset();
            
            if (obj.isMulti())
                process(obj, outputs, input, sizeOut, ModeIndices());
            else
                modeSelect<NumModes-1>(obj, obj.getMode(), outputs[0], input, sizeOut);
        }
        
    private:
    
        T mFilter;
        FilterParams mFilterParameters;
    };
    
    // Implementation class for a coefficient calculator (specialisation)
    
    template <class U>
    struct Implementation<U, true>
    {
        template <size_t Idx>
        using ModeRecurse = ModeSelector<Implementation, Idx>;
        
        void setup(FrameLib_Filter& obj)
        {
            mValueOutput = !obj.isTagged();
            
            for (unsigned long i = 0; i < NumCoeffs; i++)
                mTaggedSize += FrameLib_Parameters::Serial::calcSize(T::sCoefficients[i].mName, 1);
            
        }
                               
        std::string outputInfo(FrameLib_Filter& obj, unsigned long idx, bool verbose)
        {
            return obj.isTagged() ? "Tagged Coefficients" : T::sCoefficients[idx].mOutputName;
        }
        
        template <size_t Idx, typename V, size_t... Is, size_t... Js>
        void modeSelect(FrameLib_Filter& obj, size_t mode, V outputs, ParamInputs& inputs, unsigned long size, Indices<Is...>, Indices<Js...>)
        {
            if (Idx == mode)
            {
                for (unsigned long i = 0; i < size; i++)
                {
                    const double& sr = obj.mSamplingRate;
                    const FilterParams parameters { obj.getFilterParamVal<Is>(inputs, i)..., Idx, sr };
                    
                    if (!i || parameters != mFilterParameters)
                    {
                        mFilterParameters = parameters;
                        (mFilter.*ModeList[Idx].mMethod)(std::get<Is>(parameters)..., sr, outputs[Js][i]...);
                    }
                    else
                        (void) std::initializer_list<double>{ outputs[Js][i] = outputs[Js][i - 1]... };
                }
            }
            else
                ModeRecurse<Idx>()(*this, obj, mode, outputs, inputs, size, ParamIndices(), CoeffIndices());
        }
        
        // Process
        
        void process(FrameLib_Filter& obj)
        {
            ParamInputs inputs;
            
            // Get Input
            
            unsigned long sizeIn = 1;
            unsigned long sizeOut;
            unsigned long numOuts = obj.getNumOuts();
            
            // Read in the parameter inputs and check if they change
            
            if (obj.isDynamic())
            {
                for (unsigned long i = 0; i < NumParams; i++)
                {
                    inputs[i].mInput = obj.getInput(i + 1, &inputs[i].mSize);
                    sizeIn = std::max(sizeIn, inputs[i].mSize);
                }
            }
            
            // Setup outputs and do DSP
            
            if (mValueOutput)
            {
                double *outputs[NumCoeffs];

                for (unsigned long i = 0; i < numOuts; i++)
                    obj.requestOutputSize(i, sizeIn);
                
                obj.allocateOutputs();
                
                for (unsigned long i = 0; i < numOuts; i++)
                    outputs[i] = obj.getOutput(i, &sizeOut);
                
                modeSelect<NumModes-1>(obj, obj.getMode(), outputs, inputs, sizeOut, ParamIndices(), CoeffIndices());
            }
            else
            {
                double outputs[NumCoeffs][1];

                obj.requestOutputSize(0, static_cast<unsigned long>(mTaggedSize));
                obj.allocateOutputs();
                
                modeSelect<NumModes-1>(obj, obj.getMode(), outputs, inputs, 1, ParamIndices(), CoeffIndices());
            
                FrameLib_Parameters::Serial *tagged = obj.getOutput(0);
                
                for (unsigned long i = 0; i < NumCoeffs; i++)
                    tagged->write(T::sCoefficients[i].mName, &outputs[i][0], 1);
            }
        }
        
    private:
        
        bool mValueOutput = true;
        size_t mTaggedSize = 0;
        T mFilter;
        FilterParams mFilterParameters;
    };
    
    // Parameter info
    
    struct ParameterInfo : public FrameLib_Parameters::Info
    {
        ParameterInfo()
        {
            for (unsigned long i = 0; i < NumParams; i++)
                add(ParamList[i].mInfo);
            
            if (DoesModes)
            {
                std::string mode(DoesMulti ? "Sets the filter mode when multi mode is off:" : "Sets the filter mode:");
                
                for (unsigned long i = 0; i < NumModes; i++)
                {
                    mode.append(" ");
                    mode.append(ModeList[i].mName);
                    mode.append(" - ");
                    mode.append(ModeList[i].mInfo);
                    mode.append(".");
                }
                
                add(mode);
                
                if (DoesMulti)
                    add("Sets multi mode (in which all filter modes are output separately).");
            }
            
            if (!DoesCoefficients)
            {
                add("Sets the coefficients input/output mode. "
                    "static - inputs are parameters and outputs are separate coefficient values. "
                    "dynamic - inputs can be a mix of parameters and input frames and outputs are separate frames of coefficient values. "
                    "tagged - inputs are parameters and the output is a single tagged frame.");
            }
            else
            {
                add("Sets dynamic mode (which creates inputs for each settings of the filter).");
                add("Sets whether filter memories are reset before processing a new frame.");
            }
        }
    };
    
public:
    
    // Constructor
    
    FrameLib_Filter(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
    : FrameLib_Processor(context, proxy, getParamInfo())
    {
        for (unsigned long i = 0; i < NumParams; i++)
        {
            mParameters.addDouble(i, ParamList[i].mName, ParamList[i].mDefaultValue, i);
            const Clip &c = ParamList[i].mClip;
            
            switch (c.getType())
            {
                case Clip::kNone:                                           break;
                case Clip::kMin:    mParameters.setMin(c.mMin);             break;
                case Clip::kMax:    mParameters.setMax(c.mMax);             break;
                case Clip::kClip:   mParameters.setClip(c.mMin, c.mMax);    break;
            }
        }
        
        if (DoesModes)
        {
            mParameters.addEnum(ModeIndex, "mode", ModeIndex);
            for (unsigned long i = 0; i < NumModes; i++)
                mParameters.addEnumItem(i, ModeList[i].mName);
        }
        
        if (DoesMulti)
        {
            mParameters.addBool(MultiIndex, "multi", false);
            mParameters.setInstantiation();
        }
        
        if (DoesCoefficients)
        {
            mParameters.addEnum(ParamModeIndex, "coefficients");
            mParameters.addEnumItem(kStatic, "static");
            mParameters.addEnumItem(kDynamic, "dynamic");
            mParameters.addEnumItem(kTagged, "tagged");

            mParameters.setInstantiation();
        }
        else
        {
            mParameters.addBool(ParamModeIndex, "dynamic", false);
            mParameters.setInstantiation();
            mParameters.addBool(ResetIndex, "reset", true);
        }
        
        mParameters.set(serialisedParameters);
        
        unsigned long numIns = isDynamic() ? NumParams + 1 : 1;
        unsigned long numOuts = DoesCoefficients ? (isTagged() ? 1 : NumCoeffs) : isMulti() ? NumModes : 1;
        
        setIO(numIns, numOuts);
        
        if (isDynamic())
        {
            for (unsigned long i = 1; i <= NumParams; i++)
                setInputMode(i, false, false, false);
        }
        
        if (isTagged())
            setOutputType(0, kFrameTagged);
        
        addParameterInput();
        
        mImplementation.setup(*this);
    }
    
    // Info
    
    std::string objectInfo(bool verbose) override
    {
        const char *description = T::sDescription.mDescription;
        
        if (verbose)
        {
            std::string info(description);
            
            info.append(": The size of the output is equal to the input. ");
            
            if (DoesModes)
            {
                if (DoesMulti)
                    info.append("The filter can be set to output a single mode at a time (set with the mode parameter) or all modes simulatanously (set with the multi parameter). ");
                else
                    info.append("The filter mode is set by the mode parameter. ");
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
            return DoesCoefficients ? "Trigger Input" : "Input";
        
        if (idx >= getNumIns() - 1)
            return parameterInputInfo(verbose);
        
        return formatInfo("# - values per sample", "#", ParamList[idx - 1].mInputName, verbose);
    }
    
    std::string outputInfo(unsigned long idx, bool verbose) override
    {
        return mImplementation.outputInfo(*this, idx, verbose);
    }
    
private:
    
    // Params
    
    bool isMulti() const    { return DoesMulti && mParameters.getBool(MultiIndex); }
    bool isDynamic() const  { return mParameters.getInt(ParamModeIndex) == kDynamic; }
    bool isTagged() const   { return mParameters.getInt(ParamModeIndex) == kTagged; }
    bool getReset() const   { return !DoesCoefficients && mParameters.getBool(ResetIndex); }
    size_t getMode() const  { return DoesModes ? static_cast<size_t>(mParameters.getInt(ModeIndex)) : 0; }
    
    // Info
    
    FrameLib_Parameters::Info *getParamInfo()
    {
        static ParameterInfo info;
        return &info;
    }
    
    // Helpers for updating the filter coefficients
    
    template <size_t Idx>
    double getFilterParamVal(ParamInputs& inputs, unsigned long i)
    {
        if (inputs[Idx].mInput && inputs[Idx].mSize)
        {
            const Clip &c = ParamList[Idx].mClip;
            
            const double val = inputs[Idx].mInput[std::min(i, inputs[Idx].mSize - 1)];
            
            switch (c.getType())
            {
                case Clip::kNone:   return val;
                case Clip::kMin:    return std::max(val, c.mMin);
                case Clip::kMax:    return std::min(val, c.mMax);
                case Clip::kClip:   return std::min(std::max(val, c.mMin), c.mMax);
            }
        }
        
        return mParameters.getValue(static_cast<unsigned long>(Idx));
    }
    
    void process() override
    {
        mImplementation.process(*this);
    }
    
    // Select the correct implementation
    
    Implementation<T, DoesCoefficients> mImplementation;
};

#endif
