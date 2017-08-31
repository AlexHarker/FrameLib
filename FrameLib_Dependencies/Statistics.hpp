
#ifndef STATISTICS_H
#define STATISTICS_H

#include <algorithm>
#include <numeric>
#include <cmath>

// Helper functors

struct Pass      { template <class T> T operator()(T a) { return a; } };
struct Square    { template <class T> T operator()(T a) { return a * a; } };
struct Cube      { template <class T> T operator()(T a) { return a * a * a; } };
struct Pow4      { template <class T> T operator()(T a) { return Square()(Square()(a)); } };
struct Absolute  { template <class T> T operator()(T a) { return abs(a); } };
struct Logarithm { template <class T> T operator()(T a) { return log(a); } };

struct AddAbs      { template <class T> T operator()(T sum, T a) { return sum + Absolute()(a); } };
struct AddSquare   { template <class T> T operator()(T sum, T a) { return sum + Square()(a); } };
struct AddLog      { template <class T> T operator()(T sum, T a) { return sum + Logarithm()(a); } };

// Length

template <class T> double statLength(T input, size_t size)
{
    return static_cast<double>(size);
}

// Min / Max Values

template <class T> double statMin(T input, size_t size)
{
    return *(std::min_element(input, input + size));
}

template <class T> double statMax(T input, size_t size)
{
    return *(std::max_element(input, input + size));
}

// Counts

template <class T, typename CountOp> double statCount(T input, size_t size, CountOp op)
{
    size_t count = 0;
    
    for (size_t i; i < size; i++)
        if (op(input[i]))
            count++;
    
    return count;
}

template <class T, typename Op>
struct FixedCompare
{
    FixedCompare(T value, Op op) : mValue(value) {}
    bool operator()(T a) { return Op()(a, mValue); }
    T mValue;
};

template <class T, class U> double statCountAbove(T input, U threshold, size_t size)
{
    return statCount(input, size, FixedCompare<U, std::greater<U> >(threshold));
}

template <class T, class U> double statCountBelow(T input, U threshold, size_t size)
{
    return statCount(input, size, FixedCompare<U, std::less<U> >(threshold));
}

// Ratios

template <class T, class U> double statRatioAbove(T input, U threshold, size_t size)
{
    return statCountAbove(input, threshold, size) / statLength(input, size);
}

template <class T, class U> double statRatioBelow(T input, U threshold, size_t size)
{
    return statCountBelow(input, threshold, size) / statLength(input, size);
}

// Sums

template <class T> double statSum(T input, size_t size)
{
    return std::accumulate(input, input + size, 0.0);
}

template <class T> double statSumAbs(T input, size_t size)
{
    return std::accumulate(input, input + size, 0.0, AddAbs());
}

template <class T> double statSumSquares(T input, size_t size)
{
    return std::accumulate(input, input + size, 0.0, AddSquare());
}

template <class T> double statSumLogs(T input, size_t size)
{
    return std::accumulate(input, input + size, 0.0, AddLog());
}

// Weighted Sums

template <class T, typename Op, typename WeightOp> double statWeightedSum(T input, size_t size, Op op, WeightOp weight)
{
    double sum = 0.0;
    
    for (size_t i = 0; i < size; i++)
        sum += weight(i) * op(input[i]);
    
    return sum;
}

template <class T> double statWeightedSum(T input, size_t size)
{
    return statWeightedSum(input, size, Pass(), Pass());
}

template <class T> double statWeightedSumAbs(T input, size_t size)
{
    return statWeightedSum(input, size, Absolute(), Pass());
}

template <class T> double statWeightedSumSquares(T input, size_t size)
{
    return statWeightedSum(input, size, Square(), Pass());
}

template <class T> double statWeightedSumLogs(T input, size_t size)
{
    return statWeightedSum(input, size, Logarithm(), Pass());
}

// Weighted Sums (by weights)

template <class T, typename Op> double statWeightedSum(T input, T weights, size_t size, Op op)
{
    struct getWeight
    {
        getWeight(T weights) : mWeights(weights) {}
        double operator()(size_t i) { return mWeights[i]; }
        T mWeights;
    };
    
    return statWeightedSum(input, size, op, getWeight(weights));
}

template <class T> double statWeightedSum(T input, T weights, size_t size)
{
    return statWeightedSum(input, weights, size, Pass());
}

template <class T> double statWeightedSumAbs(T input, T weights, size_t size)
{
    return statWeightedSum(input, weights, size, Absolute());
}

template <class T> double statWeightedSumSquares(T input, T weights, size_t size)
{
    return statWeightedSum(input, weights, size, Square());
}

template <class T> double statWeightedSumLogs(T input, T weights, size_t size)
{
    return statWeightedSum(input, weights, size, Logarithm());
}

// Product

template <class T> double statProduct(T input, size_t size)
{
    return std::accumulate(input, input + size, 1, std::multiplies<double>());
}

// Means

template <class T> double statMean(T input, size_t size)
{
    return statSum(input, size) / statLength(input, size);
}

template <class T> double statMeanSquares(T input, size_t size)
{
    return statSumSquares(input, size) / statLength(input, size);
}

template <class T> double statGeometricMean(T input, size_t size)
{
    return exp(statSumLogs(input, size) / statLength(input, size));
}

// Variance

template <class T> double statVariance(T input, size_t size)
{
    struct AddSquareDifference
    {
        AddSquareDifference(double value) : mValue(value) {}
        double operator()(double sum, double a) { return sum + Square()(a - mValue); }
        double mValue;
    };

    return std::accumulate(input, input + size, 0.0, AddSquareDifference(statMean(input, size)));
}
                           
// Standard Deviation

template <class T> double statStandardDeviation(T input, size_t size)
{
    return sqrt(statVariance(input, size));
}

// Shape

template <class T> double statCentroid(T input, size_t size)
{
    return statWeightedSum(input, size) / statSum(input, size);
}
                        
template <class T> double statSpread(T input, size_t size)
{
    struct Weight
    {
        Weight(double value) : mValue(value) {}
        double operator()(size_t i) { return Square()(static_cast<double>(i) - mValue); }
        double mValue;
    };
    
    return statWeightedSum(input, size, Pass(), Weight(statCentroid(input, size))) / statSum(input, size);
}

template <class T> double statSkewness(T input, size_t size)
{
    struct Weight
    {
        Weight(double value) : mValue(value) {}
        double operator()(size_t i) { return Cube()(static_cast<double>(i) - mValue); }
        double mValue;
    };

    return statWeightedSum(input, size, Pass(), Weight(statCentroid(input, size))) / (Cube()(sqrt(statSpread(input, size))) * statSum(input, size));
}

template <class T> double statKurtosis(T input, size_t size)
{
    struct Weight
    {
        Weight(double value) : mValue(value) {}
        double operator()(size_t i) { return Pow4()(static_cast<double>(i) - mValue); }
        double mValue;
    };
    
    return statWeightedSum(input, size, Pass(), Weight(statCentroid(input, size))) / (Square()(statSpread(input, size)) * statSum(input, size));
}
                        
// Flatness

template <class T> double statFlatness(T input, size_t size)
{
    return statGeometricMean(input, size) / statMean(input, size);
}

// RMS

template <class T> double statRMS(T input, size_t size)
{
    return sqrt(statMeanSquares(input, size));
}

// Crest

template <class T> double statCrest(T input, size_t size)
{
    return statMax(input, size) / statRMS(input, size);
}

#endif /* Statistics_h */
