#ifndef UTILITY_STREAM_OPERATIONS_HPP_INCLUDED
#define UTILITY_STREAM_OPERATIONS_HPP_INCLUDED
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <cstdint>
#include <cstring>

namespace utility
{
    std::string peek_string(std::istream&, const unsigned int&);
    std::ostream& write_string(std::ostream&, const std::string&);
    std::istream& read_string(std::istream&, std::string&);
    
    namespace
    {
        template<class type> bool safe_getline(std::istream&, type&, const char&);
        template<> bool safe_getline<std::string>(std::istream&, std::string&, const char&);
        template<typename type> std::istream& in_mem(std::istream&, type&);
        template<typename type> std::ostream& out_mem(std::ostream&, const type&);
        template<typename type> std::ostream& write_vector(std::ostream&, const std::vector<type>&);
        template<typename type> std::istream& read_vector(std::istream&, std::vector<type>&);
        
        
        
        /**
         * @brief Safely retrieves a line from a file given a specified delimiter.  
         * In case of failure, it will leave the stream in the state it was in
         * before the function call.  Returns true/false based on success.
         * @param in std::istream to read from
         * @param t object to read into
         * @param delimiter delimiter to read to
         * @return bool true if the read was a success, false otherwise.
         */
        template<class type>
        bool safe_getline(std::istream& in, type& t, const char& delimiter)
        {
            std::string temps;
            bool success(false);
            std::istream::pos_type previous_position(in.tellg());
            
            t = type();
            
            if(in.good())
            {
                std::getline(in, temps, delimiter);
                if(in.good())
                {
                    std::stringstream ss;
                    ss<< temps;
                    ss>> t;
                    success = true;
                }
                else if(!in.eof() && in.fail())
                {
                    /* So, here we are: we didn't reach the end of the file, 
                     but somwhere there was a big mistake... the program will
                     now attempt to salvage the situation: */
                    in.seekg(previous_position);
                    success = false;
                }
            }
            return success;
        }
        
        /**
         * @brief A specialization of safe_getline for strings.
         * @param in std::istream to read from
         * @param s the string to read into
         * @param delimiter the delimiter to read up to.
         * @return a bool: true if the read was a success, false otherwise.
         */
        template<>
        bool safe_getline<std::string>(std::istream& in, std::string& s, const char& delimiter)
        {
            bool success(false);
            std::istream::pos_type previous_position(in.tellg());
            
            s.erase();
            
            if(in.good())
            {
                std::getline(in, s, delimiter);
                if(in.good()) success = true;
                else if(!in.eof() && in.fail())
                {
                    /* So, here we are: we didn't reach the end of the file, 
                     but somwhere there was a big mistake... the program will
                     now attempt to salvage the situation: */
                    in.seekg(0, std::ios::beg);
                    in.seekg(previous_position);
                    success = false;
                }
            }
            return success;
        }
        
        /**
         * @brief reads memory of a type from a file.  Fixed-width types are
         * strongly suggested for programs that need to be cross-compatible.
         */
        template<typename type>
        std::istream& in_mem(std::istream& in, type& t)
        {
            t = type();
            char *mem(new char[sizeof(type)]);
            
            in.peek();
            if(in.good())
            {
                unsigned int x(0);
                for(; ((x < sizeof(type)) && in.good()); ++x)
                {
                    mem[x] = in.get();
                }
                if(((x + 1) < sizeof(type)) && !in.fail())
                {
                    in.setstate(std::ios_base::failbit);
                }
                memcpy(&t, mem, sizeof(type));
            }
            delete[] mem;
            return in;
        }
        
        /**
         * @brief Writes memory of a type to a file.  Fixed-width types are
         * strongly suggested for programs that need to be cross-compatible.
         */
        template<typename type>
        std::ostream& out_mem(std::ostream& out, const type& t)
        {
            char *mem(new char[sizeof(type)]);
            
            memcpy(mem, &t, sizeof(type));
            if(out.good())
            {
                unsigned int x(0);
                for(; ((x < sizeof(type)) && out.good()); ++x)
                {
                    out<< mem[x];
                }
                if(((x + 1) < sizeof(type)) && !out.fail()) out.setstate(std::ios_base::failbit);
            }
            delete[] mem;
            return out;
        }
        
        /**
         * @brief Writes a vector of type 'type' to a stream.  Garunteed not to
         * cause delimiter collisions.  Intended to use in operators.
         * @param out Stream to write to.
         * @param v The vector to write.
         * @return The stream.
         */
        template<typename type>
        std::ostream& write_vector(std::ostream& out, const std::vector<type>& v)
        {
            if(out.good())
            {
                {
                    std::uint32_t sz((std::uint32_t)v.size());
                    out_mem<std::uint32_t>(out, sz);
                }
                for(unsigned int x(0); ((x < v.size()) && out.good()); ++x)
                {
                    out<< v[x];
                }
            }
            return out;
        }
        
        /**
         * @brief Reads a vector of type 'type' from a stream.  Garunteed not to
         * cause delimiter collisions. Intended to use in operators.
         * @param in The stream to read from.
         * @param v The vector to store the data in.
         * @return The stream.
         */
        template<typename type>
        std::istream& read_vector(std::istream& in, std::vector<type>& v)
        {
            v.erase(v.begin(), v.end());
            in.peek();
            if(in.good())
            {
                std::uint32_t size(0);
                
                in_mem<std::uint32_t>(in, size);
                for(std::uint32_t x(0); ((x < size) && in.good() && (in.peek() != EOF)); ++x)
                {
                    v.push_back(type());
                    in>> v.back();
                    if(in.fail() && !in.eof()) v.pop_back();
                    in.peek();
                }
                in.peek();
            }
            return in;
        }
        
        
    }
}

#endif