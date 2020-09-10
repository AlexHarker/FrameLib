
#include "FrameLib_Expression.h"
#include "FrameLib_MaxClass.h"

#include "expression_object_parsing.h"

// Max Class (inherits from the parsed version which inherits the standard max class)

struct FrameLib_MaxClass_Expression : public FrameLib_MaxClass_ExprParsed<FrameLib_Expression>
{
    // Constructor
    
    FrameLib_MaxClass_Expression(t_object *x, t_symbol *s, long argc, t_atom *argv)
    : FrameLib_MaxClass_ExprParsed<FrameLib_Expression>(x, ExprArgumentParser(x, s, argc, argv, false))
    {}
};

#ifndef FRAMELIB_MAX_SINGLE_OBJECT

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_Expression::makeClass<FrameLib_MaxClass_Expression>("fl.expr~");
}

#endif
