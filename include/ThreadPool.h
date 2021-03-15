#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <list>
#include <mutex>
#include <semaphore.h>
#include <exception>

//#define NON_FIX_FD

#ifdef NON_FIX_FD

template <typename T>
class ThreadPool
{
    public:
        ThreadPool(int threadNum = 8, int maxReq = 10000);
        virtual ~ThreadPool();

        bool append(T* req);
        static void* worker(void* arg);
        void run();
    protected:

    private:
        u_int32_t m_ThreadNum;
        u_int32_t m_MaxReq;
        pthread_t* m_Threads;
        std::list<T*> m_WorkQueue;
        std::mutex m_QueueMutex;
        sem_t m_WorkSem;
        bool m_Stop;
};

template <typename T>
ThreadPool<T> ::ThreadPool(int threadNum,int maxReq):m_ThreadNum(threadNum),m_MaxReq(maxReq),
m_Threads(nullptr),m_Stop(false)
{
    if(threadNum < 0 || m_MaxReq < 0)
    {
        throw std::exception();
    }

    m_Threads = new pthread_t[m_ThreadNum];
    if(!m_Threads)
    {
        throw std::exception();
    }

    sem_init(&m_WorkSem,0,0);


    for(int i = 0;i<threadNum;i++)
    {
        if(pthread_create(m_Threads + i,nullptr,worker,this)!=0)
        {
            delete [] m_Threads;
            sem_destroy(&m_WorkSem);
            throw std::exception();
        }
        if(pthread_detach(m_Threads[i]))
        {
            delete [] m_Threads;
            sem_destroy(&m_WorkSem);
            throw std::exception();
        }
    }
}

template <typename T>
ThreadPool<T> ::~ThreadPool()
{
    delete [] m_Threads;
    sem_destroy(&m_WorkSem);
    m_Stop = true;
}

template <typename T>
bool ThreadPool<T>::append(T* request)
{
    std::lock_guard<std::mutex> guard(m_QueueMutex);
    if(m_WorkQueue.size() > m_MaxReq)
    {
        return false;
    }
    m_WorkQueue.push_back(request);
    sem_post(&m_WorkSem);
    return true;
}

template <typename T>
void* ThreadPool<T>::worker(void* arg)
{
    ThreadPool* pool = (ThreadPool*) arg;
    pool->run();
    return pool;
}

template <typename T>
void ThreadPool<T>::run()
{
    while(!m_Stop)
    {
        sem_wait(m_WorkSem);
        m_QueueMutex.lock();
        if(m_WorkQueue.empty())
        {
            m_QueueMutex.unlock();
            continue;
        }
        T* request = m_WorkQueue.front();
        m_WorkQueue.pop_front();
        m_QueueMutex.unlock();
        if(!request)
        {
            continue;
        }
        request->process();
    }
}
#else
template <typename T>
class ThreadPool
{
    public:
        ThreadPool(int threadNum = 8, int maxReq = 10000);
        virtual ~ThreadPool();

        bool append(T* req);
        static void* worker(void* arg);
        void run(int index);
    protected:

    private:
        u_int32_t m_ThreadNum;
        u_int32_t m_MaxReq;
        pthread_t* m_Threads;
        std::list<T*>* m_WorkQueue;
        std::mutex* m_QueueMutex;
        sem_t* m_WorkSem;
        bool m_Stop;
};

template <typename T>
ThreadPool<T> ::ThreadPool(int threadNum,int maxReq):m_ThreadNum(threadNum),m_MaxReq(maxReq),
m_Threads(nullptr),m_Stop(false)
{
    if(threadNum < 0 || m_MaxReq < 0)
    {
        throw std::exception();
    }

    m_Threads = new pthread_t[m_ThreadNum];
    m_QueueMutex = new std::mutex[m_ThreadNum];
    m_WorkQueue = new std::list<T*>[m_ThreadNum];
    m_WorkSem = new sem_t[m_ThreadNum];
    if(!m_Threads)
    {
        throw std::exception();
    }
    for(int i = 0;i<m_ThreadNum;i++)
    {
        sem_init(m_WorkSem+i,0,0);
    }

    for(int i = 0;i<threadNum;i++)
    {
        if(pthread_create(m_Threads + i,nullptr,worker,this)!=0)
        {
            delete [] m_Threads;
            delete [] m_WorkQueue;
            delete [] m_QueueMutex;
            for(int j = 0;j< threadNum;j++)
            {
                sem_destroy(m_WorkSem + j);
            }
            delete [] m_WorkSem;
            throw std::exception();
        }
        if(pthread_detach(m_Threads[i]))
        {
            delete [] m_Threads;
            delete [] m_WorkQueue;
            delete [] m_QueueMutex;
            for(int j = 0;j< threadNum;j++)
            {
                sem_destroy(m_WorkSem + j);
            }
            delete [] m_WorkSem;
            throw std::exception();
        }
    }
}

template <typename T>
ThreadPool<T> ::~ThreadPool()
{
    delete [] m_Threads;
    delete [] m_WorkQueue;
    delete [] m_QueueMutex;
    for(int j = 0;j< m_ThreadNum;j++)
    {
        sem_destroy(m_WorkSem + j);
    }
    delete [] m_WorkSem;
    m_Stop = true;
}

template <typename T>
bool ThreadPool<T>::append(T* request)
{
    int goal = request->GetSocketFd() % m_ThreadNum;
    std::lock_guard<std::mutex> guard(m_QueueMutex[goal]);
    if(m_WorkQueue[goal].size() > m_MaxReq)
    {
        return false;
    }
    m_WorkQueue[goal].push_back(request);
    sem_post(m_WorkSem + goal);
    return true;
}

template <typename T>
void* ThreadPool<T>::worker(void* arg)
{
    static int i = 0;
    ThreadPool* pool = (ThreadPool*) arg;
    pool->run(i++);
    return pool;
}

template <typename T>
void ThreadPool<T>::run(int index)
{
    while(!m_Stop)
    {
        sem_wait(m_WorkSem + index);
        m_QueueMutex[index].lock();
        if(m_WorkQueue[index].empty())
        {
            m_QueueMutex[index].unlock();
            continue;
        }
        T* request = m_WorkQueue[index].front();
        m_WorkQueue[index].pop_front();
        m_QueueMutex[index].unlock();
        if(!request)
        {
            continue;
        }
        request->process();
    }
}
#endif // NON_FIX_FD

#endif // THREADPOOL_H
