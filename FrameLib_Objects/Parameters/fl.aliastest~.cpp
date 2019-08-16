//
//  fl.cpp
//  fl.aliastest~
//
//  Created by Alex Harker on 16/03/2018.
//

#include "FrameLib_ParamAlias.h"
#include "FrameLib_Map.h"
#include "FrameLib_Convert.h"

class AliasControl : public FrameLib_ParamAlias
{
    class ParameterInfo : public FrameLib_Parameters::Info
    {
    public:
        ParameterInfo();
    };
    
public:

    AliasControl(FrameLib_Context context, void *owner) : FrameLib_ParamAlias(context, 2, owner, &sParamInfo)
    {
        addAlias(0, "cmode", "mode");
        addAlias(1, "mmode", "mode");
        addAlias(1, "lo", "outlo");
        addAlias(1, "hi", "outhi");
        addAlias(1, "exponent", "exponent");
    }
    
    static ParameterInfo sParamInfo;
};

AliasControl::ParameterInfo AliasControl::sParamInfo;

AliasControl::ParameterInfo::ParameterInfo()
{
    add("Sets the conversion mode.");
    add("Sets the map mode");
    add("Sets the lo output");
    add("Sets the hi output");
    add("Sets the exponent");
}

class FrameLib_AliasTest : public FrameLib_Block
{
    struct ParameterInfo : public FrameLib_Parameters::Info { ParameterInfo(); };
    
public:
    
    // Constructor / Destructor
    
    FrameLib_AliasTest(FrameLib_Context context, FrameLib_Parameters::Serial *serialisedParameters, void *owner);
    
    // Object Type
    
    static ObjectType getType() { return kProcessor; }
    
    // Info
    
    //std::string objectInfo(bool verbose);
    //std::string inputInfo(unsigned long idx, bool verbose);
    //std::string outputInfo(unsigned long idx, bool verbose);
    
    // Connection Types
    
    virtual FrameType inputType(unsigned long idx) const    { return kFrameAny; }
    virtual FrameType outputType(unsigned long idx) const   { return kFrameAny; }
    
    // N.B. - Nothing can be acheived by setting a fixed input, so ignore this
    
    virtual void setFixedInput(unsigned long idx, double *input, unsigned long size) {}
    virtual const double *getFixedInput(unsigned long idx, unsigned long *size) { return getEmptyFixedInput(idx, size); }

    // Audio Processing
    
    virtual void blockUpdate(const double * const *ins, double **outs, unsigned long blockSize) {}
    virtual void reset(double samplingRate, unsigned long maxBlockSize)
    {
        mMap.reset(samplingRate, maxBlockSize);
        mConvert.reset(samplingRate, maxBlockSize);
        mAlias.reset(samplingRate, maxBlockSize);
    }
    
    virtual const FrameLib_Parameters *getParameters() const { return mAlias.getParameters(); }
    
    // Ordering Connections
    
    virtual void autoOrderingConnections() {};
    virtual void clearAutoOrderingConnections() {};
    
private:
    
    // Data
    
    AliasControl mAlias;
    FrameLib_Map mMap;
    FrameLib_Convert mConvert;
};


FrameLib_AliasTest::FrameLib_AliasTest(FrameLib_Context context, FrameLib_Parameters::Serial *params, void *owner)
: FrameLib_Block(kProcessor, context, owner), mAlias(context, owner), mMap(context, mAlias.aliasForConstruction(params, 1), owner), mConvert(context, mAlias.aliasForConstruction(params, 0), owner)
{
    setIO(3, 2);
    
    mAlias.setInputAlias(Connection(this, 2), 0);
    
    mConvert.addConnection(Connection(&mAlias, 0), 1);
    mMap.addConnection(Connection(&mAlias, 1), 1);
    
    mConvert.setInputAlias(Connection(this, 0), 0);
    mMap.setInputAlias(Connection(this, 1), 0);

    mConvert.setOutputAlias(Connection(this, 0), 0);
    mMap.setOutputAlias(Connection(this, 1), 0);
    
    mAlias.initialise();
}

#include "FrameLib_MaxClass.h"

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expand<FrameLib_AliasTest>::makeClass(CLASS_BOX, "fl.aliastest~");
}
