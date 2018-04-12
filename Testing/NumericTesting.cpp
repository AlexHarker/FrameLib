
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "FrameLib_FixedPoint.h"
#include "FrameLib_RandGen.h"

// ************************************************************************************** //

// Random Numbers

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

FL_FP randFixed(uint64_t i)
{
    uint64_t hi = randu64();
    uint64_t lo = randu64();
    
    hi >>= (i % 64);
    lo >>= gen.randInt(64);
    
    return FL_FP(hi, lo);
}

FL_SP randSP()
{
    uint64_t hi = randu32();
    uint64_t md = randu64();
    uint64_t lo = randu64();
    
    return FL_SP(hi, md, lo);
}

// ************************************************************************************** //

// Output

void tabbedOut(const std::string& name, const std::string& text, int tab = 25)
{
    std::cout << std::setw(tab) << std::setfill(' ');
    std::cout.setf(std::ios::left);
    std::cout.unsetf(std::ios::right);
    std::cout << name;
    std::cout.unsetf(std::ios::left);
    std::cout << text << "\n";
}

void fixedOut(const std::string& name, FL_FP in)
{
    std::cout << std::setw(25) << std::setfill(' ');
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

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 4, bool fixed = true)
{
    std::ostringstream out;
    if (fixed)
        out << std::setprecision(n) << std::fixed << a_value;
    else
        out << std::setprecision(n) << a_value;
    
    return out.str();
}

std::string getPercentage(uint64_t count, uint64_t total)
{
    double percentage = 100.0 * ((double) count / (double) total);
    
    std::string outStr(to_string_with_precision(percentage, 4, false) + "%");
    
    return outStr;
}

void postPercentage(std::string msg, uint64_t count, uint64_t total)
{
    std::cout << msg << " " << getPercentage(count, total) << "\n";
}

// ************************************************************************************** //

// Helpers for fixed precision tasks

bool operator > (const FL_SP& a, const FL_SP& b)
{
    return ((a.intVal() > b.intVal()) || (a.intVal() == b.intVal() && a.fracHiVal() > b.fracHiVal()) || (a.intVal() == b.intVal() && a.fracHiVal() == b.fracHiVal() && a.fracLoVal() > b.fracLoVal()));
}

bool operator < (const FL_SP& a, const FL_SP& b)
{
    return ((a.intVal() < b.intVal()) || (a.intVal() == b.intVal() && a.fracHiVal() < b.fracHiVal()) || (a.intVal() == b.intVal() && a.fracHiVal() == b.fracHiVal() && a.fracLoVal() < b.fracLoVal()));
}

FL_SP operator - (const FL_SP& a, const FL_SP& b)
{
    uint64_t hi, md, lo;
    
    hi = a.intVal() - b.intVal();
    md = a.fracHiVal() - b.fracHiVal();
    hi = md > a.fracHiVal() ? --hi : hi;
    lo = a.fracLoVal() - b.fracLoVal();
    
    // N.B. - must be able to double carry!
    
    if (lo > a.fracLoVal())
    {
        hi = md ? hi : --hi;
        --md;
    }
    
    return FL_SP(hi, md, lo);
}

FL_SP absDifference(FL_SP a, FL_SP b)
{
    if (a > b)
        return a - b;
    else
        return b - a;
}

// ************************************************************************************** //

// Checks

enum Equality
{
    kExact,
    kEqualFixed,
    kNextDoubleOneULP,
    kOneULP,
    kNextDouble,
    kInaccurate,
};

Equality equalCheck(FL_FP a, double b)
{
    double c  = a;
    double cp = nextafter(c, std::numeric_limits<double>::infinity());
    double cm = nextafter(c, -std::numeric_limits<double>::infinity());
    
    FL_FP d = b;
    
    if (b == c)
        return kExact;
    
    if (a == d)
        return kEqualFixed;
    
    bool ulp = ((a + FL_FP(0U, 1U)) == d) || ((a - FL_FP(0U, 1U)) == d);
    bool next = (b == cp) || (b == cm);
    
    if (ulp && next)
        return kNextDoubleOneULP;
    
    if (ulp)
        return kOneULP;
    
    if (next)
        return kNextDouble;
    
    return kInaccurate;
}

