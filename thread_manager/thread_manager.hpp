#ifndef THREAD_MANAGER_HPP_INCLUDED
#define THREAD_MANAGER_HPP_INCLUDED
#include <thread>
#include <queue>
#include <memory>
#include <utility>

namespace utility
{
    typedef class thread_manager thread_manager;
    
    void manage_joined(utility::thread_manager*, const std::shared_ptr<bool>&, 
            const std::shared_ptr<bool>&);
    
    /**
     * @class thread_manager
     * @author Jonathan Whitlock
     * @date 11/18/2015
     * @file thread_manager.hpp
     * @brief A basic thread manager that assures all threads are joined on destruction.
     * Threads are joined automatically, and removed from the list of stored threads once
     * they are.  On destruction this object will wait for all threads to terminate.
     */
    typedef class thread_manager
    {
    private:
        thread_manager(const thread_manager&) = delete;
        thread_manager(thread_manager&&) = delete;
        
        thread_manager& operator=(const thread_manager&) = delete;
        thread_manager& operator=(thread_manager&&) = delete;
        
    public:
        explicit thread_manager();
        ~thread_manager();
        
        template<typename function_t, typename... args_t>
        void add_thread(function_t&& f, args_t&&... args)
        {
            this->threads.emplace(std::make_shared<std::thread>(f, args...));
        }
        
        unsigned int size() const;
        bool threads_running() const;
        bool join(const unsigned long&) const;
        
        friend void manage_joined(utility::thread_manager*, const std::shared_ptr<bool>&, 
                const std::shared_ptr<bool>&);
        
    private:
        std::queue<std::shared_ptr<std::thread> > threads;
        std::shared_ptr<bool> finished, jr_returning;
        std::thread joined_remover;
        
    } thread_manager;
    
}

#endif