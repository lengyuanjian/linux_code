#include <iostream>
#include "thread_pool.h"
#include <unistd.h>
#include <atomic>

void *process(void *arg)
{
    printf("thread 0x%x working on task %d\n ",(unsigned int)pthread_self(),*(int *)arg);
    static std::atomic<int>   cout(0);
    for(int i = 0; i < 1000; ++i)
    {
            cout++;
    }
    sleep(1);
    printf("[%d]  thread 0x%x working off task %d\n ",(int)cout,(unsigned int)pthread_self(),*(int *)arg);

    return NULL;
}

int main(void)
{
    
    thread_pool app;
    
    app.init(5);

    app.start();

    int arr[500] ={};
    for(int i = 0; i < 400; )
    {
        arr[i] = i;
        bool ret = app.add_task(process,(void *)(arr + i));
        if(ret)
        {
            ++i;
        }
    }
    
    while(!app.is_task_queue_empty())
    {
        sleep(5); printf("main is wait\n");
    }
    app.stop(); printf("main is close\n");
    
    app.close(); printf("main is end\n");
    
    return 0;
}