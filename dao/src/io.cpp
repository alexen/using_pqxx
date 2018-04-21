//
// io.cpp
//
// Created on: Apr 21, 2018
//     Author: alexen
//

#include <dao/io.h>

#include <tools/io.h>
#include <dao/dao.h>


std::ostream& operator<<( std::ostream& ostr, const dao::Status& s )
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
