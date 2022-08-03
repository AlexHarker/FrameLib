
#include "FrameLib_PDClass.h"
#include "../FrameLib_Exports/FrameLib_Objects.h"
#include "../FrameLib_Exports/FrameLib_TypeAliases.h"

// Buffer

#include "pd_buffer.h"

// Context Control - A pd class to communicate with the current pd

class FrameLib_PDClass_ContextControl : public PDClass_Base
{
    using PDClass = FrameLib_PDClass<void>;
    
public:
    
    static void classInit(t_class *c, const char *classname)
    {
        addMethod<FrameLib_PDClass_ContextControl, &FrameLib_PDClass_ContextControl::multithread>(c, "multithread");
        addMethod<FrameLib_PDClass_ContextControl, &FrameLib_PDClass_ContextControl::idSet>(c, "id");
        addMethod<FrameLib_PDClass_ContextControl, &FrameLib_PDClass_ContextControl::rtSet>(c, "rt");
        
        class_addmethod(c, (t_method) &extTimeOut, gensym("timeout"), A_DEFFLOAT, A_DEFFLOAT, 0);
        class_addmethod(c, (t_method) &extCodeExport, gensym("export"), A_SYMBOL, A_SYMBOL, 0);
    }
    
    FrameLib_PDClass_ContextControl(t_object *x, t_symbol *sym, long argc, t_atom *argv)
    : mPDContext{ true, canvas_getcurrent(), gensym("") }
    , mContext(mGlobal->makeContext(mPDContext))
    {
        long i = 0;
        
        // Parse attribute tags
        
        while (i < argc)
        {
            t_symbol *sym = atom_getsymbol_default(argv + i++);
            
            if (PDClass::isAttributeTag(sym))
            {
                // Check attributes are valid
                                
                if (!PDClass::isNamedAttributeTag(sym, "rt") && !PDClass::isNamedAttributeTag(sym, "id"))
                {
                    pd_error(asObject(), "unknown attribute %s", sym->s_name);
                    continue;
                }
                
                // Check for missing values
                
                if ((i >= argc) || PDClass::isTag(argv + i))
                {
                    pd_error(asObject(), "no values given for attribute %s", sym->s_name);
                    continue;
                }
                
                if (PDClass::isNamedAttributeTag(sym, "rt"))
                    rtSet(atom_getfloat(argv + i++));
                else if (PDClass::isNamedAttributeTag(sym, "id"))
                    idSet(atom_getsymbol_default(argv + i++));
               
                if (i < argc && !PDClass::isTag(argv + i))
                    pd_error(asObject(), "stray items after attribute %s", sym->s_name);
            }
            else
                pd_error(asObject(), "additional unrecognised arguments");
        }
    }
    
    ~FrameLib_PDClass_ContextControl()
    {
        mGlobal->releaseContext(mContext);
    }
    
    // Attributes
    
    // id attribute
    
    void idSet(t_symbol *name)
    {
        mPDContext.mName = name;
        updateContext();
    }
    
    // rt attribute
    
    void rtSet(t_floatarg arg)
    {
        mPDContext.mRealtime = arg;
        updateContext();
    }
    
    // Time out
    
    static void extTimeOut(FrameLib_PDClass_ContextControl *x, double relative, double absolute)
    {
        x->timeOut(relative, absolute);
    }
    
    // Export
    
    static void extCodeExport(FrameLib_PDClass_ContextControl *x, t_symbol *className, t_symbol *path)
    {
        x->codeExport(className, path);
    }

private:
    
    // Multithreading
    
    void multithread(t_floatarg on)
    {
        FrameLib_Context::ProcessingQueue processingQueue(mContext);
        processingQueue->setMultithreading(on);
    }
    
    // Time out
    
    void timeOut(t_floatarg relative, t_floatarg absolute)
    {
        FrameLib_Context::ProcessingQueue processingQueue(mContext);
        processingQueue->setTimeOuts(relative / 100.0, absolute / 1000.0);
    }
    
    // Export
    
