
#include <stdint.h>
#include <vector>

#define WINDOW_PI			3.14159265358979323846
#define WINDOW_TWOPI		6.28318530717958647692
#define WINDOW_THREEPI		9.42477796076937971538
#define WINDOW_FOURPI		12.56637061435817295384
#define WINDOW_SIXPI		18.84955592153875943076

template <class T, class Ref>
class WindowFunctions
{
    typedef void (*Func)(T, uint32_t, uint32_t);

    struct Function
    {
        Function(Ref reference, Func func) : mReference(reference), mFunc(func) {}
        
        Ref mReference;
        Func mFunc;
    };
    
public:
    
    void add(Ref reference, Func func) { mFunctions.push_back(Function(reference, func)); }
    
    bool calculate(Ref reference, T window, uint32_t windowSize, uint32_t generateSize)
    {
        for (long i = 0; i < mFunctions.size(); i++)
        {
            if (reference == mFunctions[i].mReference)
            {
                mFunctions[i].mFunc(window, windowSize, generateSize);
                return true;
            }
        }
        
        return false;
    }
    
private:
    
    std::vector<Function> mFunctions;
};

static double normalise(uint32_t pos, uint32_t windowSize)
{
    return static_cast<double>(pos) / static_cast<double>(windowSize);
}

template <class T> void window_rect(T window, uint32_t windowSize, uint32_t generateSize)
{
	for (long i = 0; i < generateSize; i++)
		window[i] = 1;
}

/*
 
 for (unsigned long i = 0; i <= (windowSize >> 1); i++)
 mWindow[i] = (double) i / (double) (windowSize / 2.0);
 for (unsigned long i = (windowSize >> 1) + 1; i <= windowSize; i++)
 mWindow[i] = (double) (((double) windowSize - 1.0) - (double) i) / (double) (windowSize / 2.0);
 break;
 
 }*/


template <class T> void window_triangle(T window, uint32_t windowSize, uint32_t generateSize)
{
    // FIX - this is approximate for now...
    
    for (uint32_t i = 0; i < (windowSize >> 1) && i < generateSize; i++)
        window[i] = (double) i / (double) (generateSize >> 1);
    for (uint32_t i = (windowSize >> 1); i < generateSize; i++)
        window[i] = (double) (((double) generateSize - 1) - (double) i) / (double) (generateSize >> 1);
}

template <class T> void window_hann(T window, uint32_t windowSize, uint32_t generateSize)
{
	for (uint32_t i = 0; i < generateSize; i++)
		window[i] = 0.5 - (0.5 * cos(WINDOW_TWOPI * normalise(i, windowSize)));
}

template <class T> void window_cosine(T window, uint32_t windowSize, uint32_t generateSize)
{
    for (uint32_t i = 0; i < generateSize; i++)
        window[i] = sin(WINDOW_PI * normalise(i, windowSize));
}

template <class T> void window_hamming(T window, uint32_t windowSize, uint32_t generateSize)
{
	for (uint32_t i = 0; i < generateSize; i++)
		window[i] = 0.54347826 - (0.45652174 * cos(WINDOW_TWOPI * normalise(i, windowSize)));
}

template <class T> void window_blackman(T window, uint32_t windowSize, uint32_t generateSize)
{
	for (uint32_t i = 0; i < generateSize; i++)
		window[i] = 0.42659071 - (0.49656062 * cos(WINDOW_TWOPI * normalise(i, windowSize))) + (0.07684867 * cos(WINDOW_FOURPI * normalise(i, windowSize)));
}

template <class T> void window_blackman_62(T window, uint32_t windowSize, uint32_t generateSize)
{
	for (uint32_t i = 0; i < generateSize; i++)
		window[i] = (0.44859f - 0.49364f * cos(WINDOW_TWOPI * normalise(i, windowSize)) + 0.05677f * cos(WINDOW_FOURPI * normalise(i, windowSize)));
}

template <class T> void window_blackman_70(T window, uint32_t windowSize, uint32_t generateSize)
{
	for (uint32_t i = 0; i < generateSize; i++)
		window[i] = (0.42323f - 0.49755f * cos(WINDOW_TWOPI * normalise(i, windowSize)) + 0.07922f * cos(WINDOW_FOURPI * normalise(i, windowSize)));
}

template <class T> void window_blackman_74(T window, uint32_t windowSize, uint32_t generateSize)
{
	for (uint32_t i = 0; i < generateSize; i++)
		window[i] = (0.402217f - 0.49703f * cos(WINDOW_TWOPI * normalise(i, windowSize)) + 0.09892f * cos(WINDOW_FOURPI * normalise(i, windowSize)) - 0.00188 * cos(WINDOW_THREEPI * normalise(i, windowSize)));
}

template <class T> void window_blackman_92(T window, uint32_t windowSize, uint32_t generateSize)
{
	for (uint32_t i = 0; i < generateSize; i++)
		window[i] = (0.35875f - 0.48829f * cos(WINDOW_TWOPI * normalise(i, windowSize)) + 0.14128f * cos(WINDOW_FOURPI * normalise(i, windowSize)) - 0.01168 * cos(WINDOW_THREEPI * normalise(i, windowSize)));
}

