//
//  main.cpp
//  NumericTesting
//
//  Created by Alex Harker on 04/08/2016.
//
//

#include <mach/mach.h>
#include <mach/mach_time.h>
#include <iostream>
#include <iomanip>
#include "FrameLib_FixedPoint.h"
#include "FrameLib_RandGen.h"

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

FrameLib_RandGen gen;

FL_UInt64 randu64()
{
    return (FL_UInt64) gen.randInt() | (((FL_UInt64) gen.randInt()) << 0x20);
}

FL_UInt64 randu32()
{
    return (FL_UInt64) gen.randInt();
}

FrameLib_FixedPoint randSmallFixed()
{
    FL_UInt64 hi = randu32() & 0xF;
    FL_UInt64 lo = randu32();
    
    return FrameLib_FixedPoint(hi, lo);
}

FrameLib_FixedPoint randFixed()
{
    FL_UInt64 hi = randu32();
    FL_UInt64 lo = randu64();
    
    return FrameLib_FixedPoint(hi, lo);
}

SuperPrecision randSP()
{
    FL_UInt64 hi = randu32();
    FL_UInt64 md = randu64();
    FL_UInt64 lo = randu64();
    
    return SuperPrecision(hi, md, lo);
}

SuperPrecision diff(SuperPrecision a, SuperPrecision b)
{
    if (a > b)
        return a - b;
    else
        return b - a;
}

bool equal(FrameLib_FixedPoint a, double b)
{
    double c = a;
    
    if (b == c || (b == nextafter(a, std::numeric_limits<double>::infinity())) || (b == nextafter(a, -std::numeric_limits<double>::infinity())))
        return true;
    
    return false;
}

