
#ifndef _RANDOMGENERATOR_HPP_
#define _RANDOMGENERATOR_HPP_

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <random>

namespace random_generators
{
    // Basic CMWC Generator

    // A complementary modulo with carry algorithm (proposed by George Marsaglia)
    // Details can be found in Marsaglia, G. (2003). "Random number generators". Journal of Modern Applied Statistical Methods 2
    // See - http://digitalcommons.wayne.edu/cgi/viewcontent.cgi?article=1725&context=jmasm

    // The memory requirement is 34 unsigned 32 bit integers (can be altered using cmwc_lag_size)
    // The period length is currently circa 2^1054 - 1 which shold be more than adequate for most purposes

    // N.B. cmwc_lag_size must be a power of two
    // N.B. cmwc_a_value should be a suitable value according to cmwc_lag_size

    class cmwc
    {
        static constexpr uint32_t cmwc_lag_size = 32;
        static constexpr uint64_t cmwc_a_value = 987655670LL;
        
    public:
        
        inline uint32_t operator()()
        {
            uint32_t i = m_increment;
            uint32_t c = m_carry;
            uint32_t x;
            
            uint64_t t;
            
            i = (i + 1) & (cmwc_lag_size - 1);
            t = cmwc_a_value * m_state[i] + c;
            c = (t >> 32);
            x = static_cast<uint32_t>((t + c) & 0xFFFFFFFF);
            
            if (x < c)
            {
                x++;
                c++;
            }
            
            m_state[i] = (0xFFFFFFFE - x);
            m_increment = i;
            m_carry = c;
            
            return m_state[i];
        }
        
        // Seeding (specific / OS-specific random values)
        
        void seed(uint32_t *init)
        {
            m_increment = (cmwc_lag_size - 1);
            m_carry = 123;
            
            for (uint32_t i = 0; i < cmwc_lag_size; i++)
                m_state[i] = init[i];
        }
        
        void rand_seed()
        {
            uint32_t seeds[cmwc_lag_size];
            
            std::random_device rd;
            
            for (uint32_t i = 0; i < cmwc_lag_size; i++)
                seeds[i] = rd();
        
            seed(seeds);
        }
        
        // State
        
        uint32_t m_increment;
        uint32_t m_carry;
        uint32_t m_state[cmwc_lag_size];
    };
}

template <typename Generator = random_generators::cmwc>
class random_generator
{
public:

    class windowed_gaussian_params
    {
        friend random_generator;
        
    public:
        
        windowed_gaussian_params(double mean, double dev) : m_mean(mean), m_dev(dev)
        {
            constexpr double inf = HUGE_VAL;

            const double a = 1.0 / (dev * sqrt(2.0));
            const double b = -mean * a;
                   
            m_lo = erf(b);
            m_hi = erf(a + b);
            
            // N.B. inf is fine as an input, but nan is not...
            
            m_lo = std::isnan(m_lo) ? erf(-inf) : m_lo;
            m_hi = std::isnan(m_hi) ? erf( inf) : m_hi;
        };
        
        double mean() const { return m_mean; }
        double dev() const  { return m_dev; }

    private:
        
        double m_mean;
        double m_dev;
        double m_lo;
        double m_hi;
    };
    
    random_generator()                  { m_generator.rand_seed(); }
    random_generator(uint32_t *init)    { m_generator.seed(init); }

    // Seeding (specific / random values)
    
    void seed(uint32_t *init)   { m_generator.seed(init); }
    void rand_seed()            { m_generator.rand_seed(); }
    
    // Generate a Single Pseudo-random Unsigned Integer (full range /  in the range [0, n] / in the range [lo, hi])
    
    uint32_t rand_int()
    {
        return m_generator();
    }
    
    uint32_t rand_int(uint32_t n)
    {
        uint32_t used = n;
        uint32_t i;
        
        used |= used >> 1;
        used |= used >> 2;
        used |= used >> 4;
        used |= used >> 8;
        used |= used >> 16;
        
        do
            i = rand_int() & used;   // toss unused bits shortens search
        while (i > n);
        
        return i;
    }
    
    int32_t rand_int(int32_t lo, int32_t hi)
    {
        return lo + rand_int(hi - lo);
    }
    
    // Generate a 32 bit Random Double (in the range [0,1] / in the range [0, n] / in the range [lo, hi])
    
    double rand_double()                        { return rand_int() * 2.32830643653869628906e-10; }
    double rand_double(double n)                { return rand_double() * n; }
    double rand_double(double lo, double hi)    { return lo + rand_double() * (hi - lo); }

    // Generate a 32 bit Random Double of Gaussian Distribution with given Mean / Deviation
    
