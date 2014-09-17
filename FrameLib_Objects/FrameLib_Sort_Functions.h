
#ifndef FRAMELIB_SORT_FUNCTIONS_H
#define FRAMELIB_SORT_FUNCTIONS_H

void sort_ascending (double *data, unsigned long size)
{
	double temp;
	unsigned long swaps = 1;
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
		
		for (i = 0, swaps = 0; i + gap < size; i++)
		{
			if (data[i] > data[i + gap])
			{
				temp = data[i];
				data[i] = data[i + gap];
				data[i + gap] = temp;
				swaps = 1;
			}
		}
	}
}

void sort_descending (double *data, unsigned long size)
{
	double temp;
	unsigned long swaps = 1;
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
		
		for (i = 0, swaps = 0; i + gap < size; i++)
		{
			if (data[i] < data[i + gap])
			{
				temp = data[i];
				data[i] = data[i + gap];
				data[i + gap] = temp;
				swaps = 1;
			}
		}
	}
}

void sort_ascending (double *output, double *input, unsigned long size)
{
	for (unsigned long i = 0; i < size; i++)
        output[i] = input[i];
    
    sort_ascending(output, size);
}

void sort_descending (double *output, double *input, unsigned long size)
{
    for (unsigned long i = 0; i < size; i++)
        output[i] = input[i];
    
    sort_descending(output, size);
}

#endif
