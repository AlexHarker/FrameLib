
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "FrameLib_Errors.h"
#include "FrameLib_Memory.h"
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

// Correctness

typedef uintptr_t testFunc(uintptr_t count);

static int numTests = 0;
static int numPassedTests = 0;

void runTest(std::string name, testFunc func, uintptr_t count)
{
    numTests++;
    
    std::cout << name << " - Test Started\n";

    uintptr_t done = func(count);

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

typedef void timingCompareFunc(uintptr_t count);

void runTimeCompareTest(std::string name, std::string name1, std::string name2, timingCompareFunc func1, timingCompareFunc func2, uintptr_t count)
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

struct NoNotifier : FrameLib_ErrorReporter::HostNotifier
{
    bool notify(const FrameLib_ErrorReporter::ErrorReport& report) override
    {
        return false;
    }
};

NoNotifier notifier;
FrameLib_ErrorReporter reporter(&notifier);

// Correctness tests

size_t randomSize()
{
    return randu32() & 0xFFFF;
}

uintptr_t globalAllocTest1(uintptr_t count)
{
    std::vector<void *> ptrs(count);
    
    FrameLib_GlobalAllocator allocator(reporter);
    
    for (uintptr_t i = 0; i < count; i++)
        ptrs[i] = allocator.alloc(randomSize());
    
    for (uintptr_t i = 0; i < count; i++)
        allocator.dealloc(ptrs[i]);
    
    return count;
}

uintptr_t globalAllocTest2(uintptr_t count)
{
    FrameLib_GlobalAllocator allocator(reporter);
    
    for (uintptr_t i = 0; i < count; i++)
        allocator.dealloc(allocator.alloc(randomSize()));
    
    return count;
}

uintptr_t globalAllocTest3(uintptr_t count)
{
    std::vector<void *> ptrs(count);
    
    FrameLib_GlobalAllocator allocator(reporter);
    
    for (uintptr_t i = 0; i < count; i++)
        ptrs[i] = allocator.alloc(i);
    
    for (uintptr_t i = 0; i < count; i++)
        allocator.dealloc(ptrs[i]);
    
    for (uintptr_t i = count; i > 0; i--)
        ptrs[i] = allocator.alloc(i);
    
    for (uintptr_t i = count; i > 0; i--)
        allocator.dealloc(ptrs[i]);
    
    return count;
}

uintptr_t localAllocTest1(uintptr_t count)
{
    std::vector<void *> ptrs(count);
    
    FrameLib_GlobalAllocator gAllocator(reporter);
    FrameLib_LocalAllocator allocator(gAllocator);
    
    for (uintptr_t i = 0; i < count; i++)
        ptrs[i] = allocator.alloc(randomSize());
    
    for (uintptr_t i = 0; i < count; i++)
        allocator.dealloc(ptrs[i]);
    
    return count;
}

uintptr_t localAllocTest2(uintptr_t count)
{
    FrameLib_GlobalAllocator gAllocator(reporter);
    FrameLib_LocalAllocator allocator(gAllocator);
    
    for (uintptr_t i = 0; i < count; i++)
        allocator.dealloc(allocator.alloc(randomSize()));
    
    return count;
}

uintptr_t localAllocTest3(uintptr_t count)
{
    std::vector<void *> ptrs(count);
    
    FrameLib_GlobalAllocator gAllocator(reporter);
    FrameLib_LocalAllocator allocator(gAllocator);
    
    for (uintptr_t i = 0; i < count; i++)
        ptrs[i] = allocator.alloc(i);
    
    for (uintptr_t i = 0; i < count; i++)
        allocator.dealloc(ptrs[i]);
    
    for (uintptr_t i = count; i > 0; i--)
        ptrs[i] = allocator.alloc(i);
    
    for (uintptr_t i = count; i > 0; i--)
        allocator.dealloc(ptrs[i]);
    
    return count;
}



// ************************************************************************************** //

// Speed tests

FrameLib_GlobalAllocator globalAllocator(reporter);
FrameLib_LocalAllocator localallocator(globalAllocator);

void mallocAllocTest(uintptr_t count)
{
    std::vector<void *> ptrs(count);
    
    for (uintptr_t i = 0; i < count; i++)
        ptrs[i] = malloc(randomSize());
    
    for (uintptr_t i = 0; i < count; i++)
        free(ptrs[i]);
}

void frameLibGlobalAllocTest(uintptr_t count)
{
    std::vector<void *> ptrs(count);

    for (uintptr_t i = 0; i < count; i++)
        ptrs[i] = globalAllocator.alloc(randomSize());
    
    for (uintptr_t i = 0; i < count; i++)
        globalAllocator.dealloc(ptrs[i]);
}

void frameLibLocalAllocTest(uintptr_t count)
{
    std::vector<void *> ptrs(count);
    
    for (uintptr_t i = 0; i < count; i++)
        ptrs[i] = localallocator.alloc(randomSize());
    
    for (uintptr_t i = 0; i < count; i++)
        localallocator.dealloc(ptrs[i]);
    
    localallocator.clear();
}

void mallocAllocTest2(uintptr_t count)
{
    std::vector<void *> ptrs(count);
    
    for (uintptr_t i = 0; i < count; i++)
        ptrs[i] = malloc(800);
    
    for (uintptr_t i = 0; i < count; i++)
        free(ptrs[i]);
}

void frameLibGlobalAllocTest2(uintptr_t count)
{
    std::vector<void *> ptrs(count);
    
    for (uintptr_t i = 0; i < count; i++)
        ptrs[i] = globalAllocator.alloc(800);
    
    for (uintptr_t i = 0; i < count; i++)
        globalAllocator.dealloc(ptrs[i]);
}

void frameLibLocalAllocTest2(uintptr_t count)
{
    std::vector<void *> ptrs(count);
    
    for (uintptr_t i = 0; i < count; i++)
        ptrs[i] = localallocator.alloc(800);
    
    for (uintptr_t i = 0; i < count; i++)
        localallocator.dealloc(ptrs[i]);
    
    localallocator.clear();
}


// ************************************************************************************** //

// Main

int main(int argc, const char * argv[]) {
    
    std::cout << "CORRECTNESS TESTS\n\n";

    runTest("Global Allocator 1", &globalAllocTest1, 500);
    runTest("Global Allocator 2", &globalAllocTest2, 2000);
    runTest("Global Allocator 3", &globalAllocTest3, 2000);
    
    runTest("Local Allocator 1", &localAllocTest1, 500);
    runTest("Local Allocator 2", &localAllocTest2, 2000);
    runTest("Local Allocator 3", &localAllocTest3, 2000);
    
    std::cout << "TIMING TESTS\n\n";
    
    runTimeCompareTest("Comparison Speed", "malloc", "global", &mallocAllocTest, &frameLibGlobalAllocTest, 200000);
    runTimeCompareTest("Comparison Speed", "malloc", "local", &mallocAllocTest, &frameLibLocalAllocTest, 200000);
    runTimeCompareTest("Comparison Speed", "malloc 2", "global 2", &mallocAllocTest2, &frameLibGlobalAllocTest2, 200000);
    runTimeCompareTest("Comparison Speed", "malloc 2", "local 2", &mallocAllocTest2, &frameLibLocalAllocTest2, 200000);
    

    testsCompleted();
    
    return 0;
}
