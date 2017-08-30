
#ifndef STATISTICS_H
#define STATISTICS_H

template <class T> double vectorMin(T *input, unsigned long size)
{
    return *(std::min_element(input, input + size));
}

template <class T> double vectorMax(T *input, unsigned long size)
{
    return *(std::max_element(input, input + size));
}

template <class T> double vectorSum(T *input, unsigned long size)
{
    return std::accumulate(input, input + size, 0);
}

template <class T> double vectorProduct(T *input, unsigned long size)
{
    return std::accumulate(input, input + size, 1, std::multiplies<double>());
}

template <class T> double vectorLength(T *input, unsigned long size)
{
    return (double) size;
}

// COMPOSITES

template <class T> double vectorMean(T *input, unsigned long size)
{
    return vectorSum(input, size) / (double) size;
}

template <class T> double vectorGeometricMean(T *input, unsigned long size)
{
    // FIX - better done with log summing?
    
    return pow(vectorProduct(input, size), 1.0 / (double) size);
}



#endif /* Statistics_h */
