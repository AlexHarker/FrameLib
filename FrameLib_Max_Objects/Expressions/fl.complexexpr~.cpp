
#include "FrameLib_ComplexExpression.h"
#include "FrameLib_MaxClass.h"

// This object parses it's arguments differently to normal, which is handled by pre-parsing the atoms into a different format

class ArgumentParser
{
    
public:
    
    ArgumentParser(t_symbol *s, long argc, t_atom *argv) : mSymbol(s)
    {
        concatenate(argc, argv);
        
        while (argc--)
        {
            if (atom_getsym(argv) == gensym("/expr"))
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
        t_symbol *sym = atom_getsym(a);
        size_t len = strlen(sym->s_name);
        
        // Input tag
        
        if (len > 2 && sym->s_name[0] == '[' && sym->s_name[len - 1] == ']')
            return true;
        
        // Basic parameter tag test
        
        if (len <= 1 || sym->s_name[0] != '/')
            return false;
     
        // Escape division by known constants
        
        if (sym == gensym("/i") || sym == gensym("/pi") || sym == gensym("/epsilon") || sym == gensym("/e") || sym == gensym("/inf"))
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
            if (atom_gettype(argv) == A_SYM)
                concatenated += atom_getsym(argv)->s_name;
            else
                concatenated += std::to_string(atom_getfloat(argv));
            
            // Add whitespace between symbols
            
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

struct FrameLib_MaxClass_ComplexExpression_Parsed : public FrameLib_MaxClass_Expand<FrameLib_ComplexExpression>
{
    FrameLib_MaxClass_ComplexExpression_Parsed(const ArgumentParser &parsed) :
    FrameLib_MaxClass(parsed.symbol(), parsed.count(), parsed.args(), new FrameLib_MaxProxy()) {}
};

// Max Class (inherits from the parsed version which inherits the standard max class

struct FrameLib_MaxClass_ComplexExpression : public FrameLib_MaxClass_ComplexExpression_Parsed
{
    // Constructor
    
    FrameLib_MaxClass_ComplexExpression(t_symbol *s, long argc, t_atom *argv) :
        FrameLib_MaxClass_ComplexExpression_Parsed(ArgumentParser(s, argc, argv)) {}
};

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_ComplexExpression::makeClass<FrameLib_MaxClass_ComplexExpression>(CLASS_BOX, "fl.complexexpr~");
}
