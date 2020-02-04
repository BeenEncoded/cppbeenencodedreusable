#ifndef TIME_CLASS_HPP_INCLUDED
#define TIME_CLASS_HPP_INCLUDED
#include <ctime>
#include <iostream>
#include <chrono>

/* Nonzero if YEAR is a leap year (every 4 years,
   except every 100th isn't, and every 400th is).  */
# define __isleap(year)	\
  ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))

namespace tdata
{
    //time constants: 
    namespace t_const
    {
        constexpr long minute{60};
        constexpr long hour{minute * 60};
        constexpr long day{hour * 24};
        constexpr long week{day * 7};
    }
    
    namespace util
    {
        struct fixed_size_tm;
        
        
        struct fixed_size_tm to_fixed_tm(const struct tm&);
        struct tm from_fixed_tm(const struct fixed_size_tm&);
        
        struct fixed_size_tm
        {
            int32_t sec, min, hour, mday, mon, year, wday, yday, isdst;
        };
    }
    
    typedef class time_class time_class;
    
    typedef std::chrono::system_clock clock_type;
    typedef std::chrono::time_point<clock_type> time_point;
    
    struct tm current_time();
    const time_point& tmtotp( struct tm&, time_point&);
    const struct tm& tptotm(time_point&, struct tm&);
    int days_in_month(const int&, const int&);
    
    std::istream& operator>>(std::istream&, time_class&);
    std::ostream& operator<<(std::ostream&, const time_class&);
    
    /** Time: The indefinite continued progress of existence and events in the past, 
     * present, and future regarded as a whole. */
    typedef class time_class
    {
    public:
        time_class(const time_class&) noexcept;
        time_class(time_class&&) noexcept;
        explicit time_class(const struct tm&);
        explicit time_class();
        ~time_class();
        
        const struct tm& value() const;
        
        time_class& operator=(const time_class&);
        time_class& operator=(time_class&&) noexcept;
        time_class& operator=(const struct tm&);
        
        //comparisons
        bool operator==(const time_class&) const;
        bool operator!=(const time_class&) const;
        bool operator<(const time_class&) const;
        bool operator>(const time_class&) const;
        bool operator<=(const time_class&) const;
        bool operator>=(const time_class&) const;
        
        //mathematical ops: 
        const time_class& operator++();
        time_class        operator+(int) const;
        const time_class& operator+=(int);
        const time_class& operator--();
        time_class        operator-(int) const;
        const time_class& operator-=(int);
        
        /* data retrieval.  I hate to do this, but the information
         * must be modified before reads and after writes. */
        
        int& second();//0-59
        const int& second() const;
        int& mhour(); //0-23
        const int& mhour() const;
        int& minute();//0-59
        const int& minute() const;
        int& month(); //0-11 
        const int& month() const;
        int& wday();  //0-6
        const int& wday() const;
        int& mday(); //1-31
        const int& mday() const;
        int& yday();
        const int& yday() const;
        int hour() const;
        bool am() const;
        int gyear() const;
        void syear(const int&);
        void smonth(const int&);
        std::string month_name() const;
        std::string wday_name() const;
        
        friend std::istream& operator>>(std::istream&, time_class&);
        friend std::ostream& operator<<(std::ostream&, const time_class&);
        
    private:
        struct tm cur_time;
        
        void sync_to_yday();
        void subtract_day();
        void add_day();
        
    } time_class;
    
}

#endif
