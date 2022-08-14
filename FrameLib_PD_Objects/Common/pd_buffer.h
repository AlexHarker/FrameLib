
#ifndef PD_BUFFER_H
#define PD_BUFFER_H

#include "../../FrameLib_Dependencies/TableReader.hpp"

class pd_buffer
{
    struct fetch : public table_fetcher<double>
    {
        fetch(t_word *data, intptr_t size)
        : table_fetcher(size, 1.0), mData(data) {}
        
        t_sample& operator()(intptr_t offset)
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
    
    void read(double *output, size_t length, size_t offset, size_t chan)
    {
        intptr_t read_length = static_cast<intptr_t>(constrain_length(length, chan, offset));
        
        fetch fetcher(m_table, m_length);
        
        for (intptr_t i = 0; i < read_length; i++)
            output[i] = fetcher(offset + i);
        
        // Zero remaining samples if needed
        
        std::fill_n(output + read_length, length - read_length, 0.0);
    }
    
    void write(const double *input, size_t length, size_t offset, size_t chan)
    {
        length = constrain_length(length, chan, offset);
        
        fetch writer(m_table, m_length);
        
        for (size_t i = 0; i < length; i++)
            writer(i + offset) = static_cast<t_sample>(input[i]);
        
        if (length)
            garray_redraw(m_array);
    }
    
private:
    
    size_t constrain_length(size_t length, size_t chan, size_t offset)
    {
        // Request is entirely outside of the buffer's memory
        
        if (offset >= static_cast<size_t>(m_length))
            return 0;
        
        // Request is partially outside of the buffer's memory
        
        if ((offset + length) > static_cast<size_t>(m_length))
            return static_cast<size_t>(m_length) - offset;
        
        return length;
    }
    
    t_garray *m_array;
    t_word *m_table;
    int m_length;
};

#endif