    void codeExport(t_symbol *className, t_symbol *path)
    {
        // Get the first object and its underlying framelib object
        
        t_object *object = searchPatch(mPDContext.mCanvas);
        FrameLib_Multistream *flObject = toFLObject(object);
        
        if (!object || !flObject)
        {
            pd_error(this, "couldn't find any framelib objects in the current context");
            return;
        }
        
        // FIX
        //objectMethod<void>(object, FrameLib_PDPrivate::messageResolveContext());
        
        auto replace = FrameLib_TypeAliases::makeReplaceStrings();
        
        // FIX - should we deal with search paths etc.?
        
        ExportError error = exportGraph(flObject, path->s_name, className->s_name, &replace);
        
        if (error == ExportError::PathError)
            pd_error(this, "couldn't write to or find specified path");
        else if (error == ExportError::WriteError)
            pd_error(this, "couldn't write file");
    }
    
    // Context

    void updateContext()
    {
        mGlobal->releaseContext(mContext);
        mContext = mGlobal->makeContext(mPDContext);
    }
    
    // Convert an object to an FLObject
    
    FrameLib_Multistream *toFLObject(t_object *x)
    {
        return FrameLib_PDPrivate::toFrameLibObject(x);
    }

    t_object *searchPatch(t_glist *gl)
    {
        // Call method on all objects
                        
        for (t_gobj *g = gl->gl_list; g; g = g->g_next)
        {
            if (t_object *object = pd_checkobject(&g->g_pd))
            {
                FrameLib_Multistream *flObject = toFLObject(object);
                if (flObject && flObject->getContext() == mContext)
                    return object;
            }
        }
        
        return nullptr;
    }
    
    // Members
    
    FrameLib_PDGlobals::ManagedPointer mGlobal;
    FrameLib_PDContext mPDContext;
    FrameLib_Context mContext;
};

// To PD Class

class FrameLib_PDClass_ToPD : public FrameLib_PDClass_Expand<FrameLib_ToHost>
{
    struct ToHostProxy : public FrameLib_ToHost::Proxy, public FrameLib_PDMessageProxy
    {
        ToHostProxy(FrameLib_PDClass_ToPD *object)
        : FrameLib_PDMessageProxy(object->asObject())
        , mObject(object)
        {}
        
        void sendToHost(unsigned long index, unsigned long stream, const double *values, unsigned long N, FrameLib_TimeFormat time)  override
        {
            mObject->getHandler()->add(MessageInfo(this, time, stream), values, N);
        }
        
        void sendToHost(unsigned long index, unsigned long stream, const FrameLib_Parameters::Serial *serial, FrameLib_TimeFormat time)  override
        {
            mObject->getHandler()->add(MessageInfo(this, time, stream), serial);
        }
        
        void sendMessage(unsigned long stream, t_symbol *s, short ac, t_atom *av) override
        {
            auto& outlets = mObject->mOutlets;
            unsigned long idx = stream % outlets.size();
            
            if (s)
                outlet_anything(outlets[idx], s, ac, av);
            else if (!ac)
                outlet_bang(outlets[idx]);
            else if (ac == 1)
                outlet_float(outlets[idx], atom_getfloat(av));
            else
                outlet_list(outlets[idx], nullptr, ac, av);
        }
        
    private:
        
        FrameLib_PDClass_ToPD *mObject;
    };
    
public:
    
    // Constructor
    
    FrameLib_PDClass_ToPD(t_object *x, t_symbol *s, long argc, t_atom *argv)
    : FrameLib_PDClass(x, s, argc, argv, new ToHostProxy(this))
    {
        unsigned long nStreams = getSpecifiedStreams();
        
        mOutlets.resize(nStreams);
        
        for (unsigned long i = 0; i < nStreams; i++)
            mOutlets[i] = outlet_new(asObject(), 0L);
        
        mHostProxy = static_cast<ToHostProxy *>(mProxy.get());
    }
    
private:
    
    // Data
    
    ToHostProxy *mHostProxy;
    std::vector<t_outlet *> mOutlets;
};

// From PD Class

class FrameLib_PDClass_FromPD : public FrameLib_PDClass_Expand<FrameLib_FromHost>
{
    struct FromHostProxy : public FrameLib_FromHost::Proxy, public FrameLib_PDProxy
    {
        FromHostProxy(t_object *x)
        : FrameLib_FromHost::Proxy(true, true)
        , FrameLib_PDProxy(x) {}
    };
    
public:
    
    // Class Initialisation
    
