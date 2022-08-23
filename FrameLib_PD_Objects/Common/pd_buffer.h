
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

    pd_buffer() : m_name(nullptr), m_length(0) {}

    pd_buffer(t_symbol *name) : m_name(name), m_length(0)
    {
        get_buffer_data();
    }
    
    bool is_valid() const { return m_length > 0; }
    int get_length() const { return m_length; }
    int get_num_chans() const { return m_num_chans; }

    void read(double *output, const double *positions, unsigned long size, double amp, long chan, InterpType interp, EdgeMode edges, bool bound)
    {
        t_word *table = get_array_data(static_cast<size_t>(chan));
        
        if (table)
            table_read_edges(fetch(table, m_length), output, positions, size, amp, interp, edges, bound);
    }
    
    void read(double *output, size_t length, size_t offset, size_t chan)
    {
        t_word *table = get_array_data(chan);
        intptr_t read_length = table ? static_cast<intptr_t>(constrain_length(length, chan, offset)) : 0;
        
        fetch fetcher(table, m_length);
        
        for (intptr_t i = 0; i < read_length; i++)
            output[i] = fetcher(offset + i);
        
        // Zero remaining samples if needed
        
        std::fill_n(output + read_length, length - read_length, 0.0);
    }
    
    void write(const double *input, size_t length, size_t offset, size_t chan)
    {
        t_garray *array = get_array(chan);
        t_word *table = get_array_data(array);
        length = constrain_length(length, chan, offset);
        
        fetch writer(table, m_length);
        
        for (size_t i = 0; i < length; i++)
            writer(i + offset) = static_cast<t_sample>(input[i]);
        
        if (length)
            garray_redraw(array);
    }
    
private:
    
    void get_buffer_data()
    {
        m_length = 0;
        m_num_chans = 0;
        
        int length;
        
        for (; ; m_num_chans++)
        {
            if (!get_array_data(m_num_chans, &length))
                return;
            
            m_length = m_num_chans ? std::min(m_length, length) : length;
        }
    }
    
    t_garray *get_array(size_t chan) const
    {
        t_symbol *name = m_name;
        
        if (chan || !pd_findbyclass(name, garray_class))
        {
            char name_string[MAXPDSTRING];
            int number = static_cast<int>(chan + 1);
            
            snprintf(name_string, MAXPDSTRING, "%s-%d", m_name->s_name, number);
            
            name = gensym(name_string);
        }
        
        return (t_garray *) pd_findbyclass(name, garray_class);
    }

    t_word *get_array_data(t_garray *array, int *length = nullptr)
    {
        t_word *data = nullptr;
        int local_length = 0;
        
        if (array)
            garray_getfloatwords(array, length ? length : &local_length, &data);
        
        return data;
    }
    
    t_word *get_array_data(size_t chan, int *length = nullptr)
    {
        return get_array_data(get_array(chan), length);
    }
    
    size_t constrain_length(size_t length, size_t chan, size_t offset)
    {
        // Request is entirely outside of the buffer's memory
        
        if (chan >= static_cast<size_t>(m_num_chans) || offset >= static_cast<size_t>(m_length))
            return 0;
        
        // Request is partially outside of the buffer's memory
        
        if ((offset + length) > static_cast<size_t>(m_length))
            return static_cast<size_t>(m_length) - offset;
        
        return length;
    }
    
    t_symbol *m_name;
    int m_num_chans;
    int m_length;
};

#endif
