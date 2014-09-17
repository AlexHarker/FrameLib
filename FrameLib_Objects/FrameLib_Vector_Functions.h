
#ifndef FRAMELIB_VECTOR_FUNCTIONS_H
#define FRAMELIB_VECTOR_FUNCTIONS_H

double vectorMin(double *input, unsigned long size)
{
    double min = HUGE_VAL;
    
    for (unsigned i = 0; i < size; i++)
        min = input[i] < min ? input[i] : min;
    
    return min;
}

double vectorMax(double *input, unsigned long size)
{
    double max = -HUGE_VAL;
    
    for (unsigned i = 0; i < size; i++)
        max = input[i] > max ? input[i] : max;
    
    return max;
}

double vectorSum(double *input, unsigned long size)
{
    double sum = 0.0;
    
    for (unsigned i = 0; i < size; i++)
        sum += input[i];
    
    return sum;
}

double vectorProduct(double *input, unsigned long size)
{
    double product = 1.0;
    
    for (unsigned i = 0; i < size; i++)
        product *= input[i];
    
    return product;
}

double vectorMean(double *input, unsigned long size)
{
    return vectorSum(input, size) / (double) size;
}

double vectorGeometricMean(double *input, unsigned long size)
{
    return pow(vectorProduct(input, size), 1.0 / (double) size);
}

double vectorLength(double *input, unsigned long size)
{
    return (double) size;
}

#endif
