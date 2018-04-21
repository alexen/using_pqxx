cmake_minimum_required(VERSION 3.5)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include_directories("${CMAKE_SOURCE_DIR}")

add_definitions(-std=c++17)
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0 -fno-inline")

find_package(Pqxx REQUIRED)
include_directories(${Pqxx_INCLUDE_DIRS})

add_subdirectory(dao)

set(EXECNAME using_pqxx)
add_executable(${EXECNAME}
     main.cpp
)
target_link_libraries(${EXECNAME}
     dao
)