int main(int argc, const char * argv[]) {
    
    //FrameLib_FixedPoint num = FrameLib_FixedPoint(5, 35798);//(9.34567434);
    //FrameLib_FixedPoint num = FrameLib_FixedPoint(9, 0x587e1d115dcb894c);//(9.34567434);
    //FrameLib_FixedPoint denom = FrameLib_FixedPoint(3547938458, 0x00000000FEFEFEF1);
    FrameLib_FixedPoint denom = FrameLib_FixedPoint(0, 1);

    //FrameLib_FixedPoint num = FrameLib_FixedPoint(7892343, 4378529);//(9.34567434);
   // FrameLib_FixedPoint denom = FrameLib_FixedPoint(9, 0xF0000000FEFEFEF1);
    
    FrameLib_FixedPoint num = FrameLib_FixedPoint(0, 87430);//(9.34567434);

    // Multiplication commutation test
    
    for (FL_UInt64 i=0; i <= 0xFFFFFF; i++)
    {
        FrameLib_FixedPoint rand1 = randFixed();
        FrameLib_FixedPoint rand2 = randFixed();
        FrameLib_FixedPoint result1 = rand1 * rand2;
        FrameLib_FixedPoint result2 = rand2 * rand1;
        
        if (result1 != result2)
            std::cout << "Fixed mul not commutative " << i << "\n";
    }
    
    // Multiplication double comparison test
    
    for (FL_UInt64 i=0; i <= 0xFFFFFF; i++)
    {
        double rand1D = (double) randFixed();
        double rand2D = (double) randFixed();
        FrameLib_FixedPoint rand1 = rand1D;
        FrameLib_FixedPoint rand2 = rand2D;
        FrameLib_FixedPoint result1 = (rand1 * rand2);

        double result2 = rand2D * rand1D;
        
        if (!equal(result1, result2))
            std::cout << "Fixed mul not the same as double " << i << "\n";
    }
    
    // Multiplication SP commutation test
    
    for (FL_UInt64 i=0; i <= 0xFFFFFF; i++)
    {
        SuperPrecision rand1 = randSP();
        SuperPrecision rand2 = randSP();
        SuperPrecision result1 = rand1 * rand2;
        SuperPrecision result2 = rand2 * rand1;
        
        if ((result1.intPart() != result2.intPart()) || (result1.fracHiPart() != result2.fracHiPart()) || (result1.fracLoPart() != result2.fracLoPart()))
            std::cout << "SP mul not commutative " << i << "\n";
    }
    
    // SP and QMul comparison
    
    for (FL_UInt64 i=0; i <= 0xFFFFFF; i++)
    {
        SuperPrecision rand1 = randSP();
        SuperPrecision rand2 = SuperPrecision(randu64(), randu64(), 0);
        
        SuperPrecision result1 = rand1 * rand2;
        SuperPrecision result2 = qMul(rand1, rand2.intPart(), rand2.fracHiPart());
        
        if ((result1.intPart() != result2.intPart()) || (result1.fracHiPart() != result2.fracHiPart()) || (result1.fracLoPart() != result2.fracLoPart()))
            std::cout << "SP mul not equal to qMul " << i << "\n";
    }
    
    // SP and Fixed comparison
    /*
    for (FL_UInt64 i=0; i <= 0xFFFFFF; i++)
    {
        SuperPrecision rand1 = randSP();
        SuperPrecision rand2 = SuperPrecision(randu64(), randu64(), 0);
        
        SuperPrecision result1 = rand1 * rand2;
        SuperPrecision result2 = qMul(rand1, rand2.intPart(), rand2.fracHiPart());
        
        if ((result1.intPart() != result2.intPart()) || (result1.fracHiPart() != result2.fracHiPart()) || (result1.fracLoPart() != result2.fracLoPart()))
            std::cout << "SP mul not equal to fixed " << i << "\n";
    }
    
    // QMul and Fixed comparison
    
    for (FL_UInt64 i=0; i <= 0xFFFFFF; i++)
    {
        FrameLib_FixedPoint rand1 = randFixed();
        FrameLib_FixedPoint rand2 = randFixed();
        
        FrameLib_FixedPoint result1 = rand1 * rand2;
        SuperPrecision result2 = qMul(rand1, rand2.intPart(), rand2.fracHiPart());
        
        if ((result1.intPart() != result2.intPart()) || (result1.fracHiPart() != result2.fracHiPart()) || (result2.fracLoPart() != result2.fracLoPart()))
            std::cout << "fixed mul not equal to qMul " << i << "\n";
    }*/
    
    // Simple Divide Check
    
    for (FL_UInt64 i=0; i <= 0xFFFFFF; i++)
    {
        FrameLib_FixedPoint a = randSmallFixed();
        FrameLib_FixedPoint b = randSmallFixed();
        FrameLib_FixedPoint result = a / b;
        
        FrameLib_FixedPoint check1 = result * b;
        FrameLib_FixedPoint check2 = (result + FrameLib_FixedPoint(0,1)) * b;
        FrameLib_FixedPoint check3 = (result - FrameLib_FixedPoint(0,1)) * b;
        
        if (check1 == a)
            continue;
        
        if (check2 > a && check3 < a)
            continue;
        
        std::cout << "failed \n";
        break;
    }

    std::cout << "done simple\n";
    
    // Hard Divide Check
    
    for (FL_UInt64 i=0; i <= 0xFFFFFF; i++)
    {
        FrameLib_FixedPoint a = randSmallFixed();
        FrameLib_FixedPoint b = randSmallFixed();
        FrameLib_FixedPoint result = a / b;
        FrameLib_FixedPoint resultP1 = result + FrameLib_FixedPoint(0,1);
        FrameLib_FixedPoint resultM1 = result - FrameLib_FixedPoint(0,1);

        SuperPrecision a1(a.intPart(), a.fracPart(), 0);
        SuperPrecision b1(b.intPart(), b.fracPart(), 0);
        SuperPrecision r1(result.intPart(), result.fracPart(), 0);
        SuperPrecision rP1(resultP1.intPart(), resultP1.fracPart(), 0);
        SuperPrecision rM1(resultM1.intPart(), resultM1.fracPart(), 0);
        
        SuperPrecision check1 = diff(a1, r1 * b1);
        SuperPrecision check2 = diff(a1, rP1 * b1);
        SuperPrecision check3 = diff(a1, rM1 * b1);
        
        if ((check3 > check1) && (check2 > check1))
            continue;
        
        std::cout << "failed \n";
        break;
    }
    
    std::cout << "done hard\n";
    
    /* Get the timebase info */
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    
    uint64_t        start;
    uint64_t        end;
    uint64_t        elapsed;

    start = mach_absolute_time();
    
    {
        double result = 1.0;

        for (FL_UInt64 i=0; i <= 0xFFFFFFF; i++)
            result *= 2.5;
        
        if (result == 0.0)
            std::cout << "equals zero\n";
    }
    
    end = mach_absolute_time();
    elapsed = end - start;
    elapsed *= info.numer;
    elapsed /= info.denom;
    double elapsed1 = elapsed / 1000000.0;
    std::cout << "elapsed " << elapsed1 << "\n";

    start = mach_absolute_time();

    {
        FrameLib_FixedPoint result = FrameLib_FixedPoint(1.0);
    
        for (FL_UInt64 i=0; i <= 0xFFFFFFF; i++)
            result *= FrameLib_FixedPoint(2,0x8000000000000000ULL);
        
            if (result == FrameLib_FixedPoint(0,0))
                std::cout << "equals zero\n";
    }

    end = mach_absolute_time();
    elapsed = end - start;
    elapsed *= info.numer;
    elapsed /= info.denom;
    double elapsed2 = elapsed / 1000000.0;
    
    std::cout << "elapsed " << elapsed2 << "\n";

    std::cout << "relative " << elapsed2 / elapsed1 << "\n";
    
    start = mach_absolute_time();

    ///                      
    for (FL_UInt64 i=0; i <= 0x8FFFFFF; i++)
    {
        double result = 1.0 / (i + 2);
        
        if (result == i)
            std::cout << "failed " << i << "\n";
    }
    
    end = mach_absolute_time();
    elapsed = end - start;
    elapsed *= info.numer;
    elapsed /= info.denom;
    elapsed1 = elapsed / 1000000.0;
    std::cout << "elapsed " << elapsed1 << "\n";
    
    start = mach_absolute_time();

    for (FL_UInt64 i=0; i <= 0x8FFFFFF; i++)
    {
        FrameLib_FixedPoint result = FrameLib_FixedPoint(0,i) / FrameLib_FixedPoint(0,1);
        
        if (result != FrameLib_FixedPoint(i,0))
            std::cout << "failed " << i << "\n";
    }
    
    end = mach_absolute_time();
    elapsed = end - start;
    elapsed *= info.numer;
    elapsed /= info.denom;
    elapsed2 = elapsed / 1000000.0;
    
    std::cout << "elapsed " << elapsed2 << "\n";
    
    std::cout << "relative " << elapsed2 / elapsed1 << "\n";
    
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
