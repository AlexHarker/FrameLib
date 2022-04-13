
#include "ScheduleTest1.h"

#include <iostream>
#include <vector>

// Main

void resize(std::vector<double>& io, std::vector<double *>& pointers, int blockSize, unsigned long size)
{
    io.resize(blockSize * size);
    pointers.resize(size);
    
    for (unsigned long i = 0; i < size; i++)
        pointers[i] = io.data() + blockSize * i;
}

int main(int argc, const char * argv[]) {
    
    ScheduleTest1 testGraph1(new FrameLib_Proxy, { 31, 31, 31, 0, true });
    
    testGraph1.setMultithreading(true);
    
    int blockSize = 16384;
    
    std::vector<double> inputsRaw;
    std::vector<double> outputsRaw;
        
    std::vector<double *> inputs;
    std::vector<double *> outputs;
    
    resize(inputsRaw, inputs, blockSize, testGraph1.getNumAudioIns());
    resize(outputsRaw, outputs, blockSize, testGraph1.getNumAudioOuts());
    
    testGraph1.reset(44100.0, blockSize);
    
    while (1)
        testGraph1.process(inputs.data(), outputs.data(), blockSize);
        
    return 0;
}
