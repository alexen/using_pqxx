//
// main.cpp
//
// Created on: Mar 3, 2018
//     Author: alexen
//

#include <stdexcept>
#include <iostream>
#include <random>
#include <limits>

#include <boost/optional/optional.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <pqxx/connection>
#include <pqxx/transaction>
#include <pqxx/result>

#include <dao/types.h>
#include <dao/dao.h>


template< typename T >
std::ostream& operator<<( std::ostream& ostr, const boost::optional< T >& opt )
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
std::ostream& operator<<( std::ostream& ostr, const std::unique_ptr< T >& uptr )
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


std::ostream& operator<<( std::ostream& ostr, const dao::Status s )
{
     switch( s )
     {
          case dao::Status::Initial:
               ostr << "Initial";
               break;
          case dao::Status::Intermediate:
               ostr << "Intermediate";
               break;
          case dao::Status::Finished:
               ostr << "Finished";
               break;
          default:
               ostr << "???";
     }
     return ostr;
}


std::ostream& operator<<( std::ostream& ostr, const dao::table_1::TestRow& row )
{
     ostr << "id " << row.id << ": " << row.comment << ", status " << row.status;
     return ostr;
}


std::vector< std::uint8_t > generateRandomBinary( std::uint32_t len )
{
     static std::default_random_engine dre{
          static_cast< std::default_random_engine::result_type >( clock() )
     };
     static std::uniform_int_distribution< std::uint8_t > distr{
          std::numeric_limits< std::uint8_t >::min(),
          std::numeric_limits< std::uint8_t >::max()
     };

     std::vector< std::uint8_t > result( len );
     std::generate( result.begin(), result.end(), std::bind( distr, std::ref( dre ) ) );
     return result;
}


int main( int argc, char** argv )
{
     try
     {
          pqxx::connection conn{ "postgresql://dbuser@localhost:6432/testdb" };
          pqxx::work tr{ conn };

          dao::table_1::createTable( tr, true );

          dao::table_1::insert( tr, "Initial status expected", dao::Status::Initial );
          dao::table_1::insert( tr, "This is intermediate", dao::Status::Intermediate );
          dao::table_1::insert( tr, "Something is finished!", dao::Status::Finished, {} );
          dao::table_1::insert( tr, "Oops! Unknown status!", boost::none, {} );

          dao::table_1::insert( tr, tr.esc( "Peter O'Tool Escaped" ) );
          dao::table_1::insert( tr, tr.quote( "Peter O'Tool Quoted" ) );
          dao::table_1::insert( tr, tr.quote( 1000 ) );
          dao::table_1::insert( tr, tr.quote( 2000 ) );

          dao::table_1::insert(
               tr,
               "Random sequence of bytes",
               dao::Status::Initial,
               generateRandomBinary( 32 )
               );

          std::cout << "Loaded data:\n";
          for( auto&& each: dao::table_1::selectAll( tr ) )
          {
               std::cout << each << "\n";
          }

          tr.commit();
     }
     catch( const std::exception& e )
     {
          std::cerr << "exception: " << boost::diagnostic_information( e ) << '\n';
          return 1;
     }
     return 0;
}
