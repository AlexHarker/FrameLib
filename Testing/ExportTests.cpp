

#include <fstream>
#include <sstream>
#include <iostream>
#include "../FrameLib_Exports/FrameLib_TypeList.h"
#include "FrameLib_SerialiseGraph.h"

// Ideally this would be generated, but for now this will work

using FrameLib_MultistreamList = detail::FrameLib_Typelist<

FrameLib_Expand<FrameLib_FromHost>,
FrameLib_Expand<FrameLib_AudioTrigger>,
FrameLib_Expand<FrameLib_Prioritise>,
FrameLib_Expand<FrameLib_Once>,
FrameLib_Expand<FrameLib_Wrap>,
FrameLib_Expand<FrameLib_Clip>,
FrameLib_Expand<FrameLib_Fold>,
FrameLib_Expand<FrameLib_SVF>,
FrameLib_Expand<FrameLib_BiquadCoeff>,
FrameLib_Expand<FrameLib_OnePoleZero>,
FrameLib_Expand<FrameLib_OnePole>,
FrameLib_Expand<FrameLib_Biquad>,
FrameLib_Expand<FrameLib_Convert>,
FrameLib_Expand<FrameLib_Map>,
FrameLib_Expand<FrameLib_Constant>,
FrameLib_Expand<FrameLib_SampleRate>,
FrameLib_Expand<FrameLib_Lookup>,
FrameLib_Expand<FrameLib_Sqrt>,
FrameLib_Expand<FrameLib_Floor>,
FrameLib_Expand<FrameLib_Exp2>,
FrameLib_Expand<FrameLib_Tanh>,
FrameLib_Expand<FrameLib_Cbrt>,
FrameLib_Expand<FrameLib_Cos>,
FrameLib_Expand<FrameLib_Atanh>,
FrameLib_Expand<FrameLib_Acosh>,
FrameLib_Expand<FrameLib_Atan>,
FrameLib_Expand<FrameLib_Erf>,
FrameLib_Expand<FrameLib_Cosh>,
FrameLib_Expand<FrameLib_Asin>,
FrameLib_Expand<FrameLib_Exp>,
FrameLib_Expand<FrameLib_Log10>,
FrameLib_Expand<FrameLib_Log2>,
FrameLib_Expand<FrameLib_Erfc>,
FrameLib_Expand<FrameLib_Acos>,
FrameLib_Expand<FrameLib_Abs>,
FrameLib_Expand<FrameLib_Log>,
FrameLib_Expand<FrameLib_Asinh>,
FrameLib_Expand<FrameLib_Trunc>,
FrameLib_Expand<FrameLib_LogicalNot>,
FrameLib_Expand<FrameLib_Sinh>,
FrameLib_Expand<FrameLib_Ceil>,
FrameLib_Expand<FrameLib_Round>,
FrameLib_Expand<FrameLib_Tan>,
FrameLib_Expand<FrameLib_Sin>,
FrameLib_Expand<FrameLib_TimeStdDev>,
FrameLib_Expand<FrameLib_MovingAverage>,
FrameLib_Expand<FrameLib_TimeMedian>,
FrameLib_Expand<FrameLib_Lag>,
FrameLib_Expand<FrameLib_FrameDelta>,
FrameLib_Expand<FrameLib_TimeMean>,
FrameLib_Expand<FrameLib_Source>,
FrameLib_Expand<FrameLib_FIRPhase>,
FrameLib_Expand<FrameLib_FFT>,
FrameLib_Expand<FrameLib_Multitaper>,
FrameLib_Expand<FrameLib_Convolve>,
FrameLib_Expand<FrameLib_iFFT>,
FrameLib_Expand<FrameLib_Correlate>,
FrameLib_Expand<FrameLib_Window>,
FrameLib_Expand<FrameLib_Store>,
FrameLib_Expand<FrameLib_Register>,
FrameLib_Expand<FrameLib_Recall>,
FrameLib_Expand<FrameLib_Complex_Conj>,
FrameLib_Expand<FrameLib_Complex_Cos>,
FrameLib_Expand<FrameLib_Complex_Log10>,
FrameLib_Expand<FrameLib_Complex_Sinh>,
FrameLib_Expand<FrameLib_Complex_Exp>,
FrameLib_Expand<FrameLib_Complex_Tanh>,
FrameLib_Expand<FrameLib_Complex_Sqrt>,
FrameLib_Expand<FrameLib_Complex_Log>,
FrameLib_Expand<FrameLib_Complex_Tan>,
FrameLib_Expand<FrameLib_Complex_Sin>,
FrameLib_Expand<FrameLib_Complex_Cosh>,
FrameLib_Pack,
FrameLib_Unpack,
FrameLib_Expand<FrameLib_StreamID>,
FrameLib_Expand<FrameLib_TimeDelta>,
FrameLib_Expand<FrameLib_Now>,
FrameLib_Expand<FrameLib_Timer>,
FrameLib_Expand<FrameLib_Ticks>,
FrameLib_Expand<FrameLib_ComplexExpression>,
FrameLib_Expand<FrameLib_Expression>,
FrameLib_Expand<FrameLib_Tag>,
FrameLib_Expand<FrameLib_Untag>,
FrameLib_Expand<FrameLib_CombineTags>,
FrameLib_Expand<FrameLib_MakeString>,
FrameLib_Expand<FrameLib_FilterTags>,
FrameLib_Expand<FrameLib_LookupString>,
FrameLib_Expand<FrameLib_Complex_Add>,
FrameLib_Expand<FrameLib_Complex_Subtract>,
FrameLib_Expand<FrameLib_Complex_Multiply>,
FrameLib_Expand<FrameLib_Complex_Pow>,
FrameLib_Expand<FrameLib_Complex_Divide>,
FrameLib_Expand<FrameLib_LessThan>,
FrameLib_Expand<FrameLib_Atan2>,
FrameLib_Expand<FrameLib_Subtract>,
FrameLib_Expand<FrameLib_Hypot>,
FrameLib_Expand<FrameLib_Equal>,
FrameLib_Expand<FrameLib_Multiply>,
FrameLib_Expand<FrameLib_Pow>,
FrameLib_Expand<FrameLib_Divide>,
FrameLib_Expand<FrameLib_Max>,
FrameLib_Expand<FrameLib_NotEqual>,
FrameLib_Expand<FrameLib_GreaterThan>,
FrameLib_Expand<FrameLib_LogicalAnd>,
FrameLib_Expand<FrameLib_Min>,
FrameLib_Expand<FrameLib_LessThanEqual>,
FrameLib_Expand<FrameLib_Diff>,
FrameLib_Expand<FrameLib_Add>,
FrameLib_Expand<FrameLib_LogicalOr>,
FrameLib_Expand<FrameLib_CopySign>,
FrameLib_Expand<FrameLib_GreaterThanEqual>,
FrameLib_Expand<FrameLib_Modulo>,
FrameLib_Expand<FrameLib_Random>,
FrameLib_Expand<FrameLib_Gaussian>,
FrameLib_Expand<FrameLib_Uniform>,
FrameLib_Expand<FrameLib_MakeWindow>,
FrameLib_Expand<FrameLib_Ramp>,
FrameLib_Expand<FrameLib_Dispatch>,
FrameLib_Expand<FrameLib_Route>,
FrameLib_Expand<FrameLib_Select>,
FrameLib_Expand<FrameLib_Cartopol>,
FrameLib_Expand<FrameLib_Spatial>,
FrameLib_Expand<FrameLib_Poltocar>,
FrameLib_Expand<FrameLib_FrameMaxPosition>,
FrameLib_Expand<FrameLib_Percentile>,
FrameLib_Expand<FrameLib_Centroid>,
FrameLib_Expand<FrameLib_NonZero>,
FrameLib_Expand<FrameLib_Spread>,
FrameLib_Expand<FrameLib_KernelSmooth>,
FrameLib_Expand<FrameLib_FrameMin>,
FrameLib_Expand<FrameLib_Kurtosis>,
FrameLib_Expand<FrameLib_AccumPos>,
FrameLib_Expand<FrameLib_Sort>,
FrameLib_Expand<FrameLib_Mean>,
FrameLib_Expand<FrameLib_GeometricMean>,
FrameLib_Expand<FrameLib_Join>,
FrameLib_Expand<FrameLib_Shift>,
FrameLib_Expand<FrameLib_FrameMax>,
FrameLib_Expand<FrameLib_StandardDeviation>,
FrameLib_Expand<FrameLib_Sum>,
FrameLib_Expand<FrameLib_Subframe>,
FrameLib_Expand<FrameLib_NanFilter>,
FrameLib_Expand<FrameLib_Reverse>,
FrameLib_Expand<FrameLib_Length>,
FrameLib_Expand<FrameLib_Peaks>,
FrameLib_Expand<FrameLib_Pattern>,
FrameLib_Expand<FrameLib_Product>,
FrameLib_Expand<FrameLib_Skewness>,
FrameLib_Expand<FrameLib_Crest>,
FrameLib_Expand<FrameLib_Pad>,
FrameLib_Expand<FrameLib_Chop>,
FrameLib_Expand<FrameLib_Flatness>,
FrameLib_Expand<FrameLib_RMS>,
FrameLib_Expand<FrameLib_Split>,
FrameLib_Expand<FrameLib_MedianFilter>,
FrameLib_Expand<FrameLib_FrameMinPosition>,
FrameLib_Expand<FrameLib_Interval>,
FrameLib_Expand<FrameLib_PerBlock>,
FrameLib_Expand<FrameLib_Chain>,
FrameLib_Expand<FrameLib_Info>,
FrameLib_Expand<FrameLib_Read>,
FrameLib_Expand<FrameLib_ToHost>,
FrameLib_Expand<FrameLib_Trace>,
FrameLib_Expand<FrameLib_Sink>
>;

