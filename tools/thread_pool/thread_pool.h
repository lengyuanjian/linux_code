#pragma once
#include <pthread.h>
#include <atomic>

typedef void *(*fun_task)(void *);
typedef struct 
{
    fun_task m_fun_task;                // 函数指针，回调函数 
    void *   m_arg;                    // 上面函数的参数
} thread_task;

class thread_pool
{
private:
    pthread_mutex_t     m_lock;                 /* 用于锁住本结构体 */    
    pthread_cond_t      m_queue_not_empty;      /* 任务队列里不为空时，通知等待任务的线程 */


    thread_task         *m_p_task_queue;          /* 任务队列 */

    volatile int        m_queue_front;  //w        /* task_queue队头下标 */
    volatile int        m_queue_rear;   //r     /* task_queue队尾下标 */
    
    int                 m_queue_max_size;       /* task_queue队列可容纳任务数上限 */

    pthread_t           *m_p_threads;
    int                 m_thread_num;
    std::atomic<bool>   m_running;
    //_Atomic bool        m_running;                       /* 退出标志*/
protected:
    static void * thred_run(void* arg);
    void *run(void* arg);
public:
    thread_pool(/* args */);
    ~thread_pool();
public:
    bool init(int thread_num);
    bool start();
    bool is_task_queue_empty();
    bool stop();
    bool close();
    
    bool add_task(fun_task task, void * arg);

private:

};


