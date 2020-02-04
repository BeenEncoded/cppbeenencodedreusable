#ifndef WORKER_THREAD_BASE_HPP_INCLUDED
#define WORKER_THREAD_BASE_HPP_INCLUDED
#include <thread>
#include <memory>

namespace base
{
    typedef class worker_thread_base worker_thread_base;
    
    void worker_thread(worker_thread_base*);
    void join_thread(std::thread*);
    
    
    /**
     * @class worker_thread_base
     * @author Jonathan Whitlock
     * @date 01/24/2016
     * @file worker_thread_base.hpp
     * @brief This is a worker thread base class that represents a generic worker thread.
     * To use this, simply create a class that inherits, and implement a constructor,
     * destructor, and the do_work function.  In order to change the speed of
     * the worker, a throttle is provided.  The throttle represents how many times per
     * second do_work is called.
     * 
     * This is non-copyable, and non-movable.
     */
    typedef class worker_thread_base
    {
    private:
        worker_thread_base(const worker_thread_base&) = delete;
        worker_thread_base(worker_thread_base&&) = delete;
        
        worker_thread_base& operator=(const worker_thread_base&) = delete;
        worker_thread_base& operator=(worker_thread_base&&) = delete;
    public:
        explicit worker_thread_base();
        virtual ~worker_thread_base();
        
        void start();
        void halt(const unsigned int& = 0);
        
        friend void worker_thread(worker_thread_base*);
        friend void join_thread(std::thread*);
        
    protected:
        virtual void do_work() = 0;
        
        unsigned int throttle; //how many times per second do_work is called
        
    private:
        bool running, stopped;
        std::shared_ptr<std::thread> worker, joiner;
        
    } worker_thread_base;
    
    
}

#endif