// Main

template<typename T>
struct ObjectCreator
{
    void operator ()(FrameLib_OwnedList<FrameLib_Multistream> *objects, FrameLib_Context context)
    {
        FrameLib_Parameters::Serial serial;

        using Connection = FrameLib_Multistream::Connection;
        
        auto thisObject = new T(context, &serial, nullptr, 1);
        
        if (objects->size() == 2)
        {
            thisObject->addConnection(Connection((*objects)[0].get(), 0), 0);
            thisObject->addConnection(Connection((*objects)[1].get(), 0), 1);
        }
        else if (objects->size() > 2)
            thisObject->addConnection(Connection((*objects)[2].get(), 0), 0);
        
        objects->add(thisObject);
    }
};

int main(int argc, const char * argv[]) {
    
    FrameLib_Global *global = nullptr;
    
    FrameLib_Global::get(&global, DEFAULT_THREAD_PRIORITIES);
    FrameLib_Context context(global, &global);
    FrameLib_OwnedList<FrameLib_Multistream> objects;
    
    FrameLib_MultistreamList list;

    if (argc < 2)
    {
        std::cout << "No write location!\n";
        return 1;
    }
    
    list.execute<ObjectCreator>(&objects, context);
    
    std::vector<FrameLib_ObjectDescription> descriptions;
    
    serialiseGraph(descriptions, objects.back().get());
    
    std::ofstream oFile(argv[1]);
    
    if (oFile.is_open())
    {
        for (auto it = descriptions.begin(); it != descriptions.end(); it++)
            oFile << it->mObjectType << "\n";
    }
    else
    {
        std::cout << "Couldn't open file!\n";
        return 1;
    }

    oFile.close();
    
    std::cout << "Output written to " << argv[1] << "\n";

    return 0;
}
