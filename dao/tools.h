//
// tools.h
//
// Created on: Apr 22, 2018
//     Author: alexen
//

#pragma once

#include <string>
#include <sstream>


namespace dao {
namespace tools {


template< typename Iter, typename Quoter >
inline void separatedList(
     std::ostream& ostr
     , Iter begin
     , Iter end
     , Quoter&& quote
     , const std::string& sep = ", "
)
{
     if( begin != end )
     {
          ostr << quote( *begin );
          for( ++begin; begin != end; ++begin )
          {
               ostr << sep << quote( *begin );
          }
     }
}


template< typename Iter, typename Quoter >
inline std::string separatedList(
     Iter begin
     , Iter end
     , Quoter&& quote
     , const std::string& sep = ", "
)
{
     std::ostringstream ostr;
     separatedList( ostr, begin, end, quote, sep );
     return ostr.str();
}


} // namespace tools
} // namespace dao
