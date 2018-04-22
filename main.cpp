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


void test1( const std::string& connParams, bool commitChanges = true )
{
     pqxx::connection conn{ connParams };
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

     if( commitChanges )
     {
          tr.commit();
     }
}


int main( int argc, char** argv )
{
     try
     {
          const auto connParams = "postgresql://dbuser@localhost:6432/testdb";

          test1( connParams );
     }
     catch( const std::exception& e )
     {
          std::cerr << "exception: " << boost::diagnostic_information( e ) << '\n';
          return 1;
     }
     return 0;
}
