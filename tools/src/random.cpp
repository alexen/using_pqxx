//
// random.cpp
//
// Created on: Apr 21, 2018
//     Author: alexen
//

#include <tools/random.h>

#include <random>
#include <algorithm>
#include <functional>


namespace tools {
namespace random {


std::vector< char > binarySequence( std::uint32_t len )
{
     static std::default_random_engine dre{
          static_cast< std::default_random_engine::result_type >( clock() )
     };
     static std::uniform_int_distribution< char > distr{
          std::numeric_limits< char >::min(),
          std::numeric_limits< char >::max()
     };

     std::vector< char > result( len );
     std::generate( result.begin(), result.end(), std::bind( distr, std::ref( dre ) ) );
     return result;
}


} // namespace random
} // namespace tools
