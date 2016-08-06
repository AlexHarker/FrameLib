//
//  main.cpp
//  NumericTesting
//
//  Created by Alex Harker on 04/08/2016.
//
//

#include <iostream>
#include <iomanip>
#include "FrameLib_FixedPoint.h"


void numberOut(const char *name, FrameLib_FixedPoint in)
{
    std::cout << std::setw(10) << std::setfill(' ');
    std::cout.setf(std::ios::left);
    std::cout.unsetf(std::ios::right);
    std::cout << name;
    std::cout << std::setbase(16);
    std::cout << std::setw(16) << std::setfill('0');
    std::cout.unsetf(std::ios::left);
    std::cout.setf(std::ios::right);
    std::cout << in.intPart() << " ";
    std::cout << std::setbase(16);
    std::cout << std::setw(16) << std::setfill('0') << std::setiosflags(std::ios::right);
    std::cout << in.fracPart() << " \n";
}

int main(int argc, const char * argv[]) {
    
    //FrameLib_FixedPoint num = FrameLib_FixedPoint(5, 35798);//(9.34567434);
    //FrameLib_FixedPoint num = FrameLib_FixedPoint(9, 0x587e1d115dcb894c);//(9.34567434);
    FrameLib_FixedPoint denom = FrameLib_FixedPoint(3547938458, 0x00000000FEFEFEF1);

    FrameLib_FixedPoint num = FrameLib_FixedPoint(7892343, 4378529);//(9.34567434);
   // FrameLib_FixedPoint denom = FrameLib_FixedPoint(9, 0xF0000000FEFEFEF1);
    
    FrameLib_FixedPoint divtemp = (num / denom);
    
    FrameLib_FixedPoint div = divtemp;// + FrameLib_FixedPoint(0,0x002E0000);//FrameLib_FixedPoint(divtemp.intPart(), divtemp.fractPart() & 0xFFFFFFFF00000000);//- FrameLib_FixedPoint(0, 0x1FFFFFFFFULL);
    FrameLib_FixedPoint mul = div * denom;
    FrameLib_FixedPoint fldiv = ((double) num / (double) denom);
    FrameLib_FixedPoint flmul = ((double) fldiv * (double) denom);
    FrameLib_FixedPoint flmul2 = fldiv * denom;
    FrameLib_FixedPoint mult = FrameLib_FixedPoint(56772.567890) * FrameLib_FixedPoint(3.5);
    FrameLib_FixedPoint mulp = FrameLib_FixedPoint(56772.567890 * 3.5);

    numberOut("mul", mul);
    numberOut("num", num);
    numberOut("denom", denom);
    numberOut("div", div);
    numberOut("divd", fldiv);
    numberOut("muld", flmul);
    numberOut("muldd", flmul2);
    numberOut("mulc", mult);
    numberOut("mulp", mulp);
    
    return 0;
}
