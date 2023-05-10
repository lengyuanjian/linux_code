#include <iostream>
#include "ring_buff.h"
//#include "mem_ring_queue.h"
#include <thread>
#include <cstdlib>   // srand, rand
#include <ctime>     // time
#include <string.h>

#define buff_size (1024 * 1024 * 10)

ring_buff ring(buff_size * 4);

void  write_data_thread(int w_len, int block)
{
    //std::cout<<"写入["<<w_len<<"]"<<"max block["<<block<<"]字节 开始\n";
    int *buff = new int[block]();
    int count = 0;
    while(w_len > 0)
    {
        memset(buff, 0, block * 4);
        int random_num = rand() % block + 1;
        if(random_num >= w_len)
            random_num = w_len;
        for(int i = 0; i < random_num; ++i)
        {
            buff[i] = count++;
        }
        while(!ring.push_data((const char *)buff, random_num * 4))
        {
            //std::cout<<"入队失败"<<buff[0]<<" random_num:"<<random_num<<"\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        w_len -= random_num;
    }
    delete [] buff;
    //std::cout<<"写入["<<w_len<<"]"<<"max block["<<block<<"]字节 结束\n";
}

void read_data_thread(int r_len, int block)
{
    //std::cout<<"读取["<<r_len<<"]"<<"max block["<<block<<"]字节 开始\n";
    int *buff = new int[block]();
    int count = 0;
    while(r_len > 0)
    {
        memset(buff, 0, block * 4);
        int random_num = rand() % block + 1;
        if(random_num >= r_len)
            random_num = r_len;
        while(!ring.pop_data((char *)buff, random_num * 4))
        {
            //std::cout<<"出队失败:"<<count<<" random_num:"<<random_num<<"\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        bool ret = false;
        for(int i = 0; i < random_num; ++i)
        {
            if(buff[i] != count)
            {
                ret = true;
                std::cout<<"数据校验异常 "<<count<<"!="<<buff[i]<<" block"<<random_num<<"\n";
            }
            count++;
        }
        if(ret)
        {    
            for(int i = 0; i < random_num; ++i)
            {
                std::cout<<buff[i]<<",";

            }
            std::cout<<"\n";
        }
        r_len -= random_num;
    }
    delete [] buff;
    //std::cout<<"读取["<<r_len<<"]"<<"max block["<<block<<"]字节 结束\n";
}


int main(int argc, char *argv[])
{
    std::cout<<"\nring buff test __cplusplus="<<__cplusplus<<"\n\n";
    
    srand(time(nullptr));


    for(int i = 0; i < 1024; ++i)
    {   
        int w = 1;
        w *= 1024*1024;
        w *= rand() % 1024 + 1;
        w += rand() % 8192 + 1;
        int r = w;
        
        int block_w = rand() % 1024 + 1;
        int block_r = rand() % 1024 + 1;
        printf("反复开始测试(%d):\tw[%d]\tr[%dMb]\tblock_w[%d]\tblock_r[%d]\n",i,w,r/1024/1024,block_w, block_r);
        std::thread write_thread(write_data_thread, w, block_w);
        std::thread read_thread(read_data_thread, r, block_r);
    
        write_thread.join();
        read_thread.join();
    }

    
    return 0;
}