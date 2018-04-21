//
// io.h
//
// Created on: Apr 21, 2018
//     Author: alexen
//

#pragma once

#include <ostream>
#include <memory>

#include <boost/optional/optional.hpp>


template< typename T >
inline std::ostream& operator<<( std::ostream& ostr, const boost::optional< T >& opt )
{
     if( opt )
     {
          ostr << *opt;
     }
     else
     {
          ostr << "--";
     }
     return ostr;
}


template< typename T >
inline std::ostream& operator<<( std::ostream& ostr, const std::unique_ptr< T >& uptr )
{
     if( uptr )
     {
          ostr << *uptr;
     }
     else
     {
          ostr << "--";
     }
     return ostr;
}