bool roundCheck(uint64_t a)                        { return (a & 0x8000000000000000ULL) ? true : false; }

// ************************************************************************************** //

// Correctness

typedef uint64_t testFunc(uint64_t count);

static int numTests = 0;
static int numPassedTests = 0;

void runTest(std::string name, testFunc func, uint64_t count)
{
    numTests++;
    
    std::cout << name << " - Test Started\n";

    uint64_t done = func(count);

    if (done == count)
    {
        numPassedTests++;
        std::cout << name << " - Test Passed\n";
    }
    else
    {
        postPercentage(name + " : - TEST FAILED AT", done, count);
    }
    
    std::cout << "\n";
}

void testsCompleted()
{
    if (numTests == numPassedTests)
        std::cout << " ***** ALL TESTS PASSED *****\n";
    else
    {
        std::cout << " ***** TESTS FAILED *****\n";
        std::cout << numPassedTests << " out of " << numTests << " passed" << "\n";
    }
}
// ************************************************************************************** //

// Timing

class Timer
{
    
public:
    
    Timer() : mStart(0), mStore1(0), mStore2(0) {}
    
    void start()
    {
        mStart = mach_absolute_time();
    };
    
    void stop(const std::string& msg)
    {
        uint64_t end = mach_absolute_time();
        
        mach_timebase_info_data_t info;
        mach_timebase_info(&info);
        
        uint64_t elapsed = ((end - mStart) * info.numer) / info.denom;
        tabbedOut(msg + " Elapsed ", to_string_with_precision(elapsed / 1000000.0, 2), 35);
        
        mStore2 = mStore1;
        mStore1 = elapsed;
        mStart = mach_absolute_time();
    };
    
    void relative(const std::string& msg)
    {
        tabbedOut(msg + " Comparison ", to_string_with_precision(((double) mStore1 / (double) mStore2), 2), 35);
    }
    
private:
    
    uint64_t        mStart;
    uint64_t        mStore1;
    uint64_t        mStore2;
};

typedef void timingCompareFunc(uint64_t count);

void runTimeCompareTest(std::string name, std::string name1, std::string name2, timingCompareFunc func1, timingCompareFunc func2, uint64_t count)
{
    Timer timeMeasure;
    std::cout << name << " - Test Started\n";
    
    timeMeasure.start();
    func1(count);
    timeMeasure.stop(name1);
    func2(count);
    timeMeasure.stop(name2);
    timeMeasure.relative(name);
    
    std::cout << name << " - Test Completed\n";
    std::cout << "\n";
}

// ************************************************************************************** //

// Correctness tests

uint64_t multCommutationTest(uint64_t count)
{
    // Multiplication commutation test
    
    for (uint64_t i = 0; i < count; i++)
    {
        FL_FP rand1 = randFixed(i);
        FL_FP rand2 = randFixed(i);
        FL_FP result1 = rand1 * rand2;
        FL_FP result2 = rand2 * rand1;
        
        if (result1 != result2)
            return i;
    }
    
    return count;
}

