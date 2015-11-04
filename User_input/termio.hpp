#ifndef TERMIO_HPP_INCLUDED
#define TERMIO_HPP_INCLUDED
#include <iostream>

namespace termio
{
    template<typename type> bool input(std::istream&, type&);
}

#endif
