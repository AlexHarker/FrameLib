
#ifndef PD_BUFFER_H
#define PD_BUFFER_H

#include "TableReader.hpp"

class pd_buffer
{

    struct fetch : public table_fetcher<double>
    {
        fetch(t_word *data, intptr_t size)
        : table_fetcher(size, 1.0), mData(data) {}
        
        t_sample operator()(intptr_t offset)
        {
            return mData[offset].w_float;
        }
        
        t_word *mData;
    };
    
public:

    pd_buffer() : mArray(nullptr), mTable(nullptr), mLength(0) {}

    pd_buffer(t_symbol *name) : mTable(nullptr), mLength(0)
    {
        mArray = (t_garray *) pd_findbyclass(name, garray_class);

        if (mArray)
            garray_getfloatwords(mArray, &mLength, &mTable);
    }
    
    bool is_valid() const { return mTable && (mLength > 0); }
    int get_length() const { return mLength; }
    
    void read(double *output, const double *positions, unsigned long size, double amp, InterpType interp, EdgeType edges, bool bound)
    {
        table_read_edges(fetch(mTable, mLength), output, positions, size, amp, interp, edges, bound);
    }
    
private:
    
    t_garray *mArray;
    t_word *mTable;
    int mLength;
};

#endif
