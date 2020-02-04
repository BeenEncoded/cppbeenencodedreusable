#include <thread>
#include <chrono>
#include <exception>
#include <memory>
#include <stdexcept>

#include "worker_thread_base.hpp"

/* worker_thread_base member functions: */
namespace base
{
    worker_thread_base::worker_thread_base() : 
            throttle{30},
            running{false},
            stopped{false},
            worker{},
            joiner{}
    {
    }
    
    worker_thread_base::~worker_thread_base()
    {
    }
    
    /**
     * @brief Halts the thread.
     * @param t How long (in milliseconds) to wait for the thread to die.
     * 0 is forever.  If the thread doesn't die within the alloted time,
     * it throws a runtime_error.
     */
    void worker_thread_base::halt(const unsigned int& t)
    {
        using std::this_thread::sleep_for;
        using std::chrono::milliseconds;
        
        if(!this->running) throw std::runtime_error{"Halt called on halted thread!"};
        
        this->stopped = true;
        if(t == 0)
        {
            while(this->running);
        }
        else
        {
            if(t >= 1000)
            {
                //if the time is greater than 1 second, check 30 times per second:
                for(unsigned int x{0}; ((x < (30 * (t / 1000))) && this->running); ++x)
                {
                    sleep_for(milliseconds(1000 / 30));
                }
            }
            else
            {
                //otherwise just check ten times within the alloted time:
                for(unsigned int x{0}; ((x < 10) && this->running); ++x)
                {
                    sleep_for(milliseconds(t / 10));
                }
            }
        }
        
        /* At this point, the thread has to have been stopped.
         * If it isn't, there's somthing terribly terribly wrong... */
        if(!this->running)
        {
            this->joiner->join();
        }
        else throw std::runtime_error{"worker_thread_base::halt(): Unable to halt thread!"};
    }
    
    void worker_thread_base::start()
    {
        this->worker = std::make_shared<std::thread>(worker_thread, this);
        this->joiner = std::make_shared<std::thread>(join_thread, this->worker.get());
    }
    
    
}

//thread functions:
namespace base
{
    void worker_thread(worker_thread_base* t)
    {
        using std::this_thread::sleep_for;
        using std::chrono::milliseconds;
        
        t->running = true;
        t->stopped = false;
        
        while(!t->stopped)
        {
            t->do_work();
            sleep_for(milliseconds(1000 / t->throttle));
        }
        t->running = false;
    }
    
    void join_thread(std::thread* t)
    {
        t->join();
    }
    
    
}

