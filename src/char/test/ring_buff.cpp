#include "ring_buff.h"
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <unistd.h>

int lgz_init(struct lgz_ring_buff *p_this, int size, void * k_buff)
{
    p_this->m_p_buff = k_buff;
    if (!p_this->m_p_buff) 
    {
        // 内存分配失败
        printf("error kmalloc size[%d]\n", size);
        return -1;
    }
    
    p_this->m_r = (int*)p_this->m_p_buff;
    p_this->m_w = ((int*)(p_this->m_p_buff)) + 1;
    p_this->m_p_data = ((char*)(p_this->m_p_buff)) + 8;
    
    p_this->m_size = size - 8;
    //*(p_this->m_r) = 0;
    //*(p_this->m_w) = 0;

    printf("kmalloc size[%d] data size[%d] r_pos[%d] w_pos[%d]\n", size,p_this->m_size,*(p_this->m_r),*(p_this->m_w));

    return 0;
}
void lgz_close(struct lgz_ring_buff *p_this)
{
    //kfree(p_this->m_p_buff);
}
int lgz_push_data(struct lgz_ring_buff *p_this,const char *p_data, int len)
{
    int w = *(p_this->m_w);
    int r = *(p_this->m_r);
    int capacity = r - w;
    int tail_capacity = 0;
    if (p_data == NULL || len <= 0)
    {
        printf("ring_buff p_data[%lx] data_len[%d]\n",(unsigned long)p_data,len);
        return -1;
    }
    if(w >= r)
    {
        capacity = p_this->m_size - w + r;
    }
    //int capacity = (w >= r) ? (m_size - w + r) : (r - w);

    if (capacity <= len) //必须多一个字节
    {
        printf("ring_buff is full capacity[%d] len[%d] r_pos[%d] w_pos[%d]\n",capacity,len,r,w);
        return -1;
    }

    if (p_this->m_size - w >= len)
    {
        memcpy(p_this->m_p_data + w, p_data, len);
        w += len;
        if (w == p_this->m_size) w = 0;
    }
    else
    {
        tail_capacity = p_this->m_size - w;
        memcpy(p_this->m_p_data + w, p_data, tail_capacity);
        w = len - tail_capacity;
        memcpy(p_this->m_p_data, p_data + tail_capacity, w);
        
    }

    *(p_this->m_w) = w;

    return 0;

}

int lgz_pop_data(struct lgz_ring_buff *p_this, char *p_data, int len)
{
    int w = *(p_this->m_w);
    int r = *(p_this->m_r);
    int data_len =  p_this->m_size - r + w;
    int head_capacity = 0;

    if(w >= r)
    {
       data_len = w - r;
    }
    
    if (data_len < len)
    {
        return -1;
    }


    if (p_this->m_size - r >= len)
    {
        memcpy(p_data, p_this->m_p_data + r, len);
        r += len;
        if (r == p_this->m_size) r = 0;
    }
    else
    {
        head_capacity = p_this->m_size - r;
        memcpy(p_data, p_this->m_p_data + r, head_capacity);
        r = len - head_capacity;
        memcpy(p_data + head_capacity, p_this->m_p_data, r);
        //test();
    }
    
    *(p_this->m_r) = r;

    return 0;
}


