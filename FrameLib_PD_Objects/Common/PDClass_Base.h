
#ifndef __PD_BASE_H__
#define __PD_BASE_H__

#include <m_pd.h>
#include <string>
#include <vector>

// This is a very lightweight C++ template wrapper for writing PD objects as C++ classes
// This work is loosely based on https://github.com/grrrwaaa/maxcpp by Graham Wakefield

class PDClass_Base
{
    template <class T>
    struct DefaultArg
    {
        DefaultArg(T value) : mValue(value) {}
        operator T() { return mValue; }
        
        T mValue;
    };
    
    struct ObjectFree
    {
        void operator()(t_pd *ptr) { pd_free(ptr); }
    };

public:
    
    // Types for defining methods with DEFLONG or DEFFLOAT arguments
    
    //using def_int = DefaultArg<t_atom_long>;
    using def_double = DefaultArg<double>;
    
    
    // Unique pointers to t_objects
    
    using unique_pd_ptr = std::unique_ptr<t_pd, ObjectFree>;
    static unique_pd_ptr toUnique(t_pd *ptr) { return unique_pd_ptr(ptr); }
    
    // Clock struct for C++-style usage
    
    struct Clock
    {
        Clock(void *x, t_method fn) { mClock = clock_new(x, fn); }
        ~Clock() { clock_free(mClock); }
        
        void delay(double delaytime = 0.0) { clock_delay(mClock, delaytime); }
        
    private:
        
        t_clock *mClock;
    };
    
    // Qelem struct for C++-style usage
    
    struct Qelem
    {
        typedef void (*IntMethod)(void *x);

        Qelem(void *x, t_method fn)
        : mClock(this, (t_method) &call)
        , mMethod((IntMethod) fn)
        , mOwner(x)
        , mFlag(false)
        {}
        
        void set()
        {
            bool expected = false;
            
            if (mFlag.compare_exchange_strong(expected, true))
                mClock.delay();
        }
        
        static void call(Qelem *a)
        {
            a->mFlag.store(false);
            (*a->mMethod)(a->mOwner);
        }
        
    private:
        
        Clock mClock;
        IntMethod mMethod;
        void *mOwner;
        std::atomic_bool mFlag;
    };
    
    // Default Constructor
    
    PDClass_Base() {}
    
    // Template static functions, defintions to forward Max messages to class methods and methods to add Max methods to the class
    
    static void addMethod(t_class *c, t_method F, const char *name) { class_addmethod(c, F, gensym(name), A_CANT, 0); }

    template <class T> struct NoArgs { typedef void (T::*Method)(); };
    template <class T, typename NoArgs<T>::Method F> static void call(T *x) { ((x)->*F)(); }
    template <class T, typename NoArgs<T>::Method F> static void addMethod(t_class *c, const char *name) { class_addmethod(c, (t_method) call<T, F>, gensym(name), A_NULL); }
    
    template <class T>struct Gimme { typedef void (T::*MethodGimme) (t_symbol *s, long ac, t_atom *av); };
    template <class T, typename Gimme<T>::MethodGimme F> static void call(T *x, t_symbol *s, long ac, t_atom *av) {((x)->*F)(s, ac, av); };
    template <class T, typename Gimme<T>::MethodGimme F> static void addMethod(t_class *c, const char *name) { class_addmethod(c, (t_method) call<T, F>, gensym(name), A_GIMME, 0); }
    
    template <class T> struct Float { typedef void (T::*MethodFloat)(double v); };
    template <class T, typename Float<T>::MethodFloat F> static void call(T *x, double v) { ((x)->*F)(v); }
    template <class T, typename Float<T>::MethodFloat F> static void addMethod(t_class *c, const char *name) { class_addmethod(c, (t_method) call<T, F>, gensym(name), A_FLOAT, 0); }
    
