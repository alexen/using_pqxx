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
#include <pqxx/strconv>


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
          " status text"
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


namespace pqxx {


template< typename T >
struct string_traits< boost::optional< T > >
{
     static constexpr const char* name() noexcept { return "optional value"; }
     static constexpr bool has_null() noexcept { return true; }
     static bool is_null( boost::optional< T > val ) noexcept { return !val; }
     static Status null() { return boost::none; }
     static void from_string( const char* str, boost::optional< T >& out )
     {
          T val;
          string_traits< T >::from_string( str, val );
          out = val;
     }
     static std::string to_string( const boost::optional< T >& opt )
     {
          return string_traits< T >::to_string( *opt );
     }
};


template<>
struct string_traits< Status >
{
     static constexpr const char* name() noexcept { return "Status"; }
     static constexpr bool has_null() noexcept { return false; }
     static bool is_null( Status s ) noexcept { return false; }
     static Status null() { internal::throw_null_conversion(name()); }
     static void from_string( const char* str, Status& out )
     {
          if( std::strcmp( str, "INITIAL" ) == 0 )
          {
               out = Status::Initial;
          }
          else if( std::strcmp( str, "INTERMEDIATE" ) == 0 )
          {
               out = Status::Intermediate;
          }
          else if( std::strcmp( str, "FINISHED" ) == 0 )
          {
               out = Status::Finished;
          }
          else
          {
               BOOST_THROW_EXCEPTION( std::runtime_error{ "unexpected type" } );
          }
     }
     static std::string to_string( Status s )
     {
          switch( s )
          {
               case Status::Initial:
                    return "INITIAL";
               case Status::Intermediate:
                    return "INTERMEDIATE";
               case Status::Finished:
                    return "FINISHED";
               default:
                    ;
          }
          BOOST_THROW_EXCEPTION( std::runtime_error{ "unexpected type" } );
          return "";
     }
};


} // namespace pqxx


std::uint64_t insertData(
     pqxx::work& tr
     , const boost::optional< std::string >& data
     , const boost::optional< Status >& status
     )
{
     static constexpr auto query =
          "insert into test (data, status)"
          " values ($1, $2)"
          " returning id";

     return tr.parameterized( query )
          ( data )
          ( status )
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


TestRowVector loadAllData( pqxx::dbtransaction& tr )
{
     return fromResul( tr.exec( "select id, status, data from test" ) );
}


int main( int argc, char** argv )
{
     try
     {
          pqxx::connection conn{ "postgresql://alexen:ALEXEN@localhost:6432/test" };
          pqxx::work tr{ conn };

          createTable( tr );

          insertData( tr, std::string{ "first string" }, Status::Initial );
          insertData( tr, std::string{ "second string" }, Status::Intermediate );
          insertData( tr, std::string{ "third string" }, Status::Finished );
          insertData( tr, boost::none, Status::Initial );
          insertData( tr, std::string{ "fourth string" }, boost::none );

          insertData( tr, tr.esc( "Peter O'Tool Escaped" ), boost::none );
          insertData( tr, tr.quote( "Peter O'Tool Quoted" ), boost::none );
          insertData( tr, tr.quote( 1000 ), boost::none );
          insertData( tr, tr.quote( 2000 ), boost::none );

          std::cout << "Loaded data:\n";
          for( auto&& each: loadAllData( tr ) )
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
