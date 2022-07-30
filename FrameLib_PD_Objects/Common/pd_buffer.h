
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

    pd_buffer() : m_array(nullptr), m_table(nullptr), m_length(0) {}

    pd_buffer(t_symbol *name) : m_table(nullptr), m_length(0)
    {
        m_array = (t_garray *) pd_findbyclass(name, garray_class);

        if (m_array)
            garray_getfloatwords(m_array, &m_length, &m_table);
    }
    
    bool is_valid() const { return m_table && (m_length > 0); }
    int get_length() const { return m_length; }
    
    void read(double *output, const double *positions, unsigned long size, double amp, InterpType interp, EdgeMode edges, bool bound)
    {
        table_read_edges(fetch(m_table, m_length), output, positions, size, amp, interp, edges, bound);
    }
    
private:
    
    t_garray *m_array;
    t_word *m_table;
    int m_length;
};

#endif
