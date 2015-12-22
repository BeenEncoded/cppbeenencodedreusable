/**
 * This header provides template functions that convert
 * a number of one base, to it's counterpart in another base.  For example, 
 * 5 in base ten, to 101 in base 2, or vice versa.  The functions are defined
 * here -- in this header file -- to provide implicit instantiation of 
 * corresponding definitions.  */

#ifndef BITMAP_BIMAP_HPP_INCLUDED
#define BITMAP_BIMAP_HPP_INCLUDED
#include <string>
#include <vector>
#include <bitset>
#include <utility>
#include <iostream>
#include <cmath>

#include "global/global_defines.hpp"

namespace bmap
{
    namespace
    {
        template<unsigned int base> std::vector<int> to_base(const unsigned long long int&);
        template<unsigned int base> unsigned long long int from_base(const std::vector<int>&);
        template<unsigned int base1, unsigned int base2> std::vector<int> convert_base(const std::vector<int>&);
        template<unsigned int base1, unsigned int base2> std::vector<int> convert_base(const std::string&);
        
        /**
         * @brief Converts a number to the specified base.
         * @param num the number to convert.
         * @return A vector that representes the new number.  Each element of
         * the vector is a digit.
         */
        template<unsigned int base>
        std::vector<int> to_base(const unsigned long long int& num)
        {
            if(num == 0) return std::vector<int>{0};
            
            unsigned long long int tempnum(num);
            std::vector<int> new_num;
            
            while(tempnum > 0)
            {
                new_num.insert(new_num.begin(), (tempnum % base));
                tempnum /= base;
            }
            return new_num;
        }
        
        /**
         * @brief Converts a 'number' of a specified base to base ten.
         * @param num the number to convert
         * @return The number... converted to base 10.
         */
        template<unsigned int base>
        unsigned long long int from_base(const std::vector<int>& num)
        {
            //if the number is zero, don't bother with conversion.
            if(!num.empty())
            {
                if(num.size() == 1)
                {
                    if(num.front() == 0) return 0;
                }
            }
            
            unsigned long long int new_num(0);
            for(unsigned long x{num.size() - 1}; true; --x)
            {
                new_num += (num[x] * std::pow(base, (num.size() - (1 + x))));
                if(x == 0) break;
            }
            return new_num;
        }
        
        /**
         * @brief A function to make it easier to make base conversions that
         * don't include base 10.  This is really just a convenience function.
         * 
         * usage: convert_base<from_base, to_base>(number)
         * @param tempnum the number.
         * @return The number in the new base.
         */
        template<unsigned int base1, unsigned int base2>
        std::vector<int> convert_base(const std::vector<int>& tempnum)
        {
            return to_base<base2>(from_base<base1>(tempnum));
        }
        
        /**
         * @brief A function to make it easier to make base conversions that
         * don't include base 10.  This is really just a convenience function.
         * 
         * Usage:
         * convert_base<from_base, to_base>(number)
         * @param s the string.
         * @return The number in the new base.
         */
        template<unsigned int base1, unsigned int base2>
        std::vector<int> convert_base(const std::string& s)
        {
            std::vector<int> tempv;
            for(unsigned long x{0}; x < s.size(); ++x) tempv.push_back((unsigned char)s[x]);
            return convert_base<base1, base2>(tempv);
        }
        
        
    }
    
    
}

#endif