    template <class T> struct DefFloat { typedef void (T::*MethodDefFloat)(def_double v); };
    template <class T, typename DefFloat<T>::MethodDefFloat F> static void call(T *x, double v) { (x->*F)(v); }
    template <class T, typename DefFloat<T>::MethodDefFloat F>
    static void addMethod(t_class *c, const char *name) { auto f = call<T, F>; class_addmethod(c, (t_method) f, gensym(name), A_DEFFLOAT, 0); }
    
    template <class T> struct Sym { typedef void (T::*MethodSym)(t_symbol *s); };
    template <class T, typename Sym<T>::MethodSym F> static void call(T *x, t_symbol *s) { ((x)->*F)(s); }
    template <class T, typename Sym<T>::MethodSym F> static void addMethod(t_class *c, const char *name) { class_addmethod(c, (t_method) call<T, F>, gensym(name), A_DEFSYM, 0); }
    
    // FIX - is this a meaningful thing in PD?
    template <class T> struct Subpatch { typedef void *(T::*MethodSubPatch)(long index, void *arg); };
    template <class T, typename Subpatch<T>::MethodSubPatch F> static void *call(T *x, long index, void *arg) { return ((x)->*F)(index, arg); }
    template <class T, typename Subpatch<T>::MethodSubPatch F> static void addMethod(t_class *c, const char *name) { class_addmethod(c, (t_method) call<T, F>, gensym(name), A_CANT, 0); }

    template <class T> struct DSP { typedef void (T::*MethodDSP)(t_signal **sp); };
    template <class T, typename DSP<T>::MethodDSP F> static void call(T *x, t_signal **sp) { ((x)->*F)(sp); }
    template <class T, typename DSP<T>::MethodDSP F> static void addMethod(t_class *c) { class_addmethod(c, (t_method) call<T, F>, gensym("dsp"), A_CANT, 0); }
    
    template <class T>  struct Perform { typedef void (T::*MethodPerform)(int vec_size); };
    template <class T, typename Perform<T>::MethodPerform F> static t_int *callPerform(t_int *w)
    {
        T *x = (T *) w[1];
        int vec_size = (int) w[2];
        ((x)->*F)(vec_size);
        return w + 3;
    }
    
    template <class T, typename Perform<T>::MethodPerform F>
    void addPerform(t_signal **sp)
    {
        for (size_t i = 0; i < mSigIns.size(); i++)
            mSigIns[i] = sp[i]->s_vec;
        
        for (size_t i = 0; i < mSigOuts.size(); i++)
            mSigOuts[i] = sp[i + mSigIns.size()]->s_vec;
        
        dsp_add(callPerform<T, F>, 2, this, sp[0]->s_vecsize);
    }
    
    // Atom helpers
    
    static t_atomtype atom_gettype(t_atom *a)
    {
        return a->a_type;
    }

    static t_symbol *atom_getsymbol_default(t_atom *a)
    {
        return atom_gettype(a) == A_SYMBOL ? atom_getsymbol(a) : gensym("");
    }
    
    static void atom_setfloat(t_atom *a, double v)
    {
        a->a_type = A_FLOAT;
        a->a_w.w_float = v;
    }
    
    static void atom_setsymbol(t_atom *a, t_symbol *sym)
    {
        a->a_type = A_SYMBOL;
        a->a_w.w_symbol = sym;
    }
    
    // C++ style variadic call to object methods
    
    template <class ReturnType = void *, typename...Args>
    static ReturnType objectMethod(t_object *object, const char* theMethodName, Args...args)
    {
        return objectMethod<ReturnType>(object, gensym(theMethodName), args...);
    }
    
    template <class ReturnType = void *, typename...Args>
    static ReturnType objectMethod(t_object *object, t_symbol* theMethod, Args...args)
    {
        void *pad = nullptr;
        return objectMethod<ReturnType>(object, theMethod, args..., pad);
    }
    
    // Specialisation to prevent infinite padding
    
