
#ifndef FRAMELIB_SORT_FUNCTIONS_H
#define FRAMELIB_SORT_FUNCTIONS_H

void sortAscending (double *data, unsigned long size)
{
    bool swaps = TRUE;
    double temp;
    unsigned long gap = size;
	unsigned long i;
	
	while (gap > 1 || swaps)
	{
		if (gap > 1)
		{
			gap = (gap * 10) / 13;
			if (gap == 9 || gap == 10)
				gap = 11;
			if (gap < 1) gap = 1;
		}
		
		for (i = 0, swaps = FALSE; i + gap < size; i++)
		{
			if (data[i] > data[i + gap])
			{
				temp = data[i];
				data[i] = data[i + gap];
				data[i + gap] = temp;
				swaps = TRUE;
			}
		}
	}
}

void sortDescending (double *data, unsigned long size)
{
	double temp;
	bool swaps = TRUE;
    unsigned long gap = size;
	unsigned long i;
	
	while (gap > 1 || swaps)
	{
		if (gap > 1)
		{
			gap = (gap * 10) / 13;
			if (gap == 9 || gap == 10)
				gap = 11;
			if (gap < 1) gap = 1;
		}
		
		for (i = 0, swaps = FALSE; i + gap < size; i++)
		{
			if (data[i] < data[i + gap])
			{
				temp = data[i];
				data[i] = data[i + gap];
				data[i + gap] = temp;
				swaps = TRUE;
			}
		}
	}
}

void sortAscending (double *output, double *input, unsigned long size)
{
	for (unsigned long i = 0; i < size; i++)
        output[i] = input[i];
    
    sortAscending(output, size);
}

void sortDescending (double *output, double *input, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
        output[i] = input[i];
    
    sortDescending(output, size);
}


void sortAscending (unsigned long *indices, double *data, unsigned long size)
{
    bool swaps = TRUE;
    unsigned long gap = size;
    unsigned long gapIndex;
    unsigned long index;
    unsigned long i;
    
    for (i = 0; i < size; i++)
        indices[i] = i;
    
    while (gap > 1 || swaps)
    {
        if (gap > 1)
        {
            gap = (gap * 10) / 13;
            if (gap == 9 || gap == 10)
                gap = 11;
            if (gap < 1) gap = 1;
        }
        
        for (i = 0, swaps = FALSE; i + gap < size; i++)
        {
            index = indices[i];
            gapIndex = indices[i + gap];
            if (data[index] > data[gapIndex])
            {
                indices[i] = gapIndex;
                indices[i + gap] = index;
                swaps = TRUE;
            }
        }
    }
}

void sortDescending (unsigned long *indices, double *data, unsigned long size)
{
    bool swaps = TRUE;
    unsigned long gap = size;
    unsigned long gapIndex;
    unsigned long index;
    unsigned long i;
    
    for (i = 0; i < size; i++)
        indices[i] = i;
    
    while (gap > 1 || swaps)
    {
        if (gap > 1)
        {
            gap = (gap * 10) / 13;
            if (gap == 9 || gap == 10)
                gap = 11;
            if (gap < 1) gap = 1;
        }
        
        for (i = 0, swaps = FALSE; i + gap < size; i++)
        {
            index = indices[i];
            gapIndex = indices[i + gap];
            if (data[index] < data[gapIndex])
            {
                indices[i] = gapIndex;
                indices[i + gap] = index;
                swaps = TRUE;
            }
        }
    }
}
#endif
