#include <iostream>
#include <exception>
#include <string>
#include <sstream>
#include <stdexcept>

#include "termio.hpp"

namespace
{
    template<typename type> bool convert(const std::string&, type&);
    bool str_is_number(const std::string&);
    
    
    //This should NEVER be used!
    template<typename type>
    bool convert(const std::string& s __attribute__((unused)), type& t __attribute__((unused)))
    {
        throw std::runtime_error("programmer didn't instantiate this conversion...");
        return false;
    }
    
    //Template specializations:
    
    template<>
    bool convert<std::string>(const std::string& s, std::string& to)
    {
        to = s;
        return true;
    }
    
    template<>
    __attribute__((unused)) bool convert<long double>(const std::string& s, long double& to)
    {
        if(str_is_number(s))
        {
            std::stringstream ss;
            ss<< s;
            ss>> to;
            return true;
        }
        return false;
    }
    
    template<>
    __attribute__((unused)) bool convert<unsigned int>(const std::string& s, unsigned int& num)
    {
        if(str_is_number(s) && (s.find('.') == std::string::npos) && 
                (s.find('-') == std::string::npos))
        {
            std::stringstream ss;
            ss<< s;
            ss>> num;
            return true;
        }
        return false;
    }
    
    template<>
    __attribute__((unused)) bool convert<int>(const std::string& s, int& num)
    {
        if(str_is_number(s) && (s.find('.') == std::string::npos))
        {
            std::stringstream ss;
            ss<< s;
            ss>> num;
            return true;
        }
        return false;
    }
    
    inline bool str_is_number(const std::string& s)
    {
        if(s.empty() || (s.find('.') != s.rfind('.')) || (s.find('-') != s.rfind('-')))
        {
            return false;
        }
        
        bool isnum(true);
        
        if((s.find('-') != std::string::npos) && (s[0] != '-')) return false;
        for(std::string::const_iterator it(s.begin()); ((it != s.end()) && isnum); ++it)
        {
            if(std::string("1234567890").find(*it) == std::string::npos)
            {
                if((*it != '.') && (*it != '-')) isnum = false;
            }
        }
        return isnum;
    }
    
    
}

namespace termio
{
    template<typename type>
    bool input(std::istream& in, type& t)
    {
        std::string temps;
        bool success(false);
        
        std::getline(in, temps, '\n');
        success = !in.fail();
        if(success) success = convert(temps, t);
        return success;
    }
    
    template bool input<std::string>(std::istream&, std::string&); //basically std::getline
}