    static void classInit(t_class *c, const char *classname)
    {
        FrameLib_PDClass::classInit(c, classname);
        
        addFloatMethod<FrameLib_PDClass_FromPD, &FrameLib_PDClass_FromPD::floatHandler>(c);
        addListMethod<FrameLib_PDClass_FromPD, &FrameLib_PDClass_FromPD::list>(c);
        addAnythingMethod<FrameLib_PDClass_FromPD, &FrameLib_PDClass_FromPD::anything>(c);
    }

    // Constructor
    
    FrameLib_PDClass_FromPD(t_object *x, t_symbol *s, long argc, t_atom *argv)
    : FrameLib_PDClass(x, s, argc, argv, new FromHostProxy(x))
    {
        mHostProxy = static_cast<FromHostProxy *>(mProxy.get());
    }
    
    // Additional handlers
    
    void floatHandler(t_floatarg in)
    {
        double d_in = in;
        mHostProxy->sendFromHost(0, &d_in, 1);
    }
    
    void list(t_symbol *s, long argc, t_atom *argv)
    {
        std::vector<double> temporary(argc);
        
        for (long i = 0; i < argc; i++)
            temporary[i] = atom_getfloat(argv++);
        
        mHostProxy->sendFromHost(0, temporary.data(), argc);
    }
    
    void anything(t_symbol *s, long argc, t_atom *argv)
    {
        if (argc > 1 && atom_gettype(argv) == A_SYMBOL)
            pd_error(this, "too many arguments for string value");
        
        if (argc && atom_gettype(argv) == A_SYMBOL)
            mHostProxy->sendFromHost(0, s->s_name, atom_getsymbol_default(argv)->s_name);
        else
        {
            std::vector<double> temporary(argc);
            
            for (long i = 0; i < argc; i++)
                temporary[i] = atom_getfloat(argv++);
            
            mHostProxy->sendFromHost(0, s->s_name, temporary.data(), argc);
        }
    }

private:
    
    FromHostProxy *mHostProxy;
};

// PD Read Class

class FrameLib_PDClass_Read : public FrameLib_PDClass_Expand<FrameLib_Read>
{
    struct ReadProxy : public FrameLib_Read::Proxy, public FrameLib_PDProxy
    {
        ReadProxy(t_object *x) : FrameLib_PDProxy(x) {}
        
        void update(const char *name) override
        {
            mBufferName = gensym(name);
        }
        
        void acquire(unsigned long& length, double& samplingRate) override
        {
            mBuffer = pd_buffer(mBufferName);
            length = mBuffer.get_length();
            samplingRate = 0.0;
        }
        
        void release() override
        {
            mBuffer = pd_buffer();
        };
        
        void read(double *output, const double *positions, unsigned long size, long chan, InterpType interp, EdgeMode edges, bool bound) override
        {
            mBuffer.read(output, positions, size, 1.0, interp, edges, bound);
        }
        
        FrameLib_Read::Proxy *clone() const override
        {
            return new ReadProxy(*this);
        }
        
    private:
        
        pd_buffer mBuffer;
        t_symbol *mBufferName;
    };
    
public:
    
    // Constructor
    
    FrameLib_PDClass_Read(t_object *x, t_symbol *s, long argc, t_atom *argv)
    : FrameLib_PDClass(x, s, argc, argv, new ReadProxy(x)) {}
};

// PD Expression Classes

// The expression objects parses arguments differently to normal, which is handled by pre-parsing the atoms into a different format

class ExprArgumentParser
{
public:
    
    ExprArgumentParser(t_symbol *s, long argc, t_atom *argv, bool complex) : mSymbol(s), mComplex(complex)
    {
        concatenate(argc, argv);
        
        while (argc--)
        {
            if (atom_getsymbol(argv) == gensym("/expr"))
                concatenate(argc, ++argv);
            else
                mArgs.push_back(*argv++);
        }
    }
    
    t_symbol *symbol() const    { return mSymbol;}
    long count() const          { return static_cast<long>(mArgs.size());}
    t_atom* args() const        { return const_cast<t_atom*>(mArgs.data()); }
    
private:
    
    // Tag Detection
    
