
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
            constexpr Coeff(const char *outputName) : mOutputName(outputName) {}
            
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
    };
    
    // Recursion helper
    
    template <size_t Idx>
    struct ModeRecurse
    {
        template <class U, typename... Args>
        void operator()(U& object, Args... args)
        {
            object.template modeSelect<Idx-1>(args...);
        }
    };
    
    template <>
    struct ModeRecurse<0>
    {
        template <class U, typename... Args>
        void operator()(U& object, Args... args) {}
    };
    
    // Index sequence type functionality without C++14
    
    template<std::size_t... Ints>
    struct Indices {};
    
    template<std::size_t M, std::size_t... Is>
    struct MakeIndices : MakeIndices<M-1, M-1, Is...> {};
    
    template<std::size_t... Is>
    struct MakeIndices<0, Is...> : Indices<Is...> {};
    
    // Main class template for FrameLib DSP objects
    
    template <class T>
    class BaseImpl : public FrameLib_Processor
    {
        
    protected:
        
        // Input structure
        
        struct Input
        {
            const double* mInput = nullptr;
            unsigned long mSize = 0;
        };
        
        // Types and constexpr values
        
        static constexpr size_t NumParams = std::tuple_size<typename T::ParamType>::value;
        static constexpr size_t NumModes = std::tuple_size<typename T::ModeType>::value;
        static constexpr size_t NumCoeffs = std::tuple_size<typename T::CoeffType>::value;
        
        static constexpr bool HasModes = NumModes > 1;
        static constexpr bool DoesCoefficients = NumCoeffs != 0;
        
        static constexpr unsigned long ModeIndex = NumParams;
        static constexpr unsigned long MultiIndex = NumParams + 1;
        static constexpr unsigned long DynamicIndex = NumParams + (HasModes ? DoesCoefficients ? 1 : 2 : 0);
        static constexpr unsigned long ResetIndex = DynamicIndex + 1;
        
        static constexpr MakeIndices<NumParams> ParamIndices() { return MakeIndices<NumParams>(); }
        static constexpr MakeIndices<NumCoeffs> CoeffIndices() { return MakeIndices<NumCoeffs>(); }
        static constexpr MakeIndices<NumModes>  ModeIndices()  { return MakeIndices<NumModes>(); }
        static constexpr const typename T::Param& GetFilterParam(size_t Idx)    { return T::sParameters[Idx]; }
        static constexpr const typename T::Mode& GetMode(size_t Idx)            { return T::sModes[Idx]; }
        
        using Clip = typename T::Clip;
        using ParamInputs = std::array<Input, NumParams>;
        using FilterParams = std::array<double, NumParams + 1 + (DoesCoefficients ? 1 : 0)>;
        
        // Parameter info
        
        struct ParameterInfo : public FrameLib_Parameters::Info
        {
            ParameterInfo()
            {
                for (unsigned long i = 0; i < NumParams; i++)
                    add(GetFilterParam(i).mInfo);
                
                if (HasModes)
                {
                    std::string mode("Sets the filter mode when multi mode is off:");
                    
                    for (unsigned long i = 0; i < NumModes; i++)
                    {
                        mode.append(" ");
                        mode.append(GetMode(i).mName);
                        mode.append(" - ");
                        mode.append(GetMode(i).mInfo);
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
        
        BaseImpl(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
        : FrameLib_Processor(context, proxy, getParamInfo())
        {
            for (unsigned long i = 0; i < NumParams; i++)
            {
                mParameters.addDouble(i, GetFilterParam(i).mName, GetFilterParam(i).mDefaultValue, i);
                const Clip &c = GetFilterParam(i).mClip;
                
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
                    mParameters.addEnumItem(i, GetMode(i).mName);
                
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
            
            unsigned long numIns = getDynamicParam() ? NumParams + 1 : 1;
            unsigned long numOuts = DoesCoefficients ? NumCoeffs : getMultiParam() ? NumModes : 1;
            
            setIO(numIns, numOuts);
            
            if (getDynamicParam())
            {
                for (unsigned long i = 1; i <= NumParams; i++)
                    setInputMode(i, false, false, false);
            }
            
            addParameterInput();
        }
        
        // Info
        
        std::string objectInfo(bool verbose) override
        {
            const char *description = T::sDescription.mDescription;
            
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
                return DoesCoefficients ? "Trigger Input" : "Input";
            
            if (idx >= getNumIns() - 1)
                return parameterInputInfo(verbose);
            
            return formatInfo("# - values per sample", "#", GetFilterParam(idx - 1).mInputName, verbose);
        }
        
    protected:
        
        // Params
        
        bool getMultiParam() const      { return HasModes && mParameters.getBool(MultiIndex); }
        size_t getModeParam() const     { return HasModes ? static_cast<size_t>(mParameters.getInt(ModeIndex)) : 0; }
        size_t getDynamicParam() const  { return mParameters.getBool(DynamicIndex); }
        size_t getResetParam() const    { return mParameters.getBool(ResetIndex); }

        // Info
        
        FrameLib_Parameters::Info *getParamInfo()
        {
            static ParameterInfo info;
            return &info;
        }
        
        // Helpers for updating the filter coefficients
        
        template <size_t Idx>
        double getFilterParamVal(const ParamInputs& inputs, unsigned long i)
        {
            if (inputs[Idx].mInput && inputs[Idx].mSize)
            {
                const Clip &c = GetFilterParam(Idx).mClip;
                
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
        
        // Data
        
        T mFilter;
        FilterParams mFilterParameters;
    };
    
    // Main class template for FrameLib DSP objects
    
    template <class T>
    class FrameLib_Filter final : public BaseImpl<T>
    {
        using Base = BaseImpl<T>;
        using FilterParams = typename Base::FilterParams;
        using FilterOutputs = std::array<double *, Base::NumModes>;
        using ParamInputs = typename Base::ParamInputs;
        
        template <size_t Idx>
        friend struct ModeRecurse;
        
    public:
        
        // Constructor
        
        FrameLib_Filter(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
        : Base(context, serialisedParameters, proxy) {}
        
        std::string outputInfo(unsigned long idx, bool verbose) override
        {
            if (!Base::getMultiParam())
                return "Output";
            
            return Base::GetMode(idx).mOutputName;
        }
        
    private:
        
        template <size_t... Is>
        void updateCoefficients(const ParamInputs& inputs, unsigned long i, Indices<Is...>)
        {
            const FilterParams parameters { Base::template getFilterParamVal<Is>(inputs, i)..., Base::mSamplingRate };
            
            if (parameters != Base::mFilterParameters)
            {
                Base::mFilterParameters = parameters;
                Base::mFilter.updateCoefficients(std::get<Is>(parameters)..., Base::mSamplingRate);
            }
        }
        
        // Output calculations
        
        template <size_t Idx>
        void calculateOutput(double *output, const double *input, unsigned long i)
        {
            output[i] = (Base::mFilter.*Base::GetMode(Idx).mMethod)(input[i]);
        }
        
        template <size_t Idx>
        void calculateOutput(double **outputs, const double *input, unsigned long i)
        {
            outputs[Idx][i] = (Base::mFilter.*Base::GetMode(Idx).mMethod)(input[i]);
        }
        
        // Main DSP loops
        
        template <typename O, size_t... Is>
        void processLoops(O outputs, const double *input, const ParamInputs& paramIns, unsigned long size, bool dynamic, Indices<Is...>)
        {
            if (dynamic)
            {
                for (unsigned long i = 0; i < size; i++)
                {
                    updateCoefficients(paramIns, i, Base::ParamIndices());
                    Base::mFilter(input[i]);
                    (void) std::initializer_list<int>{ (calculateOutput<Is>(outputs, input, i), 0)... };
                }
            }
            else
            {
                updateCoefficients(paramIns, 0, Base::ParamIndices());
                
                for (unsigned long i = 0; i < size; i++)
                {
                    Base::mFilter(input[i]);
                    (void) std::initializer_list<int>{ (calculateOutput<Is>(outputs, input, i), 0)... };
                }
            }
        }
        
        // Mode selection
        
        template <size_t Idx>
        void modeSelect(double *output, const double *input, const ParamInputs& paramIns, unsigned long size, size_t mode, bool dynamic)
        {
            if (Idx == mode)
                processLoops(output, input, paramIns, size, dynamic, Indices<Idx>());
            else
                ModeRecurse<Idx>()(*this, output, input, paramIns, size, mode, dynamic);
        }
        
        // Process
        
        void process() override
        {
            ParamInputs paramIns;
            FilterOutputs multiOuts;
            bool dynamic = false;
            
            // Get Input
            
            unsigned long sizeIn, sizeOut;
            unsigned long numOuts = Base::getNumOuts();
            const double *input = Base::getInput(0, &sizeIn);
            
            // Read in the inputs and check if they change
            
            if (Base::getDynamicParam())
            {
                for (unsigned long i = 0; i < Base::NumParams; i++)
                {
                    paramIns[i].mInput = Base::getInput(i + 1, &paramIns[i].mSize);
                    dynamic = dynamic || paramIns[i].mSize > 1;
                }
            }
            
            // Setup outputs
            
            for (unsigned long i = 0; i < numOuts; i++)
                Base::requestOutputSize(i, sizeIn);
            
            Base::allocateOutputs();
            
            for (unsigned long i = 0; i < numOuts; i++)
                multiOuts[i] = Base::getOutput(i, &sizeOut);
            
            // Do (optional) reset and DSP
            
            if (Base::getResetParam())
                Base::mFilter.reset();
            
            if (Base::getMultiParam())
                processLoops(multiOuts.data(), input, paramIns, sizeOut, dynamic, Base::ModeIndices());
            else
                modeSelect<Base::NumModes-1>(multiOuts[0], input, paramIns, sizeOut, Base::getModeParam(), dynamic);
        }
    };
    
    // Main class template for FrameLib DSP objects
    
    template <class T>
    class FrameLib_Coefficients final : public BaseImpl<T>
    {
        using Base = BaseImpl<T>;
        using FilterParams = typename Base::FilterParams;
        using FilterOutputs = std::array<double *, Base::NumModes>;
        using ParamInputs = typename Base::ParamInputs;
        
        template <size_t Idx>
        friend struct ModeRecurse;
        
    public:
        
        // Constructor
        
        FrameLib_Coefficients(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
        : Base(context, serialisedParameters, proxy) {}
        
        std::string outputInfo(unsigned long idx, bool verbose) override
        {
            return T::sCoeffs[idx].mOutputName;
        }
        
    private:
        
        template <size_t Idx, size_t... Is, size_t... Js>
        void modeSelect(double **outputs, const ParamInputs& inputs, unsigned long size, size_t mode, Indices<Is...>, Indices<Js...>)
        {
            if (Idx == mode)
            {
                for (unsigned long i = 0; i < size; i++)
                {
                    const FilterParams parameters { Base::template getFilterParamVal<Is>(inputs, i)..., Idx, Base::mSamplingRate };
                    
                    if (!i || parameters != Base::mFilterParameters)
                    {
                        Base::mFilterParameters = parameters;
                        (Base::mFilter.*Base::GetMode(Idx).mMethod)(std::get<Is>(parameters)..., Base::mSamplingRate, outputs[Js][i]...);
                    }
                    else
                        (void) std::initializer_list<double>{ outputs[Js][i] = outputs[Js][i - 1]... };
                }
            }
            else
                ModeRecurse<Idx>()(*this, outputs, inputs, size, mode);
        }
        
        // Process
        
        void process() override
        {
            ParamInputs paramIns;
            FilterOutputs multiOuts;
            bool dynamic = false;
            
            // Get Input
            
            unsigned long sizeIn = 1;
            unsigned long sizeOut;
            unsigned long numOuts = Base::getNumOuts();
            
            // Read in the inputs and check if they change
            
            if (Base::getDynamicParam())
            {
                for (unsigned long i = 0; i < Base::NumParams; i++)
                {
                    paramIns[i].mInput = Base::getInput(i + 1, &paramIns[i].mSize);
                    dynamic = dynamic || paramIns[i].mSize > 1;
                    sizeIn = std::max(sizeIn, paramIns[i].mSize);
                }
            }
            
            // Setup outputs
            
            for (unsigned long i = 0; i < numOuts; i++)
                Base::requestOutputSize(i, sizeIn);
            
            Base::allocateOutputs();
            
            for (unsigned long i = 0; i < numOuts; i++)
                multiOuts[i] = Base::getOutput(i, &sizeOut);
            
            // DSP
            
            modeSelect<Base::NumModes-1>(multiOuts.data(), paramIns, sizeOut, Base::getModeParam(), Base::ParamIndices(), Base::CoeffIndices());
        }
    };
};

template <class T>
using FrameLib_Filter = FrameLib_Filters::FrameLib_Filter<T>;

template <class T>
using FrameLib_Coefficients = FrameLib_Filters::FrameLib_Coefficients<T>;

#endif
