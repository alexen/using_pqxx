//
// io.h
//
// Created on: Apr 21, 2018
//     Author: alexen
//

#pragma once

#include <ostream>


namespace dao {


enum class Status;


namespace table_1 {


struct TestRow;


} // namespace table_1
} // namespace dao


std::ostream& operator<<( std::ostream&, const dao::Status& );

std::ostream& operator<<( std::ostream&, const dao::table_1::TestRow& );