    double rand_gaussian(double mean, double dev)
    {
        double x, y, R;
        
        rand_gaussians(x, y, R);
        
        return (R * x) * dev + mean;
    }
    
    // Generate two independent gaussians (Mean 0 and Deviation 1)
    
    void rand_gaussians(double& x, double& y)
    {
        double R;
        
        rand_gaussians(x, y, R);
        
        x *= R;
        y *= R;
    }
    
    double rand_windowed_gaussian(const windowed_gaussian_params& params)
    {
        const double r = ltqnorm(0.5 + 0.5 * rand_double(params.m_lo, params.m_hi)) * params.dev() + params.mean();
        return std::max(0.0, std::min(1.0, r));
    }
    
    double rand_windowed_gaussian(double mean, double dev)
    {
        const windowed_gaussian_params params(mean, dev);
        return rand_windowed_gaussian(params);
    }
    
private:

    // Gaussian Helper
    
    void rand_gaussians(double& x, double& y, double& R)
    {
        x = 0.0;
        y = 0.0;
        R = 0.0;
        
        while (R >= 1.0 || R == 0.0)
        {
            x = rand_double(-1.0, 1.0);
            y = rand_double(-1.0, 1.0);
            R = (x * x) + (y * y);
        }
        
        R = sqrt((-2.0 * std::log(R)) / R);
    }
    
    // This is adapted from http://home.online.no/~pjacklam/notes/invnorm/impl/sprouse/ltqnorm.c

    /*
     * Lower tail quantile for standard normal distribution function.
     *
     * This function returns an approximation of the inverse cumulative
     * standard normal distribution function.  I.e., given P, it returns
     * an approximation to the X satisfying P = Pr{Z <= X} where Z is a
     * random variable from the standard normal distribution.
     *
     * The algorithm uses a minimax approximation by rational functions
     * and the result has a relative error whose absolute value is less
     * than 1.15e-9.
     *
     * Author:      Peter J. Acklam
     * Time-stamp:  2002-06-09 18:45:44 +0200
     * E-mail:      jacklam@math.uio.no
     * WWW URL:     http://www.math.uio.no/~jacklam
     *
     */

    double ltqnorm(double p)
    {
        /* Coefficients in rational approximations. */

        constexpr double a[] =
        {
            -3.969683028665376e+01,
            2.209460984245205e+02,
            -2.759285104469687e+02,
            1.383577518672690e+02,
            -3.066479806614716e+01,
            2.506628277459239e+00
        };

        constexpr double b[] =
        {
            -5.447609879822406e+01,
            1.615858368580409e+02,
            -1.556989798598866e+02,
            6.680131188771972e+01,
            -1.328068155288572e+01
        };

        constexpr double c[] =
        {
            -7.784894002430293e-03,
            -3.223964580411365e-01,
            -2.400758277161838e+00,
            -2.549732539343734e+00,
            4.374664141464968e+00,
            2.938163982698783e+00
        };

        constexpr double d[] =
        {
            7.784695709041462e-03,
            3.224671290700398e-01,
            2.445134137142996e+00,
            3.754408661907416e+00
        };

        constexpr double low = 0.02425;
        constexpr double high = 0.97575;
        
        double q, r;
        
        errno = 0;
        
        if (p < 0 || p > 1)
        {
            errno = EDOM;
            return 0.0;
        }
        else if (p == 0)
        {
            errno = ERANGE;
            return -HUGE_VAL    /* minus "infinity" */;
        }
        else if (p == 1)
        {
            errno = ERANGE;
            return HUGE_VAL        /* "infinity" */;
        }
        else if (p < low)
        {
            /* Rational approximation for lower region */
            
            q = sqrt(-2.0*std::log(p));
            return (((((c[0]*q+c[1])*q+c[2])*q+c[3])*q+c[4])*q+c[5]) /
            ((((d[0]*q+d[1])*q+d[2])*q+d[3])*q+1);
        }
        else if (p > high)
        {
            /* Rational approximation for upper region */
            
            q  = sqrt(-2.0*std::log(1-p));
            return -(((((c[0]*q+c[1])*q+c[2])*q+c[3])*q+c[4])*q+c[5]) /
            ((((d[0]*q+d[1])*q+d[2])*q+d[3])*q+1);
        }
        else
        {
            /* Rational approximation for central region */
            
            q = p - 0.5;
            r = q*q;
            return (((((a[0]*r+a[1])*r+a[2])*r+a[3])*r+a[4])*r+a[5])*q /
            (((((b[0]*r+b[1])*r+b[2])*r+b[3])*r+b[4])*r+1);
        }
    }
    // State
    
    Generator m_generator;
};

#endif
