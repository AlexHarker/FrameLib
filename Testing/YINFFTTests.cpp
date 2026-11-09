#include "FrameLib_DSP.h"

#define private public
#include "../FrameLib_Objects/Spectral/FrameLib_YINFFT.h"
#undef private

#include <cmath>
#include <iostream>
#include <vector>

class YINFFTTestReader : public FrameLib_Processor
{
public:
    YINFFTTestReader(FrameLib_Context context) : FrameLib_Processor(context, nullptr, nullptr, 2, 0) {}
    const double *read(unsigned i, unsigned long *size) { return getInput(i, size); }
    std::string objectInfo(bool) override { return ""; }
    std::string inputInfo(unsigned long, bool) override { return ""; }
    std::string outputInfo(unsigned long, bool) override { return ""; }

private:
    void process() override {}
};

int main()
{
    FrameLib_Global *global = nullptr;
    FrameLib_Global::get(&global, FrameLib_Thread::defaultPriorities());
    int status = 0;
    {
        int reference;
        FrameLib_Context context(global, &reference);
        FrameLib_YINFFT yin(context, nullptr, nullptr);
        YINFFTTestReader reader(context);
        using Connection = FrameLib_Block::Connection;
        reader.addConnection(Connection(&yin, 0), 0);
        reader.addConnection(Connection(&yin, 1), 1);
        yin.reset(48000, 64);

        constexpr unsigned N = 2048;
        std::vector<double> magnitude(N / 2 + 1);
        const double pi = std::acos(-1.0);
        for (unsigned k = 0; k <= N / 2; ++k)
        {
            double real = 0, imag = 0;
            for (unsigned n = 0; n < N; ++n)
            {
                double signal = std::sin(2 * pi * 440 * n / 48000) * (0.5 - 0.5 * std::cos(2 * pi * n / N));
                real += signal * std::cos(2 * pi * k * n / N);
                imag -= signal * std::sin(2 * pi * k * n / N);
            }
            magnitude[k] = std::hypot(real, imag);
        }

        yin.setFixedInput(0, magnitude.data(), magnitude.size());
        yin.process();
        unsigned long pitchSize = 0, confidenceSize = 0;
        const double *pitch = reader.read(0, &pitchSize);
        const double *confidence = reader.read(1, &confidenceSize);
        if (!pitch || !confidence || pitchSize != 1 || confidenceSize != 1
            || std::abs(*pitch - 440) > 10 || *confidence < 0.8 || *confidence > 1)
        {
            std::cerr << "Unexpected pitch or confidence for a 440 Hz sine\n";
            status = 1;
        }
        else
            std::cout << "pitch=" << *pitch << " confidence=" << *confidence << "\n";

        for (double scale : { 0.1, 10.0 })
        {
            std::vector<double> scaled = magnitude;
            for (double& value : scaled)
                value *= scale;
            yin.setFixedInput(0, scaled.data(), scaled.size());
            yin.process();
            pitch = reader.read(0, &pitchSize);
            confidence = reader.read(1, &confidenceSize);
            if (!pitch || !confidence || pitchSize != 1 || confidenceSize != 1
                || std::abs(*pitch - 440) > 10 || *confidence < 0.8)
            {
                std::cerr << "Unexpected output for scaled spectrum\n";
                status = 1;
            }
        }

        std::fill(magnitude.begin(), magnitude.end(), 0);
        yin.setFixedInput(0, magnitude.data(), magnitude.size());
        yin.process();
        pitch = reader.read(0, &pitchSize);
        confidence = reader.read(1, &confidenceSize);
        if (!pitch || !confidence || pitchSize != 1 || confidenceSize != 1 || *pitch != 0 || *confidence != 0)
        {
            std::cerr << "Unexpected output for silence\n";
            status = 1;
        }

        magnitude.resize(100);
        yin.setFixedInput(0, magnitude.data(), magnitude.size());
        yin.process();
        reader.read(0, &pitchSize);
        reader.read(1, &confidenceSize);
        if (pitchSize || confidenceSize)
        {
            std::cerr << "Invalid spectrum produced output\n";
            status = 1;
        }

    }
    FrameLib_Global::release(&global);
    return status;
}
