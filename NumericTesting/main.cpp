
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <iostream>
#include <iomanip>
#include "FrameLib_FixedPoint.h"
#include "FrameLib_RandGen.h"

void numberOut(const char *name, FL_FP in)
{
    std::cout << std::setw(10) << std::setfill(' ');
    std::cout.setf(std::ios::left);
    std::cout.unsetf(std::ios::right);
    std::cout << name;
    std::cout << std::setbase(16);
    std::cout << std::setw(16) << std::setfill('0');
    std::cout.unsetf(std::ios::left);
    std::cout.setf(std::ios::right);
    std::cout << in.intVal() << " ";
    std::cout << std::setbase(16);
    std::cout << std::setw(16) << std::setfill('0') << std::setiosflags(std::ios::right);
    std::cout << in.fracVal() << " \n";
}

FrameLib_RandGen gen;

uint64_t randu64()
{
    return (uint64_t) gen.randInt() | (((uint64_t) gen.randInt()) << 0x20);
}

uint64_t randu32()
{
    return (uint64_t) gen.randInt();
}

FL_FP randSmallFixed()
{
    uint64_t hi = randu32() & 0xF;
    uint64_t lo = randu32();
    
    return FL_FP(hi, lo);
}

FL_FP randFixed()
{
    uint64_t hi = randu32();
    uint64_t lo = randu64();
    
    return FL_FP(hi, lo);
}

FL_SP randSP()
{
    uint64_t hi = randu32();
    uint64_t md = randu64();
    uint64_t lo = randu64();
    
    return FL_SP(hi, md, lo);
}

FL_SP diff(FL_SP a, FL_SP b)
{
    if (a > b)
        return a - b;
    else
        return b - a;
}

bool equal(FL_FP a, double b)
{
    double c = a;
    
    if (b == c || (b == nextafter(a, std::numeric_limits<double>::infinity())) || (b == nextafter(a, -std::numeric_limits<double>::infinity())))
        return true;
    
    return false;
}

bool roundCheck(uint64_t a)                        { return (a & 0x8000000000000000ULL) ? true : false; }

