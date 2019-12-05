
#ifndef EXPRESSION_OBJECT_PARSING_H
#define EXPRESSION_OBJECT_PARSING_H

#include "FrameLib_MaxClass.h"

// These objects parses their arguments differently to normal, which is handled by pre-parsing the atoms into a different format

class ExprArgumentParser
{
    
public:
    
    ExprArgumentParser(t_object *object, t_symbol *s, long argc, t_atom *argv, bool complex) : mSymbol(s)
    {
        if (argc && isStreamSpecifier(argv))
        {
            argc--;
            mArgs.push_back(*argv++);
        }
        
        concatenate(argc, argv, complex);
        
        while (argc--)
        {
            if (atom_getsym(argv) == gensym("/expr"))
                concatenate(argc, ++argv, complex);
            else
                mArgs.push_back(*argv++);
        }
    }
    
    t_symbol *symbol() const    { return mSymbol;}
    long count() const          { return static_cast<long>(mArgs.size());}
    t_atom* args() const        { return const_cast<t_atom*>(mArgs.data()); }
    
private:
    
    // Stream Specifier Detector
    
    bool isStreamSpecifier(t_atom *a)
    {
        if (atom_gettype(a) == A_SYM)
        {
            t_symbol *sym = atom_getsym(a);
            return strlen(sym->s_name) > 1 && sym->s_name[0] == '=';
        }
        
        return false;
    }
    
    // Tag Detection
    
    bool isValidTag(t_atom *a, bool complex)
    {
        t_symbol *sym = atom_getsym(a);
        size_t len = strlen(sym->s_name);
        
        // Attribute
        
        if (len > 1 && sym->s_name[0] == '@')
            return true;

        // Input Tag
        
        if (FrameLib_MaxClass<void>::isInputTag(sym))
            return true;
        
        // Context Tag
        
        if (FrameLib_MaxClass<void>::isContextTag(sym))
            return true;
        
        // Basic parameter tag test
        
        if (!FrameLib_MaxClass<void>::isParameterTag(sym))
            return false;
        
        // Escape division by known constants
        
        if ((complex && sym == gensym("/i")) || sym == gensym("/pi") || sym == gensym("/epsilon") || sym == gensym("/e") || sym == gensym("/inf"))
            return false;
        
        // Require a parameter tag to have only letters after the slash
        
        for (const char *c = sym->s_name + 1; *c; c++)
            if (!((*c >= 'a') && (*c <= 'z')) || ((*c >= 'A') && (*c <= 'Z')))
                return false;
        
        return true;
    }
    
    // Concatenator
    
    void concatenate(long& argc, t_atom*& argv, bool complex)
    {
        std::string concatenated;
        
        for (; argc && !isValidTag(argv, complex); argc--, argv++)
        {
            if (atom_gettype(argv) == A_SYM)
                concatenated += atom_getsym(argv)->s_name;
            else
                concatenated += std::to_string(atom_getfloat(argv));
            
            // Add whitespace between atoms
            
            concatenated += " ";
        }
        
        if (concatenated.length())
        {
            t_symbol *arg = gensym(concatenated.c_str());
            mArgs.push_back(t_atom());
            atom_setsym(&mArgs.back(), arg);
        }
    }
    
    t_symbol *mSymbol;
    std::vector<t_atom> mArgs;
};

// This class is a wrapper that allows the parsing to happen correctly

template <class T>
struct FrameLib_MaxClass_ExprParsed : public FrameLib_MaxClass_Expand<T>
{
    FrameLib_MaxClass_ExprParsed(t_object *x, const ExprArgumentParser &parsed) :
    FrameLib_MaxClass_Expand<T>(x, parsed.symbol(), parsed.count(), parsed.args(), new FrameLib_MaxProxy()) {}
};

#endif /* EXPRESSION_OBJECT_PARSING_H */
