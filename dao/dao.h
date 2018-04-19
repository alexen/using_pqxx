//
// dao.h
//
// Created on: Apr 18, 2018
//     Author: alexen
//

#pragma once

#include <boost/optional/optional.hpp>

#include <pqxx/transaction>
#include <pqxx/result>

#include <dao/types.h>


namespace dao {
namespace table_1 {


struct TestRow
{
     std::uint32_t id;
     boost::optional< std::string > comment;
     boost::optional< Status > status;
};


TestRowUptr fromRow( const pqxx::row& );
TestRowUptrVector fromResul( const pqxx::result& );

void createTable( pqxx::work& tr, bool dropBefore = true );


bool insert(
     pqxx::work& tr
     , const std::string& comment
     , const boost::optional< Status >& status = boost::none
     , const std::vector< char >& binary = {}
);


TestRowUptrVector selectAll( pqxx::dbtransaction& tr );


} // namespace table_1
} // namespace dao
