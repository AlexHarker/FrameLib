
#ifndef FRAMELIB_BIQUAD_H
#define FRAMELIB_BIQUAD_H

#include "FrameLib_Filter_Template.h"

namespace FrameLib_Filters
{
    class Biquad : public Filter<Biquad, 5, 1, false>
    {
    public:
        
        Biquad() : a1(0.0), a2(0.0), b0(0.0), b1(0.0), b2(0.0), x1(0.0), x2(0.0), y1(0.0), y2 (0.0) {}
        
        // Filter Implementation
        
        double process(double x);

        void reset();
        
        void updateCoefficients(double A1, double A2, double B0, double B1, double B2, double);

        // Description / Parameters / Modes
        
        static constexpr Description sDescription
        {
            "Applies a biquad filter to the input"
        };
        
        static constexpr ParamType sParameters
        {
            Param("a1", "A1 Coefficients", "Sets the a1 coefficient.", 0.0, None()),
            Param("a2", "A2 Coefficients", "Sets the a2 coefficient.", 0.0, None()),
            Param("b0", "B0 Coefficients", "Sets the b0 coefficient.", 0.0, None()),
            Param("b1", "B1 Coefficients", "Sets the b1 coefficient.", 0.0, None()),
            Param("b2", "B2 Coefficients", "Sets the b2 coefficient.", 0.0, None())
        };
        
        static constexpr ModeType sModes
        {
            Mode("biquad", "Biquad Output", "biquad", &Biquad::process)
        };
        
    private:
        
        // Coefficients / Memories
        
        double a1, a2, b0, b1, b2;
        double x1, x2, y1, y2;
    };
}

using FrameLib_Biquad = FrameLib_Filter<FrameLib_Filters::Biquad>;

#endif
