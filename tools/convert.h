//
// tools.h
//
// Created on: Apr 21, 2018
//     Author: alexen
//

#pragma once

#include <cstddef>
#include <string>
#include <vector>


namespace tools {
namespace convert {


std::string toHex( const char* ptr, const std::size_t len );


std::string toHex( const std::string& str );


std::string toHex( const std::vector< char >& vec );


} // namespace convert
} // namespace tools