    template <class ReturnType, class S, class T, class U, class V, class W>
    static ReturnType objectMethod(t_object *object, t_symbol* theMethod, S s, T t, U u, V v, W w)
    {
        typedef void *(*t_fn5)(void *x, void *arg1, void *arg2, void *arg3, void *arg4, void *arg5);

        t_fn5 *m = (t_fn5 *) zgetfn((t_pd *) object, theMethod);
        
        void *ret = (*m)(object,
                         objectMethodArg(s),
                         objectMethodArg(t),
                         objectMethodArg(u),
                         objectMethodArg(v),
                         objectMethodArg(w));
        
        return static_cast<ReturnType>(ret);
    }
    
    // Check if a class exists or not
    
    static bool classExists(const char *name)
    {
        return zgetfn(&pd_objectmaker, gensym(name));
    }

    // Create a named object from code

    template <class T>
    static T *createNamed(const char *name)
    {
        typedef void *(*createFn)(t_symbol *, long, t_atom *);
        
        t_symbol *sym = gensym(name);
        createFn createMethod = (createFn) getfn(&pd_objectmaker, sym);
        
        return reinterpret_cast<T *>((*createMethod)(sym, 0, nullptr));
    }
    
    // Static Methods for class initialisation, object creation and deletion
    
    template <class T> static t_class **getClassPointer()
    {
        static t_class *C;
        
        return &C;
    }
    
    template <class T> static std::string *accessClassName()
    {
        static std::string str;
        
        return &str;
    }

    template <class T>
    static void makeClass(const char *classname, int flags = 0)
    {
        t_class **C = getClassPointer<T>();
        
        *C = class_new(gensym(classname), (t_newmethod)create<T>, (t_method)destroy<T>, sizeof(T), flags, A_GIMME, 0);
        T::classInit(*C, classname);
        *accessClassName<T>() = std::string(classname);
        class_sethelpsymbol(*C, gensym(classname));
    }
    
    template <class T>
    static void *create(t_symbol *sym, long ac, t_atom *av)
    {
        void *x = pd_new(*getClassPointer<T>());
        new(x) T(reinterpret_cast<t_object *>(x), sym, ac, av);
        return x;
    }
    
    template <class T>
    static void destroy(t_object * x)
    {
        ((T *)x)->~T();
    }
    
    // Stub for an object with no additional methods (N.B. - inheriting classes should first call through to the inherited class version)
    
    static void classInit(t_class *c, const char *classname) {}
    
    static void dspInit(t_class *c) { class_addmethod(c, nullfn, gensym("signal"), A_NULL); }
    
    void dspSetup(unsigned long numSigIns, unsigned long numSigOuts)
    {
        mSigIns.resize(numSigIns);
        mSigOuts.resize(numSigOuts);
        
        // Create signal inlets
        
        for (unsigned long i = 0; numSigIns && i < (numSigIns - 1); i++)
            signalinlet_new(*this, 0.0);
        
        // Create signal outlets
        
        for (unsigned long i = 0; i < numSigOuts; i++)
            outlet_new(*this, gensym("signal"));
    }
    
    bool dspIsRunning()
    {
        return canvas_dspstate;
    }
    
    bool dspSetBroken()
    {
        if (!dspIsRunning())
            return false;
        
        canvas_update_dsp();
        return true;
    }

    const t_sample *getAudioIn(unsigned long idx) { return mSigIns[idx]; }
    t_sample *getAudioOut(unsigned long idx) { return mSigOuts[idx]; }
    
    // Allows type conversion to a t_object
    
    operator t_object&() { return mObject; }
    
    // Allows type conversion to a t_object pointer
    
    operator t_object* () { return (t_object *) this; }
    
private:
    
    template <class T>
    static void *objectMethodArg(T a)
    {
        static_assert(sizeof(T) == sizeof(void *), "Argument is not the correct size");
        return reinterpret_cast<void *>(a);
    }
    
    // Deleted
    
    PDClass_Base(const PDClass_Base&) = delete;
    PDClass_Base& operator=(const PDClass_Base&) = delete;
    
    // The object structure
    
    t_object mObject;
    std::vector<t_sample *> mSigIns;
    std::vector<t_sample *> mSigOuts;
};
    
#endif
