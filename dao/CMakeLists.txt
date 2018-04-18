set(NAME dao)
add_library(${NAME}
     src/dao.cpp
)
target_link_libraries(${NAME}
     ${Pqxx_LIBRARIES}
)