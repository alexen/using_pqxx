//
// main.cpp
//
// Created on: Mar 3, 2018
//     Author: alexen
//

#include <stdexcept>
#include <iostream>

#include <boost/optional/optional.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <pqxx/connection>
#include <pqxx/transaction>
#include <pqxx/result>

#include <dao/types.h>
#include <dao/dao.h>
#include <dao/io.h>
#include <dao/tools.h>
#include <tools/convert.h>
#include <tools/random.h>
#include <tools/io.h>


int main( int argc, char** argv )
{
     try
     {
          const auto ints = { 1, 2, 3, 4, 5 };
          const auto strs = {
               "my",
               "Bonny's",
               "is",
               "over",
               "the",
               "o'cean"
          };

          pqxx::connection conn{ "postgresql://dbuser@localhost:6432/testdb" };

          std::cout
               << "ints: " << pqxx::separated_list( ", ", ints.begin(), ints.end() ) << '\n'
               << "strs: " << pqxx::separated_list( ", ", strs.begin(), strs.end() ) << '\n'
               << "ints: " <<
               dao::tools::separatedList( ints.begin(), ints.end(),
                    []( int n )
                    {
                         return std::to_string( n );
                    }
                    ) << '\n'
               << "strs: " <<
               dao::tools::separatedList( strs.begin(), strs.end(),
                    [ &conn ]( const char* str )
                    {
                         return conn.esc( str );
                    }
                    ) << '\n'
               ;

          std::cout << "*ints: ";
          dao::tools::separatedList( std::cout, ints.begin(), ints.end(),
               []( auto& val )
               {
                    return val;
               }
          );
          std::cout << '\n';
          std::cout << "*strs: ";
          dao::tools::separatedList( std::cout, strs.begin(), strs.end(),
               [ &conn ]( const auto& s )
               {
                    return conn.quote( s );
               }
               );
          std::cout << '\n';

          pqxx::work tr{ conn };

          dao::table_1::createTable( tr, true );

          dao::table_1::insert( tr, "Initial status expected", dao::Status::Initial );
          dao::table_1::insert( tr, "This is intermediate", dao::Status::Intermediate );
          dao::table_1::insert( tr, "Something is finished!", dao::Status::Finished );
          dao::table_1::insert( tr, "Oops! Unknown status!" );

          dao::table_1::insert( tr, tr.esc( "Peter O'Tool Escaped" ) );
          dao::table_1::insert( tr, tr.quote( "Peter O'Tool Quoted" ) );
          dao::table_1::insert( tr, tr.quote( 1000 ) );
          dao::table_1::insert( tr, tr.quote( 2000 ) );

          const auto& bin = tools::random::binarySequence( 32 );

          std::cout << "binary: " << tools::convert::toHex( bin ) << '\n';

          dao::table_1::insert(
               tr,
               "Random sequence of bytes",
               dao::Status::Initial,
               bin
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
