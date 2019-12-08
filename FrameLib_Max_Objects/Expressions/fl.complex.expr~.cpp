
#include "FrameLib_ComplexExpression.h"
#include "FrameLib_MaxClass.h"

#include "expression_object_parsing.h"

// Max Class (inherits from the parsed version which inherits the standard max class)

struct FrameLib_MaxClass_ComplexExpression : public FrameLib_MaxClass_ExprParsed<FrameLib_ComplexExpression>
{
    // Constructor
    
    FrameLib_MaxClass_ComplexExpression(t_object *x, t_symbol *s, long argc, t_atom *argv) :
        FrameLib_MaxClass_ExprParsed<FrameLib_ComplexExpression>(x, ExprArgumentParser(x, s, argc, argv, true)) {}
};

#ifndef FRAMELIB_MAX_SINGLE_OBJECT

extern "C" int C74_EXPORT main(void)
{
    FrameLib_MaxClass_ComplexExpression::makeClass<FrameLib_MaxClass_ComplexExpression>("fl.complex.expr~");
}

#endif