    bool isValidTag(t_atom *a)
    {
        t_symbol *sym = atom_getsymbol(a);
        size_t len = strlen(sym->s_name);
        
        // Input tag
        
        if (len > 2 && sym->s_name[0] == '[' && sym->s_name[len - 1] == ']')
            return true;
        
        // Basic parameter tag test
        
        if (len <= 1 || sym->s_name[0] != '/')
            return false;
        
        // Escape division by known constants
        
        if (sym == gensym("/pi") || sym == gensym("/epsilon") || sym == gensym("/e") || sym == gensym("/inf"))
            return false;
        
        if (mComplex && sym == gensym("/i"))
            return false;
        
        // Require a parameter tag to have only letters after the slash
        
        for (const char *c = sym->s_name + 1; *c; c++)
            if (!((*c >= 'a') && (*c <= 'z')) || ((*c >= 'A') && (*c <= 'Z')))
                return false;
        
        return true;
    }
    
    // Concatenator
    
    void concatenate(long& argc, t_atom*& argv)
    {
        std::string concatenated;
        
        for (; argc && !isValidTag(argv); argc--, argv++)
        {
            if (argv->a_type == A_SYMBOL)
                concatenated += atom_getsymbol(argv)->s_name;
            else
                concatenated += std::to_string(atom_getfloat(argv));
            
            // Add whitespace between symbols
            
            concatenated += " ";
        }
        
        if (concatenated.length())
        {
            t_symbol *arg = gensym(concatenated.c_str());
            mArgs.push_back(t_atom());
            SETSYMBOL(&mArgs.back(), arg);
        }
    }
    
    t_symbol *mSymbol;
    bool mComplex;
    std::vector<t_atom> mArgs;
};

// This expression class is a wrapper that allows the parsing to happen correctly

struct FrameLib_PDClass_Expression_Parsed : public FrameLib_PDClass_Expand<FrameLib_Expression>
{
    FrameLib_PDClass_Expression_Parsed(t_object *x, const ExprArgumentParser &parsed) :
    FrameLib_PDClass(x, parsed.symbol(), parsed.count(), parsed.args()) {}
};

// Expression PD Class (inherits from the parsed version which inherits the standard pd class)

struct FrameLib_PDClass_Expression : public FrameLib_PDClass_Expression_Parsed
{
    // Constructor
    
    FrameLib_PDClass_Expression(t_object *x, t_symbol *s, long argc, t_atom *argv) :
    FrameLib_PDClass_Expression_Parsed(x, ExprArgumentParser(s, argc, argv, false)) {}
};

// This complex expression class is a wrapper that allows the parsing to happen correctly

struct FrameLib_PDClass_ComplexExpression_Parsed : public FrameLib_PDClass_Expand<FrameLib_ComplexExpression>
{
    FrameLib_PDClass_ComplexExpression_Parsed(t_object *x, const ExprArgumentParser &parsed) :
    FrameLib_PDClass(x, parsed.symbol(), parsed.count(), parsed.args()) {}
};

// Complex Expression PD Class (inherits from the parsed version which inherits the standard pd class)

struct FrameLib_PDClass_ComplexExpression : public FrameLib_PDClass_ComplexExpression_Parsed
{
    // Constructor
    
    FrameLib_PDClass_ComplexExpression(t_object *x, t_symbol *s, long argc, t_atom *argv) :
    FrameLib_PDClass_ComplexExpression_Parsed(x, ExprArgumentParser(s, argc, argv, true)) {}
};

// Main setup routine

