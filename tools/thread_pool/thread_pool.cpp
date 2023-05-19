#include "thread_pool.h"
#include <unistd.h>
#include <time.h>
#include <iostream>

void *thread_pool::thred_run(void *arg)
{
    thread_pool * p_this = (thread_pool *)arg;
     
    void *ret = p_this->run(arg);
    
    return ret;
}

void *thread_pool::run(void *arg)
{
    std::cout<<(unsigned int)pthread_self()<<" beg\n";
    while(m_running)
    {
        thread_task task = {NULL, NULL};
        pthread_mutex_lock(&m_lock);
        if(m_queue_front == m_queue_rear)
        {
            struct timespec timeout;
            clock_gettime(CLOCK_REALTIME, &timeout);
            timeout.tv_sec += 1;    
            pthread_cond_timedwait(&m_queue_not_empty, &m_lock, &timeout);
            if(m_queue_front != m_queue_rear)
            {
                task = m_p_task_queue[m_queue_rear];
                m_queue_rear = (m_queue_rear + 1) % m_queue_max_size;
            }
        }
        else
        {
            task = m_p_task_queue[m_queue_rear];
            m_queue_rear = (m_queue_rear + 1) % m_queue_max_size;
        }
        pthread_mutex_unlock(&m_lock);

        if(task.m_fun_task != NULL)
        {
            task.m_fun_task(task.m_arg);
        }
        
    }
    std::cout<<(unsigned int)pthread_self()<<" end\n";
    return arg;
}

thread_pool::thread_pool(/* args */)
{
}

thread_pool::~thread_pool()
{
}

bool thread_pool::init(int thread_num)
{
    if(pthread_mutex_init(&m_lock, NULL) != 0 || pthread_cond_init(&m_queue_not_empty, NULL) != 0)
    {
        return false;
    }

    m_running = true;
    
    m_queue_rear = 0;
    m_queue_front = 0;
    m_queue_max_size=100;
    m_p_task_queue = new thread_task[100];

    m_thread_num = thread_num;
    m_p_threads = new pthread_t[thread_num]();


    return true;
}

bool thread_pool::start()
{
    for(int i = 0; i < m_thread_num; ++i)
    {
        pthread_create(m_p_threads + i, 0, thread_pool::thred_run, (void*)this);
    }
    return true;
}

bool thread_pool::is_task_queue_empty()
{
    pthread_mutex_lock(&m_lock);
    if(m_queue_front == m_queue_rear)
    {
        pthread_mutex_unlock(&m_lock);
        return true;
    }
    pthread_mutex_unlock(&m_lock);
    return false;
}

bool thread_pool::stop()
{
    m_running = false;
    
    for(int i = 0; i < m_thread_num; ++i)
    {
        pthread_join(m_p_threads[i], NULL);
    }

    return true;
}

bool thread_pool::close()
{
    
    pthread_mutex_destroy(&m_lock);
    pthread_cond_destroy(&m_queue_not_empty);

    delete [] m_p_threads;
    delete [] m_p_task_queue;

    return true;
}

bool thread_pool::add_task(fun_task task, void *arg)
{
    pthread_mutex_lock(&m_lock);
    if(((m_queue_front + 1) % m_queue_max_size) == m_queue_rear)
    {
        pthread_mutex_unlock(&m_lock);
        return false;
    }
    m_p_task_queue[m_queue_front].m_fun_task = task;
    m_p_task_queue[m_queue_front].m_arg= arg;
    m_queue_front = (m_queue_front + 1) % m_queue_max_size;
    pthread_mutex_unlock(&m_lock);
    pthread_cond_signal(&m_queue_not_empty);
    return true;
}