template <class T> void window_blackman_harris(T window, uint32_t windowSize, uint32_t generateSize)
{
	for (uint32_t i = 0; i < generateSize; i++)
		window[i] = 0.35875 - (0.48829 * cos(WINDOW_TWOPI * normalise(i, windowSize))) + (0.14128 * cos(WINDOW_FOURPI * normalise(i, windowSize))) - (0.01168 * cos(WINDOW_SIXPI * normalise(i, windowSize)));
}

template <class T> void window_flat_top(T window, uint32_t windowSize, uint32_t generateSize)
{
	for (uint32_t i = 0; i < generateSize; i++)
		window[i] = 0.2810639 - (0.5208972 * cos(WINDOW_TWOPI * normalise(i, windowSize))) + (0.1980399 * cos(WINDOW_FOURPI * normalise(i, windowSize)));
}

template <class T> void window_kaiser(T window, uint32_t windowSize, uint32_t generateSize)
{
    double alpha_bessel_recip;
    double new_term;
    double x_sq;
    double b_func;
    double temp;
    long j;
    
    // First find bessel function of alpha
    
    x_sq = 46.24;
    new_term = 0.25 * x_sq;
    b_func = 1.0;
    j = 2;
    
    // Gives maximum accuracy
    
    while (new_term)
    {
        b_func += new_term;
        alpha_bessel_recip = (1.0 / (4.0 * (double) j * (double) j));
        new_term = new_term * x_sq * (1.0 / (4.0 * (double) j * (double) j));
        j++;
    }
    
    alpha_bessel_recip = 1 / b_func;
    
    // Now create kaiser window
    
    
    // FIX - might not work 100%
    
    for (long i = 0; i < generateSize; i++)
    {
        temp = ((2.0 * (double) i) - ((double) windowSize - 1.0));
        temp = temp / windowSize;
        temp *= temp;
        x_sq = (1 - temp) * 46.24;
        new_term = 0.25 * x_sq;
        b_func = 1;
        j = 2;
        
        // Gives maximum accuracy
        
        while (new_term)
        {
            b_func += new_term;
            new_term = new_term * x_sq * (1.0 / (4.0 * (double) j * (double) j));
            j++;
        }
        window[i] = b_func * alpha_bessel_recip;
    }
}

// The below is incorrect!!!!

template <class T> void window_multisine_tapers(T window, uint32_t windowSize, uint32_t generateSize, uint32_t num_tapers)
{
	for (long j = 0; j < generateSize; j++)
		window[j] = 0.0;
	
	for (long i = 0; i < num_tapers; i++)
	{
		for (long j = 0; j < generateSize; j++)
			window[j] += sin(WINDOW_PI * (double) (i + 1) * (double) (j + 1) / (double) (windowSize + 1));
	}
}

template <class T> void window_msinetaper1(T window, uint32_t windowSize, uint32_t generateSize)
{
	window_multisine_tapers(window, windowSize, generateSize, 1);
}

template <class T> void window_msinetaper2(T window, uint32_t windowSize, uint32_t generateSize)
{
	window_multisine_tapers(window, windowSize, generateSize, 2);
}

template <class T> void window_msinetaper3(T window, uint32_t windowSize, uint32_t generateSize)
{
	window_multisine_tapers(window, windowSize, generateSize, 3);
}

template <class T> void window_msinetaper4(T window, uint32_t windowSize, uint32_t generateSize)
{
	window_multisine_tapers(window, windowSize, generateSize, 4);
}

template <class T> void window_msinetaper5(T window, uint32_t windowSize, uint32_t generateSize)
{
	window_multisine_tapers(window, windowSize, generateSize, 5);
}

template <class T> void window_msinetaper6(T window, uint32_t windowSize, uint32_t generateSize)
{
	window_multisine_tapers(window, windowSize, generateSize, 6);
}

template <class T>
class IndexedWindowFunctions : public WindowFunctions <T, uint32_t>
{
    
public:
    
    enum WindowTypes {kWindowRect, kWindowTriangle, kWindowHann, kWindowHamming, kWindowCosine, kWindowBlackman, kWindowBlackman62, kWindowBlackman70, kWindowBlackman74, kWindowBlackman92, kWindowBlackmanHarris, kWindowFlatTop, kWindowKaiser };
    
    IndexedWindowFunctions()
    {
        WindowFunctions<T, uint>::add(kWindowRect, window_rect);
        WindowFunctions<T, uint>::add(kWindowTriangle, window_triangle);

        WindowFunctions<T, uint>::add(kWindowHann, window_hann);
        WindowFunctions<T, uint>::add(kWindowHamming, window_hamming);
        WindowFunctions<T, uint>::add(kWindowCosine, window_cosine);
        
        WindowFunctions<T, uint>::add(kWindowBlackman, window_blackman);
        WindowFunctions<T, uint>::add(kWindowBlackman62, window_blackman_62);
        WindowFunctions<T, uint>::add(kWindowBlackman70, window_blackman_70);
        WindowFunctions<T, uint>::add(kWindowBlackman74, window_blackman_62);
        WindowFunctions<T, uint>::add(kWindowBlackman92, window_blackman_92);
        WindowFunctions<T, uint>::add(kWindowBlackmanHarris, window_blackman_harris);

        WindowFunctions<T, uint>::add(kWindowFlatTop, window_kaiser);
        
        WindowFunctions<T, uint>::add(kWindowKaiser, window_flat_top);
    }
};
