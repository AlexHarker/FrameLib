
#ifndef __PD_BASE_H__
#define __PD_BASE_H__

#include <m_pd.h>
#include <string>
#include <vector>

// This is a very lightweight C++ template wrapper for writing PD objects as C++ classes
// This work is loosely based on https://github.com/grrrwaaa/maxcpp by Graham Wakefield

class PDClass_Base
{
public:
    
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
    
    template <class T, typename Perform<T>::MethodPerform F> void addPerform(t_signal **sp)
    {
        for (size_t i = 0; i < mSigIns.size(); i++)
            mSigIns[i] = sp[i]->s_vec;
        
        for (size_t i = 0; i < mSigOuts.size(); i++)
            mSigOuts[i] = sp[i + mSigIns.size()]->s_vec;
        
        dsp_add(callPerform<T, F>, 2, this, sp[0]->s_vecsize);
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

    template <class T> static void makeClass(const char *classname)
    {
        t_class **C = getClassPointer<T>();
        
        *C = class_new(gensym(classname), (t_newmethod)create<T>, (t_method)destroy<T>, sizeof(T), 0, A_GIMME, 0);
        T::classInit(*C, classname);
        *accessClassName<T>() = std::string(classname);
        class_sethelpsymbol(*C, gensym(classname));
    }
    
    template <class T> static void *create(t_symbol *sym, long ac, t_atom *av)
    {
        void *x = pd_new(*getClassPointer<T>());
        new(x) T(sym, ac, av);
        return x;
    }
    
    template <class T> static void destroy(t_object * x)
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
    
    const t_sample *getAudioIn(unsigned long idx) { return mSigIns[idx]; }
    t_sample *getAudioOut(unsigned long idx) { return mSigOuts[idx]; }
    
    // Allows type conversion to a t_object
    
    operator t_object&() { return mObject; }
    
    // Allows type conversion to a t_object pointer
    
    operator t_object* () { return (t_object *) this; }
    
private:
    
    // Deleted
    
    PDClass_Base(const PDClass_Base&) = delete;
    PDClass_Base& operator=(const PDClass_Base&) = delete;
    
    // The object structure
    
    t_object mObject;
    std::vector<t_sample *> mSigIns;
    std::vector<t_sample *> mSigOuts;
};
    
#endif
