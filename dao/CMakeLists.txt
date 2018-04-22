set(LIBNAME dao)
add_library(${LIBNAME}
     src/dao.cpp
     src/io.cpp
)
target_link_libraries(${LIBNAME}
     ${Pqxx_LIBRARIES}
)

set(TESTNAME ut-${LIBNAME})
add_executable(${TESTNAME} test/tools_test.cpp)
target_link_libraries(${TESTNAME} gtest_main)
add_test(NAME ${LIBNAME} COMMAND ${TESTNAME})
