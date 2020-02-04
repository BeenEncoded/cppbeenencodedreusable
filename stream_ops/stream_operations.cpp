#include <iostream>
#include <string>


#include "stream_operations.hpp"

namespace
{
}

namespace utility
{
    std::ostream& write_string(std::ostream& out, const std::string& s)
    {
        if(out.good())
        {
            {
                std::uint32_t sz((std::uint32_t)s.size());
                out_mem<std::uint32_t>(out, sz);
            }
            out<< s;
        }
        return out;
    }
    
    std::istream& read_string(std::istream& in, std::string& s)
    {
        s.erase();
        in.peek();
        if(in.good())
        {
            std::uint32_t size{0};
            in_mem<std::uint32_t>(in, size);
            for(std::uint32_t x{0}; ((x < size) && in.good()); ++x)
            {
                s += in.get();
                if(in.fail()) s.erase((s.begin() + (s.size() - 1)));
            }
        }
        return in;
    }
    
    /**
     * @brief Peeks 'count' characters.  Can be used to check multi-char delimiters.
     * The stream is garunteed to be left in the state it was found.
     * @param in stream to peek
     * @param count number of characters to peek.
     * @return A string of the next 'count' characters.  Not garunteed to
     * be of size 'count'.  If the end of the stream is reached before 'count'
     * characters can be read, the operation is aborted, and the string is returned.
     */
    std::string peek_string(std::istream& in, const unsigned int& count)
    {
        std::string temps;
        
        in.peek();
        if(in.good())
        {
            std::istream::pos_type pos(in.tellg());
            for(unsigned int x(0); ((x < count) && (in.peek() != EOF) && in.good()); ++x) temps += in.get();
            if(!in.good()) in.clear();
            in.seekg(pos);
        }
        return temps;
    }
    
    
}
