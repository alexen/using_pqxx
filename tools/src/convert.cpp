//
// tools.cpp
//
// Created on: Apr 21, 2018
//     Author: alexen
//

#include <tools/convert.h>


namespace tools {
namespace convert {


std::string toHex( const char* ptr, const std::size_t len )
{
     static constexpr auto hexLetter = "0123456789abcdef";
     std::string result;
     result.reserve( len * 2 );
     for( std::size_t i = 0; i < len; ++i )
     {
          result.push_back( hexLetter[ (ptr[ i ] & 0xf0) >> 4 ] );
          result.push_back( hexLetter[ ptr[ i ] & 0x0f ] );
     }
     return result;
}


std::string toHex( const std::string& str )
{
     return toHex( str.data(), str.size() );
}


std::string toHex( const std::vector< char >& vec )
{
     return toHex( vec.data(), vec.size() );
}


} // namespace convert
} // namespace tools
