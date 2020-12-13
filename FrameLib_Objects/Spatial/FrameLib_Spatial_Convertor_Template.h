
#ifndef FrameLib_SPATIAL_CONVERTOR_TEMPLATE_H
#define FrameLib_SPATIAL_CONVERTOR_TEMPLATE_H

#include "FrameLib_DSP.h"

#include <complex>
#include <tuple>

// Spatial Convertor

namespace FrameLib_Spatial_Ops
{
    using Value2D = std::complex<double>;
    using Value3D = std::tuple<double, double, double>;
};

template <typename Op>
class FrameLib_Spatial_Convertor final : public FrameLib_Processor
{
    // Parameter Enums and Info
    
    struct ParameterInfo : public FrameLib_Parameters::Info
    {
        ParameterInfo()
        {
            add("Sets the number of dimensions");
            add("Sets the units used for angles.");
        }
    };
    
    enum Parameters { kDimensions, kAngleUnits };
    enum Dimensions { k2D, k3D };
    enum AngleUnits { kRadians, kDegrees };
    
    struct ZeroPad
    {
        ZeroPad(const double *data, unsigned long size) : mData(data), mSize(size) {}
        
        double operator[](unsigned long idx) const { return idx < mSize ? mData[idx] : 0.0; }
        
    private:
        
        const double *mData;
        unsigned long mSize;
    };
public:

    using Value2D = FrameLib_Spatial_Ops::Value2D;
    using Value3D = FrameLib_Spatial_Ops::Value3D;
    
    // Constructor
    
    FrameLib_Spatial_Convertor(FrameLib_Context context, const FrameLib_Parameters::Serial *serialisedParameters, FrameLib_Proxy *proxy)
    : FrameLib_Processor(context, proxy, getParameterInfo())
    {
        mParameters.addEnum(kDimensions, "dimensions", 0);
        mParameters.addEnumItem(k2D, "2D");
        mParameters.addEnumItem(k3D, "3D");
        mParameters.setInstantiation();
        
        mParameters.addEnum(kAngleUnits, "angles", 1);
        mParameters.addEnumItem(kRadians, "radians");
        mParameters.addEnumItem(kDegrees, "degrees");
        mParameters.setInstantiation();

        mParameters.set(serialisedParameters);
        
        m2D = mParameters.getEnum<Dimensions>(kDimensions) == k2D;
        mDegrees = mParameters.getEnum<AngleUnits>(kAngleUnits) == kDegrees;
        
        unsigned long io = m2D ? 2 : 3;
 
        setIO(io, io);
    }
    
    // Info
    
    std::string objectInfo(bool verbose) override
    {
        return "";
    }
    
    std::string inputInfo(unsigned long idx, bool verbose) override
    {
        return "";
    }
    
    std::string outputInfo(unsigned long idx, bool verbose) override
    {
        return "";
    }
    
private:
    
    // Calculate Split
    
    void calculate(double &out1, double &out2, const double &in1, const double &in2, Op &op)
    {
        const Value2D result = op(Value2D(in1, in2));
        out1 = result.real();
        out2 = result.imag();
    }
    
    void calculate(double &out1, double &out2, double &out3, const double &in1, const double &in2, const double &in3, Op &op)
    {
        const Value3D result = op(Value3D(in1, in2, in3));
        out1 = std::get<0>(result);
        out2 = std::get<1>(result);
        out3 = std::get<2>(result);
    }
    
    // Process
    
    void process() override
    {
        Op op;
        unsigned long size, sizeIn1, sizeIn2, sizeIn3;
        const double *input1 = getInput(0, &sizeIn1);
        const double *input2 = getInput(1, &sizeIn2);
        
        op.prepare(mDegrees);
        
        if (m2D)
        {
            size = std::max(sizeIn1, sizeIn2);
            requestOutputSize(0, size);
            requestOutputSize(1, size);
        
            if (!allocateOutputs())
                return;
            
            double *output1 = getOutput(0, &size);
            double *output2 = getOutput(1, &size);
                
            unsigned long sizeCommon = std::min(sizeIn1, sizeIn2);
                
            for (unsigned long i = 0; i < sizeCommon; i++)
                calculate(output1[i], output2[i], input1[i], input2[i], op);
                
            if (sizeIn1 > sizeIn2)
            {
                for (unsigned long i = sizeCommon; i < size; i++)
                    calculate(output1[i], output2[i], input1[i], 0.0, op);
            }
            else
            {
                for (unsigned long i = sizeCommon; i < size; i++)
                    calculate(output1[i], output2[i], 0.0, input2[i], op);
            }
        }
        else
        {
            const double *input3 = getInput(2, &sizeIn3);
            
            const ZeroPad pad1(input1, sizeIn1);
            const ZeroPad pad2(input2, sizeIn2);
            const ZeroPad pad3(input3, sizeIn3);
            
            size = std::max(sizeIn1, std::max(sizeIn2, sizeIn3));
            
            requestOutputSize(0, size);
            requestOutputSize(1, size);
            requestOutputSize(2, size);

            if (!allocateOutputs())
                return;
            
            double *output1 = getOutput(0, &size);
            double *output2 = getOutput(1, &size);
            double *output3 = getOutput(2, &size);

            unsigned long sizeCommon = std::min(sizeIn1, std::min(sizeIn2, sizeIn3));
                
            for (unsigned long i = 0; i < sizeCommon; i++)
                calculate(output1[i], output2[i], output3[i], input1[i], input2[i], input3[i], op);
            
            for (unsigned long i = sizeCommon; i < size; i++)
                calculate(output1[i], output2[i], output3[i], pad1[i], pad2[i], pad3[i], op);
        }
    }
    
    ParameterInfo *getParameterInfo()
    {
        static ParameterInfo info;
        return &info;
    }
    
    bool m2D;
    bool mDegrees;
};

#endif
