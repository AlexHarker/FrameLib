
#ifndef PD_BUFFER_H
#define PD_BUFFER_H

#include "TableReader.hpp"

struct fetch : public table_fetcher<double>
{
    fetch(t_word *data)
    : table_fetcher(1.0), mData(data) {}
    
    // FIX - guard for out of range
    
    t_sample operator()(intptr_t offset)   { return mData[offset].w_float; }
    
    t_word *mData;
};

class pd_buffer
{
    
public:

    pd_buffer(t_symbol *name) : mTable(NULL), mLength(0)
    {
        mArray = (t_garray *) pd_findbyclass(name, garray_class);

        if (mArray)
            garray_getfloatwords(mArray, &mLength, &mTable);
    }
    
    int get_num_chans() const { return 1; }
    bool is_valid() const { return mTable && (mLength > 0); }
    int get_length() const { return mLength; }
    
    void read(double *output, double *positions, unsigned long size, unsigned long chan, double amp, InterpType interpType)
    {
        table_read(fetch(mTable), output, positions, size, amp, interpType);
    }
    
private:
    
    t_garray *mArray;
    t_word *mTable;
    int mLength;
};

#endif /* pd_buffer_h */
