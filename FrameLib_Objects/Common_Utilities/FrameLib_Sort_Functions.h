
#ifndef FRAMELIB_SORT_FUNCTIONS_H
#define FRAMELIB_SORT_FUNCTIONS_H

#include <algorithm>

template <typename Compare, class T> void sortVector(T *data, unsigned long size)
{
    bool swaps = true;
    unsigned long gap = size;
    unsigned long i;
    
    while (gap > 1 || swaps)
    {
        gap = (gap > 1) ? (gap * 10) / 13 : gap;
        gap = (gap == 9 || gap == 10) ? 11 : gap;
        gap = !gap ? 1 : gap;
        
        for (i = 0, swaps = false; i + gap < size; i++)
        {
            if (Compare()(data[i], data[i + gap]))
            {
                std::swap(data[i], data[i + gap]);
                swaps = true;
            }
        }
    }
}

template <typename Compare, class T> void sortIndices(unsigned long *indices, T *data, unsigned long size)
{
    bool swaps = true;
    unsigned long gap = size;
    unsigned long i;
    
    for (i = 0; i < size; i++)
        indices[i] = i;
    
    while (gap > 1 || swaps)
    {
        gap = (gap > 1) ? (gap * 10) / 13 : gap;
        gap = (gap == 9 || gap == 10) ? 11 : gap;
        gap = !gap ? 1 : gap;
        
        for (i = 0, swaps = false; i + gap < size; i++)
        {
            unsigned long index1 = indices[i];
            unsigned long index2 = indices[i + gap];
            
            if (Compare()(data[index1], data[index2]))
            {
                indices[i] = index2;
                indices[i + gap] = index1;
                swaps = true;
            }
        }
    }
}

template <class T> void sortAscending(T *data, unsigned long size)
{
    sortVector<std::greater<T> >(data, size);
}

template <class T> void sortDescending(T *data, unsigned long size)
{
	sortVector<std::less<T> >(data, size);
}

template <class T> void sortAscending(T *output, const T *input, unsigned long size)
{
    std::copy(input, input + size, output);
    sortAscending(output, size);
}

template <class T> void sortDescending(T *output, const T *input, unsigned long size)
{
    std::copy(input, input + size, output);
    sortDescending(output, size);
}

template <class T> void sortIndicesAscending(unsigned long *indices, const T *data, unsigned long size)
{
    sortIndices<std::greater<T> >(indices, data, size);
}

template <class T> void sortIndicesDescending(unsigned long *indices, const T *data, unsigned long size)
{
    sortIndices<std::less<T> >(indices, data, size);
}


#endif
