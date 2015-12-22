#include <queue>
#include <memory>
#include <thread>
#include <chrono>
#include <iostream>
#include <utility>
#include <stdexcept>
#include <exception>

#include "thread_manager.hpp"

namespace utility
{
    thread_manager::thread_manager() : 
            threads{},
            finished{std::make_shared<bool>(false)}, 
            jr_returning{std::make_shared<bool>(false)}, 
            joined_remover{manage_joined, this, this->finished, this->jr_returning}
    {
    }
    
    /**
     * @brief Terminates all threads.  It is garunteed to let the joiner thread join
     * all of the remaining threads, and then join the joiner thread.  If 
     * any threads are being blocked from joining, the joiner will never join.
     * In that case, the destructor will wait only 2 seconds for it to finish up.
     * If that doesn't happen it's printed to stdout and the destructor returns.
     */
    thread_manager::~thread_manager()
    {
        using std::chrono::milliseconds;
        using std::this_thread::sleep_for;
        
        std::cout<< std::endl<< "... waiting for remaining threads to close..."<< std::endl;
        *(this->finished.get()) = true;
        
        //waits two seconds for the joiner to end.  This way, we don't lock the program if some thread
        //doesn't want to die.
        this->join(2000);
        
        //give the joiner some time to finish up (1 second max)
        for(unsigned int x{0}; ((x < 30) && !(*(this->jr_returning.get()))); ++x)
        {
            sleep_for(milliseconds(1000 / 30));
        }
        if(*(this->jr_returning.get()))
        {
            this->joined_remover.join();
            std::cout<< "Threads ended."<< std::endl;
        }
        else
        {
            std::cerr<< "Failed to terminate threads within 2 seconds!"<< std::endl;
        }
    }
    
    unsigned int thread_manager::size() const
    {
        return this->threads.size();
    }
    
    bool thread_manager::threads_running() const
    {
        return !this->threads.empty();
    }
    
    /**
     * @brief Waits for the specified number of milliseconds (approx) for a thread to join.
     * @return True if no threads were running afterwards.
     */
    bool thread_manager::join(const unsigned long& m) const
    {
        using std::this_thread::sleep_for;
        using std::chrono::milliseconds;
        
        unsigned long t{(m / (1000 / 30))};
        
        if(t == 0) ++t;
        for(unsigned int x{0}; ((x < t) && this->threads_running()); ++x)
        {
            sleep_for(milliseconds(1000 / 30));
        }
        return !this->threads_running();
    }
    
    
}

/* Misc functions: */
namespace utility
{
    /**
     * @brief Joins all the threads within the thread manager and removed joined threads.
     * When the "stop" bool is set to true, it will continue until all threads are joined.
     * When it returns, it will set the "return" bool to true.  When this function exits,
     * it is garunteed that all threads of the manager are joined.
     * 
     * Threads are joined in the order they were created.
     * @param s A pointer to a bool.  If the bool is true, then this thread will exit.
     */
    void manage_joined(utility::thread_manager* manager, const std::shared_ptr<bool>& s, 
            const std::shared_ptr<bool>& r)
    {
        using std::this_thread::sleep_for;
        using std::chrono::milliseconds;
        
        std::shared_ptr<bool> stop{s}, returning{r};
        
        *returning.get() = false;
        do
        {
            //slow down when there is nothing to do:
            while(!(*stop.get()) && !manager->threads_running()) sleep_for(milliseconds(1000 / 30));
            while(!manager->threads.empty())
            {
                if(manager->threads.front().get()->joinable()) manager->threads.front().get()->join();
                manager->threads.pop();
            }
        }while(!(*stop.get()) || manager->threads_running());
        if(manager->threads_running()) throw std::runtime_error{"Joiner terminating, but threads still running!"};
        *returning.get() = true;
    }
    
    
}