extern "C" void framelib_pd_setup(void)
{
    // Context Control
    
    FrameLib_PDClass_ContextControl::makeClass<FrameLib_PDClass_ContextControl>("fl.contextcontrol~");

    // Host Communication
    
    FrameLib_PDClass_ToPD::makeClass<FrameLib_PDClass_ToPD>("fl.topd~");
    FrameLib_PDClass_ToPD::makeClass<FrameLib_PDClass_FromPD>("fl.frompd~");

    // Filters
    
    FrameLib_PDClass_Expand<FrameLib_Biquad>::makeClass("fl.biquad~");
    FrameLib_PDClass_Expand<FrameLib_BiquadCoeff>::makeClass("fl.biquadcoeff~");
    FrameLib_PDClass_Expand<FrameLib_OnePole>::makeClass("fl.onepole~");
    FrameLib_PDClass_Expand<FrameLib_OnePoleZero>::makeClass("fl.onepolezero~");
    FrameLib_PDClass_Expand<FrameLib_SVF>::makeClass("fl.svf~");

    // Generators
    
    FrameLib_PDClass_Expand<FrameLib_Gaussian>::makeClass("fl.gaussian~");
    FrameLib_PDClass_Expand<FrameLib_MakeWindow>::makeClass("fl.makewindow~");
    FrameLib_PDClass_Expand<FrameLib_Ramp>::makeClass("fl.ramp~");
    FrameLib_PDClass_Expand<FrameLib_Random>::makeClass("fl.random~");
    FrameLib_PDClass_Expand<FrameLib_Uniform>::makeClass("fl.uniform~");

    // IO
    
    FrameLib_PDClass_Expand<FrameLib_Sink>::makeClass("fl.sink~");
    FrameLib_PDClass_Expand<FrameLib_Source>::makeClass("fl.source~");
    FrameLib_PDClass_Expand<FrameLib_Trace>::makeClass("fl.trace~");

    // Mapping
    
    FrameLib_PDClass_Expand<FrameLib_Constant>::makeClass("fl.constant~");
    FrameLib_PDClass_Expand<FrameLib_Convert>::makeClass("fl.convert~");
    FrameLib_PDClass_Expand<FrameLib_Lookup>::makeClass("fl.lookup~");
    FrameLib_PDClass_Expand<FrameLib_Map>::makeClass("fl.map~");
    FrameLib_PDClass_Expand<FrameLib_SampleRate>::makeClass("fl.samplerate~");
    
    // Parameters
    
    FrameLib_PDClass_Expand<FrameLib_CombineTags>::makeClass("fl.combinetags~");
    FrameLib_PDClass_Expand<FrameLib_FilterTags>::makeClass("fl.filtertags~");
    FrameLib_PDClass_Expand<FrameLib_LookupString>::makeClass("fl.lookupstring~");
    FrameLib_PDClass_Expand<FrameLib_MakeString>::makeClass("fl.makestring~");
    FrameLib_PDClass_Expand<FrameLib_Tag>::makeClass("fl.tag~");
    FrameLib_PDClass_Expand<FrameLib_Untag>::makeClass("fl.untag~");
    
    // Routing
    
    FrameLib_PDClass_Expand<FrameLib_Dispatch>::makeClass("fl.dispatch~");
    FrameLib_PDClass_Expand<FrameLib_Route>::makeClass("fl.route~");
    FrameLib_PDClass_Expand<FrameLib_Select>::makeClass("fl.select~");
    
    // Schedulers
    
    FrameLib_PDClass_Expand<FrameLib_AudioTrigger>::makeClass("fl.audiotrigger~");
    FrameLib_PDClass_Expand<FrameLib_Chain>::makeClass("fl.chain~");
    FrameLib_PDClass_Expand<FrameLib_Interval>::makeClass("fl.interval~");
    FrameLib_PDClass_Expand<FrameLib_Once>::makeClass("fl.once~");
    FrameLib_PDClass_Expand<FrameLib_PerBlock>::makeClass("fl.perblock~");
    
    // Spatial
    
    FrameLib_PDClass_Expand<FrameLib_Cartopol>::makeClass("fl.cartopol~");
    FrameLib_PDClass_Expand<FrameLib_Poltocar>::makeClass("fl.poltocar~");
    FrameLib_PDClass_Expand<FrameLib_Spatial>::makeClass("fl.spatial~");
    
    // Spectral
    
    FrameLib_PDClass_Expand<FrameLib_FFT>::makeClass("fl.fft~");
    FrameLib_PDClass_Expand<FrameLib_iFFT>::makeClass("fl.ifft~");
    FrameLib_PDClass_Expand<FrameLib_Correlate>::makeClass("fl.correlate~");
    FrameLib_PDClass_Expand<FrameLib_Convolve>::makeClass("fl.convolve~");
    FrameLib_PDClass_Expand<FrameLib_FIRPhase>::makeClass("fl.firphase~");
    FrameLib_PDClass_Expand<FrameLib_Multitaper>::makeClass("fl.multitaper~");
    FrameLib_PDClass_Expand<FrameLib_Window>::makeClass("fl.window~");
    
    // Storage
    
    FrameLib_PDClass_Expand<FrameLib_Recall>::makeClass("fl.recall~");
    FrameLib_PDClass_Expand<FrameLib_Register>::makeClass("fl.register~");
    FrameLib_PDClass_Expand<FrameLib_Store>::makeClass("fl.store~");
    
    // Streaming
    
    FrameLib_PDClass_Expand<FrameLib_StreamID>::makeClass("fl.streamid~");
    FrameLib_PDClass<FrameLib_Pack>::makeClass("fl.pack~");
    FrameLib_PDClass<FrameLib_Unpack>::makeClass("fl.unpack~");
    
    // Time Smoothing
    
    FrameLib_PDClass_Expand<FrameLib_FrameDelta>::makeClass("fl.framedelta~");
    FrameLib_PDClass_Expand<FrameLib_Lag>::makeClass("fl.lag~");
    FrameLib_PDClass_Expand<FrameLib_MovingAverage>::makeClass("fl.movingaverage~");
    FrameLib_PDClass_Expand<FrameLib_TimeMean>::makeClass("fl.timemean~");
    FrameLib_PDClass_Expand<FrameLib_TimeMedian>::makeClass("fl.timemedian~");
    FrameLib_PDClass_Expand<FrameLib_TimeStdDev>::makeClass("fl.timestddev~");
    
    // Timing
    
    FrameLib_PDClass_Expand<FrameLib_Now>::makeClass("fl.now~");
    FrameLib_PDClass_Expand<FrameLib_Ticks>::makeClass("fl.ticks~");
    FrameLib_PDClass_Expand<FrameLib_TimeDelta>::makeClass("fl.timedelta~");
    FrameLib_PDClass_Expand<FrameLib_Timer>::makeClass("fl.timer~");
    
    // Vector
    
    FrameLib_PDClass_Expand<FrameLib_AccumPos>::makeClass("fl.accumpos~");
    FrameLib_PDClass_Expand<FrameLib_Chop>::makeClass("fl.chop~");
    FrameLib_PDClass_Expand<FrameLib_Join>::makeClass("fl.join~");
    FrameLib_PDClass_Expand<FrameLib_KernelSmooth>::makeClass("fl.kernelsmooth~");
    FrameLib_PDClass_Expand<FrameLib_MedianFilter>::makeClass("fl.medianfilter~");
    FrameLib_PDClass_Expand<FrameLib_NonZero>::makeClass("fl.nonzero~");
    FrameLib_PDClass_Expand<FrameLib_Pad>::makeClass("fl.pad~");
    FrameLib_PDClass_Expand<FrameLib_Peaks>::makeClass("fl.pattern~");
    FrameLib_PDClass_Expand<FrameLib_Peaks>::makeClass("fl.peaks~");
    FrameLib_PDClass_Expand<FrameLib_Percentile>::makeClass("fl.percentile~");
    FrameLib_PDClass_Expand<FrameLib_Prioritise>::makeClass("fl.prioritise~");
    FrameLib_PDClass_Expand<FrameLib_Reverse>::makeClass("fl.reverse~");
    FrameLib_PDClass_Expand<FrameLib_Shift>::makeClass("fl.shift~");
    FrameLib_PDClass_Expand<FrameLib_Sort>::makeClass("fl.sort~");
    FrameLib_PDClass_Expand<FrameLib_Split>::makeClass("fl.split~");
    FrameLib_PDClass_Expand<FrameLib_Subframe>::makeClass("fl.subframe~");

    FrameLib_PDClass_Expand<FrameLib_Length>::makeClass("fl.length~");
    FrameLib_PDClass_Expand<FrameLib_FrameMin>::makeClass("fl.framemin~");
    FrameLib_PDClass_Expand<FrameLib_FrameMax>::makeClass("fl.framemax~");
    FrameLib_PDClass_Expand<FrameLib_Sum>::makeClass("fl.sum~");
    FrameLib_PDClass_Expand<FrameLib_Product>::makeClass("fl.product~");
    FrameLib_PDClass_Expand<FrameLib_Mean>::makeClass("fl.mean~");
    FrameLib_PDClass_Expand<FrameLib_GeometricMean>::makeClass("fl.geomean~");
    FrameLib_PDClass_Expand<FrameLib_StandardDeviation>::makeClass("fl.stddev~");

    FrameLib_PDClass_Expand<FrameLib_Centroid>::makeClass("fl.centroid~");
    FrameLib_PDClass_Expand<FrameLib_Spread>::makeClass("fl.spread~");
    FrameLib_PDClass_Expand<FrameLib_Skewness>::makeClass("fl.skewness~");
    FrameLib_PDClass_Expand<FrameLib_Kurtosis>::makeClass("fl.kurtosis~");
    FrameLib_PDClass_Expand<FrameLib_Flatness>::makeClass("fl.flatness~");
    FrameLib_PDClass_Expand<FrameLib_RMS>::makeClass("fl.rms~");
    FrameLib_PDClass_Expand<FrameLib_Crest>::makeClass("fl.crest~");
    FrameLib_PDClass_Expand<FrameLib_FrameMinPosition>::makeClass("fl.minpos~");
    FrameLib_PDClass_Expand<FrameLib_FrameMaxPosition>::makeClass("fl.maxpos~");
    FrameLib_PDClass_Expand<FrameLib_NanFilter>::makeClass("fl.nanfilter~");

    // Unary Operators
    
    FrameLib_PDClass_Expand<FrameLib_LogicalNot>::makeClass("fl.not~");
    
    FrameLib_PDClass_Expand<FrameLib_Acos>::makeClass("fl.acos~");
    FrameLib_PDClass_Expand<FrameLib_Asin>::makeClass("fl.asin~");
    FrameLib_PDClass_Expand<FrameLib_Atan>::makeClass("fl.atan~");
    FrameLib_PDClass_Expand<FrameLib_Cos>::makeClass("fl.cos~");
    FrameLib_PDClass_Expand<FrameLib_Sin>::makeClass("fl.sin~");
    FrameLib_PDClass_Expand<FrameLib_Tan>::makeClass("fl.tan~");
    FrameLib_PDClass_Expand<FrameLib_Acosh>::makeClass("fl.acosh~");
    FrameLib_PDClass_Expand<FrameLib_Asinh>::makeClass("fl.asinh~");
    FrameLib_PDClass_Expand<FrameLib_Atanh>::makeClass("fl.atanh~");
    FrameLib_PDClass_Expand<FrameLib_Cosh>::makeClass("fl.cosh~");
    FrameLib_PDClass_Expand<FrameLib_Sinh>::makeClass("fl.sinh~");
    FrameLib_PDClass_Expand<FrameLib_Tanh>::makeClass("fl.tanh~");
    
    FrameLib_PDClass_Expand<FrameLib_Log>::makeClass("fl.log~");
    FrameLib_PDClass_Expand<FrameLib_Log2>::makeClass("fl.log2~");
    FrameLib_PDClass_Expand<FrameLib_Log10>::makeClass("fl.log10~");
    FrameLib_PDClass_Expand<FrameLib_Exp>::makeClass("fl.exp~");
    FrameLib_PDClass_Expand<FrameLib_Exp2>::makeClass("fl.exp2~");
    
    FrameLib_PDClass_Expand<FrameLib_Abs>::makeClass("fl.abs~");
    FrameLib_PDClass_Expand<FrameLib_Ceil>::makeClass("fl.ceil~");
    FrameLib_PDClass_Expand<FrameLib_Floor>::makeClass("fl.floor~");
    FrameLib_PDClass_Expand<FrameLib_Round>::makeClass("fl.round~");
    FrameLib_PDClass_Expand<FrameLib_Trunc>::makeClass("fl.trunc~");
    
    FrameLib_PDClass_Expand<FrameLib_Sqrt>::makeClass("fl.sqrt~");
    FrameLib_PDClass_Expand<FrameLib_Cbrt>::makeClass("fl.cbrt~");
    FrameLib_PDClass_Expand<FrameLib_Erf>::makeClass("fl.erf~");
    FrameLib_PDClass_Expand<FrameLib_Erfc>::makeClass("fl.erfc~");
    
    // Binary Operators
    
    FrameLib_PDClass_Expand<FrameLib_Add, kAllInputs>::makeClass("fl.+~");
    FrameLib_PDClass_Expand<FrameLib_Subtract, kAllInputs>::makeClass("fl.-~");
    FrameLib_PDClass_Expand<FrameLib_Multiply, kAllInputs>::makeClass("fl.*~");
    FrameLib_PDClass_Expand<FrameLib_Divide, kAllInputs>::makeClass("fl./~");
    
    FrameLib_PDClass_Expand<FrameLib_Equal, kAllInputs>::makeClass("fl.==~");
    FrameLib_PDClass_Expand<FrameLib_NotEqual, kAllInputs>::makeClass("fl.!=~");
    FrameLib_PDClass_Expand<FrameLib_GreaterThan, kAllInputs>::makeClass("fl.>~");
    FrameLib_PDClass_Expand<FrameLib_LessThan, kAllInputs>::makeClass("fl.<~");
    FrameLib_PDClass_Expand<FrameLib_GreaterThanEqual, kAllInputs>::makeClass("fl.>=~");
    FrameLib_PDClass_Expand<FrameLib_LessThanEqual, kAllInputs>::makeClass("fl.<=~");
    FrameLib_PDClass_Expand<FrameLib_LogicalAnd, kAllInputs>::makeClass("fl.and~");
    FrameLib_PDClass_Expand<FrameLib_LogicalOr, kAllInputs>::makeClass("fl.or~");
    
    FrameLib_PDClass_Expand<FrameLib_Pow, kAllInputs>::makeClass("fl.pow~");
    FrameLib_PDClass_Expand<FrameLib_Atan2, kAllInputs>::makeClass("fl.atan2~");
    FrameLib_PDClass_Expand<FrameLib_Hypot, kAllInputs>::makeClass("fl.hypot~");
    FrameLib_PDClass_Expand<FrameLib_CopySign, kAllInputs>::makeClass("fl.copysign~");
    FrameLib_PDClass_Expand<FrameLib_Min, kAllInputs>::makeClass("fl.min~");
    FrameLib_PDClass_Expand<FrameLib_Max, kAllInputs>::makeClass("fl.max~");
    FrameLib_PDClass_Expand<FrameLib_Diff, kAllInputs>::makeClass("fl.diff~");
    FrameLib_PDClass_Expand<FrameLib_Modulo, kAllInputs>::makeClass("fl.%~");

    // Ternary  Operators
    
    FrameLib_PDClass_Expand<FrameLib_Clip, kDistribute>::makeClass("fl.clip~");
    FrameLib_PDClass_Expand<FrameLib_Fold, kDistribute>::makeClass("fl.fold~");
    FrameLib_PDClass_Expand<FrameLib_Wrap, kDistribute>::makeClass("fl.wrap~");
    
    // Expressions
    
    FrameLib_PDClass_Expression::makeClass("fl.expr~");
    FrameLib_PDClass_ComplexExpression::makeClass("fl.complex.expr~");
    
    // Complex Unary Operators
    
    FrameLib_PDClass_Expand<FrameLib_Complex_Cos>::makeClass("fl.complex.cos~");
    FrameLib_PDClass_Expand<FrameLib_Complex_Sin>::makeClass("fl.complex.sin~");
    FrameLib_PDClass_Expand<FrameLib_Complex_Tan>::makeClass("fl.complex.tan~");
    FrameLib_PDClass_Expand<FrameLib_Complex_Cosh>::makeClass("fl.complex.cosh~");
    FrameLib_PDClass_Expand<FrameLib_Complex_Sinh>::makeClass("fl.complex.sinh~");
    FrameLib_PDClass_Expand<FrameLib_Complex_Tanh>::makeClass("fl.complex.tanh~");

    FrameLib_PDClass_Expand<FrameLib_Complex_Log>::makeClass("fl.complex.log~");
    FrameLib_PDClass_Expand<FrameLib_Complex_Log10>::makeClass("fl.complex.log10~");
    FrameLib_PDClass_Expand<FrameLib_Complex_Exp>::makeClass("fl.complex.exp~");

    FrameLib_PDClass_Expand<FrameLib_Complex_Sqrt>::makeClass("fl.complex.sqrt~");
    FrameLib_PDClass_Expand<FrameLib_Complex_Conj>::makeClass("fl.complex.conj~");
    
    // Complex Binary Operators
    
    FrameLib_PDClass_Expand<FrameLib_Complex_Add, kAllInputs>::makeClass("fl.complex.plus~");
    FrameLib_PDClass_Expand<FrameLib_Complex_Subtract, kAllInputs>::makeClass("fl.complex.minus~");
    FrameLib_PDClass_Expand<FrameLib_Complex_Multiply, kAllInputs>::makeClass("fl.complex.times~");
    FrameLib_PDClass_Expand<FrameLib_Complex_Divide, kAllInputs>::makeClass("fl.complex.divide~");
    FrameLib_PDClass_Expand<FrameLib_Complex_Pow, kAllInputs>::makeClass("fl.complex.pow~");

    // Buffer
    
    // TODO - info is not correct
    
    FrameLib_PDClass_Expand<FrameLib_Info, kAllInputs>::makeClass("fl.info~");
    FrameLib_PDClass_Read::makeClass<FrameLib_PDClass_Read>("fl.read~");
}
