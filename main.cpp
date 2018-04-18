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

#include <boost/lexical_cast.hpp>
#include <boost/optional/optional.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <pqxx/connection>
#include <pqxx/transaction>
#include <pqxx/result>

#include <pqxx_ext/type_map.h>


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


void createTable( pqxx::work& tr )
{
     static const auto query =
          "create table if not exists test("
          " id bigserial primary key,"
          " data text,"
          " status text,"
          " binary_data bytea"
          ")";

     tr.exec( query );
}


enum class Status {
     Initial,
     Intermediate,
     Finished
};


std::ostream& operator<<( std::ostream& ostr, Status s )
{
     switch( s )
     {
          case Status::Initial:
               ostr << "Initial";
               break;
          case Status::Intermediate:
               ostr << "Intermediate";
               break;
          case Status::Finished:
               ostr << "Finished";
               break;
          default:
               ostr << "???";
     }
     return ostr;
}


namespace boost {


template<>
std::string lexical_cast< std::string, Status >( const Status& s )
{
     switch( s )
     {
          case Status::Initial:      return "INITIAL";
          case Status::Intermediate: return "INTERMEDIATE";
          case Status::Finished:     return "FINISHED";
          default:                   ;
     }
     BOOST_THROW_EXCEPTION( boost::bad_lexical_cast{} );
}


template<>
Status lexical_cast< Status, std::string >( const std::string& s )
{
     if( s == "INITIAL" )
     {
          return Status::Initial;
     }
     else if( s == "INTERMEDIATE" )
     {
          return Status::Intermediate;
     }
     else if( s == "FINISHED" )
     {
          return Status::Finished;
     }
     BOOST_THROW_EXCEPTION( boost::bad_lexical_cast{} );
}


} // namespace boost


namespace pqxx {


PQXX_STRING_TRAITS_SPEC( Status );


} // namespace pqxx


std::uint64_t insertData(
     pqxx::work& tr
     , const boost::optional< std::string >& data
     , const boost::optional< Status >& status
     , const std::vector< std::uint8_t >& binary
     )
{
     static constexpr auto query =
          "insert into test (data, status, binary_data)"
          " values ($1, $2, $3)"
          " returning id";

     return tr.parameterized( query )
          ( data )
          ( status )
          ( tr.esc_raw( binary.data(), binary.size() ), !binary.empty() )
          .exec()
          .front() // ссылка на строку (первая)
          .front() // ссылка на столбец строки (первый)
          .as< std::uint64_t >(); // значение столбца
}


struct TestRow {
     std::uint64_t id;
     boost::optional< std::string > data;
     boost::optional< Status > status;
};


std::ostream& operator<<( std::ostream& ostr, const TestRow& row )
{
     ostr << row.id << ", " << row.status << ", " << row.data;
     return ostr;
}

using TestRowVector = std::vector< TestRow >;


TestRow fromRow( const pqxx::row& row )
{
     TestRow out;
     row.at( "id" ).to( out.id );
     row.at( "data" ).to( out.data );
     row.at( "status" ).to( out.status );
     return out;
}


TestRowVector fromResul( const pqxx::result& result )
{
     TestRowVector out;
     if( !result.empty() )
     {
          out.reserve( result.size() );
          std::transform( result.cbegin(), result.cend(), std::back_inserter( out ), fromRow );
     }
     return out;
}


TestRowVector loadAllData( pqxx::dbtransaction& tr, const std::string& tableName )
{
     return fromResul( tr.exec( "select id, status, data from " + tr.quote_name( tableName ) ) );
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
     const auto randomBin = generateRandomBinary( 32 );

     try
     {
          pqxx::connection conn{ "postgresql://dbuser@localhost:6432/testdb" };
          pqxx::work tr{ conn };

          createTable( tr );

          insertData( tr, std::string{ "first string" }, Status::Initial, {} );
          insertData( tr, std::string{ "second string" }, Status::Intermediate, {} );
          insertData( tr, std::string{ "third string" }, Status::Finished, {} );
          insertData( tr, boost::none, Status::Initial, {} );
          insertData( tr, std::string{ "fourth string" }, boost::none, {} );

          insertData( tr, tr.esc( "Peter O'Tool Escaped" ), boost::none, {} );
          insertData( tr, tr.quote( "Peter O'Tool Quoted" ), boost::none, {} );
          insertData( tr, tr.quote( 1000 ), boost::none, {} );
          insertData( tr, tr.quote( 2000 ), boost::none, {} );

          insertData( tr, std::string{ "Random sequence of bytes" }, Status::Intermediate, randomBin );

          std::cout << "Loaded data:\n";
          for( auto&& each: loadAllData( tr, "test" ) )
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
