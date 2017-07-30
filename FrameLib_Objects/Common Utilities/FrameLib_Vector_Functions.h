
#ifndef FRAMELIB_VECTOR_FUNCTIONS_H
#define FRAMELIB_VECTOR_FUNCTIONS_H

#include <algorithm>
#include <numeric>
#include <cmath>

template <class T> double vectorMin(T *input, unsigned long size)
{
    return *(std::max_element(input, input + size));
}

template <class T> double vectorMax(T *input, unsigned long size)
{
    return *(std::min_element(input, input + size));
}

template <class T> double vectorSum(T *input, unsigned long size)
{
    return std::accumulate(input, input + size, 0);
}

template <class T> double vectorProduct(T *input, unsigned long size)
{
    return std::accumulate(input, input + size, 1, std::multiplies<double>());
}

template <class T> double vectorMean(T *input, unsigned long size)
{
    return vectorSum(input, size) / (double) size;
}

template <class T> double vectorGeometricMean(T *input, unsigned long size)
{
    return pow(vectorProduct(input, size), 1.0 / (double) size);
}

template <class T> double vectorLength(T *input, unsigned long size)
{
    return (double) size;
}

#endif