uint64_t  multDoubleComparisonTest(uint64_t count)
{
    // Multiplication double comparison test
    
    uint64_t exact = 0;
    uint32_t equalFP = 0;
    uint32_t nextULP = 0;
    uint32_t nextDouble = 0;
    uint32_t oneULP = 0;
    
    for (uint64_t i = 0; i < count; i++)
    {
        double rand1D = (double) randFixed(i);
        double rand2D = (double) randFixed(i);
        FL_FP rand1 = rand1D;
        FL_FP rand2 = rand2D;
        FL_FP result1 = (rand1 * rand2);
        
        double result2 = rand2D * rand1D;
        
        switch (equalCheck(result1, result2))
        {
            case kExact:            exact++;            break;
            case kEqualFixed:       equalFP++;          break;
            case kNextDouble:       nextDouble++;       break;
            case kNextDoubleOneULP: nextULP++;          break;
            case kOneULP:           oneULP++;           break;
            case kInaccurate:       return i;
        }
    }
    
    tabbedOut("exact double", getPercentage(exact, 0xFFFFFF));
    tabbedOut("equal fixed", getPercentage(equalFP, 0xFFFFFF));
    tabbedOut("one double / ULP", getPercentage(nextULP, 0xFFFFFF));
    tabbedOut("next double", getPercentage(nextDouble, 0xFFFFFF));
    tabbedOut("one ULP out", getPercentage(oneULP, 0xFFFFFF));

    return count;
}

uint64_t  multSPCommutationTest(uint64_t count)
{
    // Multiplication SP commutation test
    
    for (uint64_t i = 0; i < count; i++)
    {
        FL_SP rand1 = randSP();
        FL_SP rand2 = randSP();
        FL_SP result1 = rand1 * rand2;
        FL_SP result2 = rand2 * rand1;
        
        if ((result1.intVal() != result2.intVal()) || (result1.fracHiVal() != result2.fracHiVal()) || (result1.fracLoVal() != result2.fracLoVal()))
            return i;
    }
    
    return count;
}

uint64_t  multSPFixedComparisonTest(uint64_t count)
{
    // SP and Fixed comparison
    
    for (uint64_t i = 0; i < count; i++)
    {
        FL_SP rand1 = randSP();
        FL_SP rand2 = FL_SP(randu64(), randu64(), 0U);
        
        FL_SP result1 = rand1 * rand2;
        FL_SP result2 = qMul(rand1, rand2.intVal(), rand2.fracHiVal());
        
        if ((result1.intVal() != result2.intVal()) || (result1.fracHiVal() != result2.fracHiVal()) || (result1.fracLoVal() != result2.fracLoVal()))
            return i;
    }
    
    return count;
}

uint64_t multQMulFixedComparisonTest(uint64_t count)
{
    // QMul and Fixed comparison
    
    for (uint64_t i = 0; i < count; i++)
    {
        FL_FP rand1 = randFixed(i);
        FL_FP rand2 = randFixed(i);
        
        FL_FP result1 = rand1 * rand2;
        FL_SP result2 = qMul(FL_SP(rand1.intVal(), rand1.fracVal(), 0U), rand2.intVal(), rand2.fracVal());
        
        if ((result1.intVal() != result2.intVal()) || ((result1.fracVal() != result2.fracHiVal()) && !(roundCheck(result2.fracLoVal()) && (result1.fracVal() - 1) == result2.fracHiVal())))
            return i;
    }
    
    return count;
}

uint64_t multSPQMulComparisonTest(uint64_t count)
{
    // SP and QMul comparison
    
    for (uint64_t i = 0; i < count; i++)
    {
        FL_SP rand1 = randSP();
        FL_SP rand2 = FL_SP(randu64(), randu64(), 0U);
        
        FL_SP result1 = rand1 * rand2;
        FL_SP result2 = qMul(rand1, rand2.intVal(), rand2.fracHiVal());
        
        if ((result1.intVal() != result2.intVal()) || (result1.fracHiVal() != result2.fracHiVal()) || (result1.fracLoVal() != result2.fracLoVal()))
            return i;
    }
    
    return count;
}

