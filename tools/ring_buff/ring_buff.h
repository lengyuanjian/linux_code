#pragma once
#include <atomic>
#include <memory>
#include <iostream>
class ring_buff
{
public:
    ring_buff(int size)
    {
        m_size = size;
        m_buff = new char[m_size]();
    }
    ~ring_buff()
    {
        delete [] m_buff;
    } 
    bool push_data(const char * p_data, int len);

    bool pop_data(char * p_data, int len);
    bool test();
  
  
private:
    
protected:

private:
    //std::unique_ptr<char[]> n_buf;
    char*            m_buff;
    int              m_size{0};
    std::atomic<int> m_r{0};
    std::atomic<int> m_w{0};
};