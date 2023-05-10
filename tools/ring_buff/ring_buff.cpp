#include "ring_buff.h"
#include <string.h>

bool ring_buff::push_data(const char *p_data, int len)
{
    if (p_data == NULL || len <= 0)
    {
        return false;
    }
    int w = m_w.load();
    int r = m_r.load();
    int capacity = (w >= r) ? (m_size - w + r) : (r - w);

    if (capacity <= len) //必须多一个字节
    {
        return false;
    }

    if (m_size - w >= len)
    {
        memcpy(m_buff + w, p_data, len);
        w += len;
        if (w == m_size) w = 0;
    }
    else
    {
        int tail_capacity = m_size - w;
        memcpy(m_buff + w, p_data, tail_capacity);
        w = len - tail_capacity;
        memcpy(m_buff, p_data + tail_capacity, w);
        
    }

    m_w.store(w);

    return true;

}

bool ring_buff::pop_data(char *p_data, int len)
{
    if (p_data == NULL || len <= 0)
    {
        return false;
    }

    int w = m_w.load();
    int r = m_r.load();
    int data_len = (w >= r) ? (w - r) : (m_size - r + w);

    if (data_len < len)
    {
        return false;
    }

    if (m_size - r >= len)
    {
        memcpy(p_data, m_buff + r, len);
        r += len;
        if (r == m_size) r = 0;
    }
    else
    {
        int head_capacity = m_size - r;
        memcpy(p_data, m_buff + r, head_capacity);
        r = len - head_capacity;
        memcpy(p_data + head_capacity, m_buff, r);
        //test();
    }
    
    m_r.store(r);

    return true;
}

bool ring_buff::test()
{
    {
        for(int i = 0; i < m_size / 4; ++i)
        {
            std::cout<<((int *)m_buff)[i]<<",";
        }
        std::cout<<"\n";
    }
    return false;
}
