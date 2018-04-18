//
// types.h
//
// Created on: Apr 18, 2018
//     Author: alexen
//

#pragma once

#include <memory>
#include <vector>


namespace dao {


enum class Status {
     Initial,
     Intermediate,
     Finished
};


namespace table_1 {

struct TestRow;
using TestRowUptr = std::unique_ptr< TestRow >;
using TestRowUptrVector = std::vector< TestRowUptr >;

} // namespace table_1
} // namespace dao
