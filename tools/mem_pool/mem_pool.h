#pragma once
#include <memory>
#include <iostream>


struct mem_node
{

};


class mem_pool
{

    int m_size;
public:
    bool init(int size);
    void * c_new(int len);
    void c_delete(void *p_mem);
    void close();
};