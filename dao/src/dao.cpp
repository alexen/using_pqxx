//
// data.cpp
//
// Created on: Apr 18, 2018
//     Author: alexen
//

#include <dao/dao.h>

#include <boost/lexical_cast.hpp>

#include <pqxx_ext/type_map.h>


namespace boost {


template<>
std::string lexical_cast< std::string, dao::Status >( const dao::Status& s )
{
     switch( s )
     {
          case dao::Status::Initial:      return "INITIAL";
          case dao::Status::Intermediate: return "INTERMEDIATE";
          case dao::Status::Finished:     return "FINISHED";
          default:                        ;
     }
     BOOST_THROW_EXCEPTION( boost::bad_lexical_cast{} );
}


template<>
dao::Status lexical_cast< dao::Status, std::string >( const std::string& s )
{
     if( s == "INITIAL" )
     {
          return dao::Status::Initial;
     }
     else if( s == "INTERMEDIATE" )
     {
          return dao::Status::Intermediate;
     }
     else if( s == "FINISHED" )
     {
          return dao::Status::Finished;
     }
     BOOST_THROW_EXCEPTION( boost::bad_lexical_cast{} );
}


} // namespace boost


namespace pqxx {


PQXX_STRING_TRAITS_SPEC( dao::Status );


} // namespace pqxx


namespace dao {
namespace table_1 {


TestRowUptr fromRow( const pqxx::row& row )
{
     TestRowUptr out{ new TestRow };
     row.at( "id" ).to( out->id );
     row.at( "comment" ).to( out->comment );
     row.at( "status" ).to( out->status );
     return out;
}


TestRowUptrVector fromResul( const pqxx::result& result )
{
     TestRowUptrVector out;
     if( !result.empty() )
     {
          out.reserve( result.size() );
          std::transform( result.cbegin(), result.cend(), std::back_inserter( out ), fromRow );
     }
     return out;
}


void createTable( pqxx::work& tr, bool dropBefore )
{
     static constexpr auto dropQuery = "DROP TABLE IF EXISTS test_table_1";
     static constexpr auto createQuery =
          "CREATE TABLE IF NOT EXISTS test_table_1 ("
          " id SERIAL PRIMARY KEY,"
          " comment TEXT NOT NULL,"
          " status TEXT,"
          " data BYTEA"
          ")";

     if( dropBefore )
     {
          tr.exec( dropQuery );
     }

     tr.exec( createQuery );
}


bool insert(
     pqxx::work& tr
     , const std::string& comment
     , const boost::optional< Status >& status
     , const std::vector< char >& binary
     )
{
     static constexpr auto query =
          "INSERT INTO test_table_1 (comment, status, data)"
          " VALUES ($1, $2, $3)";

     return tr.parameterized( query )
          ( comment )
          ( status )
          ( tr.esc_raw( reinterpret_cast< const std::uint8_t* >( binary.data() ), binary.size() ), !binary.empty() )
          .exec()
          .affected_rows() > 0;
}


TestRowUptrVector selectAll( pqxx::dbtransaction& tr )
{
     static constexpr auto query = "SELECT id, comment, status, data FROM test_table_1";
     return fromResul( tr.exec( query ) );
}


} // namespace table_1
} // namespace dao
