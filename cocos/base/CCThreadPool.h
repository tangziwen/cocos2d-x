/****************************************************************************
origin from https://github.com/progschj/ThreadPool
 ****************************************************************************/
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include "platform/CCPlatformMacros.h"
#include "base/ccMacros.h"

NS_CC_BEGIN

class ThreadPool {
public:
    ThreadPool(size_t);
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>;
    static ThreadPool* getInstance() { return &s_ThreadPool; }
    ~ThreadPool();
private:
    // need to keep track of threads so we can join them
    std::vector< std::thread > _workers;
    // the task queue
    std::queue< std::function<void()> > _tasks;
    
    // synchronization
    std::mutex _queue_mutex;
    std::condition_variable _condition;
    bool _stop;
    static ThreadPool s_ThreadPool;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
:   _stop(false)
{
    for(size_t i = 0;i<threads;++i)
        _workers.emplace_back(
                             [this]
                             {
                                 for(;;)
                                 {
                                     std::unique_lock<std::mutex> lock(this->_queue_mutex);
                                     while(!this->_stop && this->_tasks.empty())
                                         this->_condition.wait(lock);
                                     if(this->_stop && this->_tasks.empty())
                                         return;
                                     std::function<void()> task(this->_tasks.front());
                                     this->_tasks.pop();
                                     lock.unlock();
                                     task();
                                 }
                             }
                             );
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type>
{
    typedef typename std::result_of<F(Args...)>::type return_type;
    
    // don't allow enqueueing after stopping the pool
    CCASSERT(!_stop, "enqueue on stopped ThreadPool");
    
    auto task = std::make_shared< std::packaged_task<return_type()> >(
                                                                      std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                                                                      );
    
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _tasks.push([task](){ (*task)(); });
    }
    _condition.notify_one();
    return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _stop = true;
    }
    _condition.notify_all();
    for(size_t i = 0;i<_workers.size();++i)
        _workers[i].join();
}

NS_CC_END

#endif