uint64_t simpleDivideTest(uint64_t count)
{
    uint64_t exact = 0;
    uint64_t between = 0;
    
    for (uint64_t i = 0; i < count; i++)
    {
        FL_FP a = randSmallFixed();
        FL_FP b = randSmallFixed();
        FL_FP result = a / b;
        
        FL_FP check1 = result * b;
        FL_FP check2 = (result + FL_FP(0U,1U)) * b;
        FL_FP check3 = (result - FL_FP(0U,1U)) * b;
        
        if (check1 == a)
            exact++;
        else if (check2 > a && check3 < a)
            between++;
        else
            return i;
    }
    
    tabbedOut("exact", getPercentage(exact, 0xFFFFFF));
    tabbedOut("between", getPercentage(between, 0xFFFFFF));

    return count;
}

uint64_t strictDivideTest(uint64_t count)
{
    // Strict Divide Check
    
    for (uint64_t i = 0; i < count; i++)
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
        
        FL_SP check1 = absDifference(a1, r1 * b1);
        FL_SP check2 = absDifference(a1, rP1 * b1);
        FL_SP check3 = absDifference(a1, rM1 * b1);
        
        // N.B. - equidistance is the maximum error allowed
        
        if (!(check3 < check1) && !(check2 < check1))
            continue;
        
        return i;
    }
    
    return count;
}

uint64_t basicSanity(uint64_t count)
{
    FL_FP One(1,0);
    FL_FP OneULP(0,1);
    FL_FP MaxInt(std::numeric_limits<uint64_t>::max(),0);
    FL_FP MaxVal(std::numeric_limits<uint64_t>::max(),std::numeric_limits<uint64_t>::max());
    FL_FP result;
    bool success = true;
    
    fixedOut("ONE", One);
    fixedOut("ONE ULP", OneULP);
    fixedOut("MAX INT", MaxInt);
    fixedOut("MAX VAL", MaxVal);
    std::cout << "\n";
    result = One * FL_FP(2,0);
    success = result != FL_FP(2,0) ? false : success;
    fixedOut("ONE x TWO (2)", result);
    result = FL_FP(8,0) * FL_FP(5,0);
    success = result != FL_FP(40,0) ? false : success;
    fixedOut("EIGHT x FIVE (HEX 28)", result);
    result = FL_FP(2.5) * FL_FP(17.625);
    success = result != FL_FP(44.0625) ? false : success;
    fixedOut("2.5 x 17.625 (Hex 2c.1)", result);
    std::cout << "\n";
    result = One / OneULP;
    success = result != MaxVal ? false : success;
    fixedOut("ONE / ONE ULP (Max)", result);
    result = One / FL_FP(0,2);
    success = result != FL_FP(0x8000000000000000ULL,0) ? false : success;
    fixedOut("ONE / TWO ULP (Max Bit)", result);
    std::cout << "\n";
    result = OneULP / OneULP;
    success = result != One ? false : success;
    fixedOut("ONE ULP SELF DIV (1)", result);
    result = MaxInt / MaxInt;
    success = result != One ? false : success;
    fixedOut("MAX INT SELF DIV (1)", result);
    result = MaxVal / MaxVal;
    success = result != One ? false : success;
    fixedOut("MAX VAL SELF DIV (1)", result);
        
    return success ? count : 0;
}

// ************************************************************************************** //

// Speed tests

void doubleMultiplySpeedTest(uint64_t count)
{
    double result = 1.0;
    
    for (uint64_t i = 0; i < count; i++)
        result *= 2.5;
    
    if (result == 0.0)
        std::cout << "should not happen\n";
}

void fixedMultiplySpeedTest(uint64_t count)
{
    FL_FP result = FL_FP(1.0);
    
    for (uint64_t i = 0; i < count; i++)
        result *= FL_FP(2U,0x8000000000000000ULL);
    
    if (result.fracVal() == 0)
        std::cout << "should not happen\n";
}

void doubleDivideSpeedTest(uint64_t count)
{
    for (uint64_t i = 0; i < count; i++)
    {
        double result = 1.0 / (i + 2);
        
        if (result == i)
            std::cout << "should not happen\n";
    }
}