int main(int argc, const char * argv[]) {
    
    //FL_FP num = FL_FP(5, 35798);//(9.34567434);
    //FL_FP num = FL_FP(9, 0x587e1d115dcb894c);//(9.34567434);
    //FL_FP denom = FL_FP(3547938458, 0x00000000FEFEFEF1);
    FL_FP denom = FL_FP(0U, 1U);

    //FL_FP num = FL_FP(7892343, 4378529);//(9.34567434);
   // FL_FP denom = FL_FP(9, 0xF0000000FEFEFEF1);
    
    FL_FP num = FL_FP(0U, 87430U);//(9.34567434);

    // Multiplication commutation test
    
    for (uint64_t i=0; i <= 0xFFFFFF; i++)
    {
        FL_FP rand1 = randFixed();
        FL_FP rand2 = randFixed();
        FL_FP result1 = rand1 * rand2;
        FL_FP result2 = rand2 * rand1;
        
        if (result1 != result2)
            std::cout << "Fixed mul not commutative " << i << "\n";
    }
    
    // Multiplication double comparison test
    
    for (uint64_t i=0; i <= 0xFFFFFF; i++)
    {
        double rand1D = (double) randFixed();
        double rand2D = (double) randFixed();
        FL_FP rand1 = rand1D;
        FL_FP rand2 = rand2D;
        FL_FP result1 = (rand1 * rand2);

        double result2 = rand2D * rand1D;
        
        if (!equal(result1, result2))
            std::cout << "Fixed mul not the same as double " << i << "\n";
    }
    
    // Multiplication SP commutation test
    
    for (uint64_t i=0; i <= 0xFFFFFF; i++)
    {
        FL_SP rand1 = randSP();
        FL_SP rand2 = randSP();
        FL_SP result1 = rand1 * rand2;
        FL_SP result2 = rand2 * rand1;
        
        if ((result1.intVal() != result2.intVal()) || (result1.fracHiVal() != result2.fracHiVal()) || (result1.fracLoVal() != result2.fracLoVal()))
            std::cout << "SP mul not commutative " << i << "\n";
    }
    
    // SP and QMul comparison
    
    for (uint64_t i=0; i <= 0xFFFFFF; i++)
    {
        FL_SP rand1 = randSP();
        FL_SP rand2 = FL_SP(randu64(), randu64(), 0U);
        
        FL_SP result1 = rand1 * rand2;
        FL_SP result2 = qMul(rand1, rand2.intVal(), rand2.fracHiVal());
        
        if ((result1.intVal() != result2.intVal()) || (result1.fracHiVal() != result2.fracHiVal()) || (result1.fracLoVal() != result2.fracLoVal()))
            std::cout << "SP mul not equal to qMul " << i << "\n";
    }
    
    // SP and Fixed comparison
    
    for (uint64_t i=0; i <= 0xFFFFFF; i++)
    {
        FL_SP rand1 = randSP();
        FL_SP rand2 = FL_SP(randu64(), randu64(), 0U);
        
        FL_SP result1 = rand1 * rand2;
        FL_SP result2 = qMul(rand1, rand2.intVal(), rand2.fracHiVal());
        
        if ((result1.intVal() != result2.intVal()) || (result1.fracHiVal() != result2.fracHiVal()) || (result1.fracLoVal() != result2.fracLoVal()))
            std::cout << "SP mul not equal to fixed " << i << "\n";
    }
    
    // QMul and Fixed comparison
    
    for (uint64_t i=0; i <= 0xFFFFFF; i++)
    {
        FL_FP rand1 = randFixed();
        FL_FP rand2 = randFixed();
        
        FL_FP result1 = rand1 * rand2;
        FL_SP result2 = qMul(FL_SP(rand1.intVal(), rand1.fracVal(), 0U), rand2.intVal(), rand2.fracVal());
        
        if ((result1.intVal() != result2.intVal()) || ((result1.fracVal() != result2.fracHiVal()) && !(roundCheck(result2.fracLoVal()) && (result1.fracVal() - FUInt64(1U)) == result2.fracHiVal())))
            std::cout << "fixed mul not equal to qMul " << i << "\n";
    }
    
    // Simple Divide Check
    
    uint64_t correct = 0;
    uint64_t between = 0 ;
    
    for (uint64_t i=0; i <= 0xFFFFFF; i++)
    {
        FL_FP a = randSmallFixed();
        FL_FP b = randSmallFixed();
        FL_FP result = a / b;
        
        FL_FP check1 = result * b;
        FL_FP check2 = (result + FL_FP(0U,1U)) * b;
        FL_FP check3 = (result - FL_FP(0U,1U)) * b;
        
        if (check1 == a)
        {
            correct++;
            continue;
        }
        
        if (check2 > a && check3 < a)
        {
            between++;
            continue;
        }
        
        std::cout << "failed\n";

        break;
    }

    std::cout << "correct " << correct << "\n";
    std::cout << "between " << between << "\n";
    std::cout << "done simple\n";
    
    // Hard Divide Check
    
    for (uint64_t i=0; i <= 0xFFFFFF; i++)
    {
        FL_FP a = randSmallFixed();
        FL_FP b = randSmallFixed();
        FL_FP result = a / b;
        FL_FP resultP1 = result + FL_FP(0U,1U);
        FL_FP resultM1 = result - FL_FP(0U,1U);

        FL_SP a1(a.intVal(), a.fracVal(), 0U);
        FL_SP b1(b.intVal(), b.fracVal(), 0U);
        FL_SP r1(result.intVal(), result.fracVal(), 0U);
        FL_SP rP1(resultP1.intVal(), resultP1.fracVal(), 0U);
        FL_SP rM1(resultM1.intVal(), resultM1.fracVal(), 0U);
        
        FL_SP check1 = diff(a1, r1 * b1);
        FL_SP check2 = diff(a1, rP1 * b1);
        FL_SP check3 = diff(a1, rM1 * b1);
        
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

        for (uint64_t i=0; i <= 0xFFFFFFF; i++)
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
        FL_FP result = FL_FP(1.0);
    
        for (uint64_t i=0; i <= 0xFFFFFFF; i++)
            result *= FL_FP(2U,0x8000000000000000ULL);
        
            if (result == FL_FP(0U,0U))
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
    for (uint64_t i=0; i <= 0x8FFFFFF; i++)
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

    for (uint64_t i=0; i <= 0x8FFFFFF; i++)
    {
        FL_FP result = FL_FP(0U,i) / FL_FP(0U,1U);
        
        if (result != FL_FP(i,0U))
            std::cout << "failed " << i << "\n";
    }
    
    end = mach_absolute_time();
    elapsed = end - start;
    elapsed *= info.numer;
    elapsed /= info.denom;
    elapsed2 = elapsed / 1000000.0;
    
    std::cout << "elapsed " << elapsed2 << "\n";
    
    std::cout << "relative " << elapsed2 / elapsed1 << "\n";
    
    FL_FP divtemp = (num / denom);
    
    FL_FP div = divtemp;// + FL_FP(0,0x002E0000);//FL_FP(divtemp.intVal(), divtemp.fractVal() & 0xFFFFFFFF00000000);//- FL_FP(0, 0x1FFFFFFFFULL);
    FL_FP mul = div * denom;
    FL_FP fldiv = ((double) num / (double) denom);
    FL_FP flmul = ((double) fldiv * (double) denom);
    FL_FP flmul2 = fldiv * denom;
    FL_FP mult = FL_FP(56772.567890) * FL_FP(3.5);
    FL_FP mulp = FL_FP(56772.567890 * 3.5);

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