void fixedDivideSpeedTest(uint64_t count)
{
    for (uint64_t i = 0; i < count; i++)
    {
        FL_FP result = FL_FP(0U,i) / FL_FP(0U,1U);
        
        if (result != FL_FP(i,0U))
            std::cout << "should not happen\n";
    }
}

void doubleComparisonSpeedTest(uint64_t count)
{
    int countGreater = 0;
    int countLess = 0;
    
    for (uint64_t i = 0; i < count; i++)
    {
        double a = gen.randInt() + gen.randInt();
        double b = gen.randInt() + gen.randInt();
        
        if (a > b)
            countGreater++;
        if (a < b)
            countLess++;
    }
    
    std::cout << "count of greater " << countGreater << " count of less " << countLess << "\n";
}
void fixedComparisonSpeedTest(uint64_t count)
{
    int countGreater = 0;
    int countLess = 0;

    for (uint64_t i = 0; i < count; i++)
    {
        FL_FP a = FL_FP(gen.randInt(), gen.randInt());
        FL_FP b = FL_FP(gen.randInt(), gen.randInt());
        
        if (a > b)
            countGreater++;
        if (a < b)
            countLess++;
    }
    
    std::cout << "count of greater " << countGreater << " count of less " << countLess << "\n";
}

void doubleEqualSpeedTest(uint64_t count)
{
    int countTrue = 0;

    for (uint64_t i = 0; i < count; i++)
    {
        double a = gen.randInt() + gen.randInt();
        double b = gen.randInt() + gen.randInt();
        bool result = a == b;
        
        if (result)
            countTrue++;
    }
    
    std::cout << "count of true " << countTrue << "\n";
}

void fixedEqualSpeedTest(uint64_t count)
{
    int countTrue = 0;

    for (uint64_t i = 0; i < count; i++)
    {
           bool result = FL_FP(gen.randInt(), gen.randInt()) == FL_FP(gen.randInt(), gen.randInt());
        
        if (result)
            countTrue++;
    }
    
    std::cout << "count of true " << countTrue << "\n";
}

// ************************************************************************************** //

// Main

int main(int argc, const char * argv[]) {
    
    std::cout << "BASIC SANITY\n\n";
    runTest("Basic Sanity", &basicSanity, 1);

    std::cout << "CORRECTNESS TESTS\n\n";

    runTest("Multiplication Commutation", &multCommutationTest, 0xFFFFFF);
    runTest("Multiplication SP Commutation", &multSPCommutationTest, 0xFFFFFF);
    runTest("Multiplication Double Comparison", &multDoubleComparisonTest, 0xFFFFFF);
    runTest("Multiplication SP Comparison", &multSPFixedComparisonTest, 0xFFFFFF);
    runTest("Multiplication QMul Comparison", &multQMulFixedComparisonTest, 0xFFFFFF);
    runTest("Multiplication SP QMul Comparison", &multSPQMulComparisonTest, 0xFFFFFF);
    runTest("Simple Divide", &simpleDivideTest, 0xFFFFFF);
    runTest("Strict Divide", &strictDivideTest, 0xFFFFFF);
    
    std::cout << "TIMING TESTS\n\n";
    
    runTimeCompareTest("Comparison Speed", "Double", "Fixed", &doubleComparisonSpeedTest, &fixedComparisonSpeedTest, 0xFFFFFFF);
    runTimeCompareTest("Equal Speed", "Double", "Fixed", &doubleEqualSpeedTest, &fixedEqualSpeedTest, 0xFFFFFFF);
    runTimeCompareTest("Multiply Speed", "Double", "Fixed", &doubleMultiplySpeedTest, &fixedMultiplySpeedTest, 0xFFFFFFF);
    runTimeCompareTest("Divide Speed", "Double", "Fixed", &doubleDivideSpeedTest, &fixedDivideSpeedTest, 0x8FFFFFF);


    testsCompleted();
    
    return 0;
}
