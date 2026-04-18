cmake_minimum_required(VERSION 4.1)
set(CMAKE_CXX_STANDARD 23)
project(wdb)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_options(-Wall -Wextra -Werror)
    add_compile_definitions(DEBUG)
endif ()

# Dependencies
find_package(DPP REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(SQLiteCpp REQUIRED)

# Libraries
add_subdirectory(commands)
#add_subdirectory(logger)
add_subdirectory(db)

# Sources
add_subdirectory(src ${CMAKE_BINARY_DIR}/bin)

configure_file("${CMAKE_SOURCE_DIR}/db/sql/sql_queries.json" "${CMAKE_BINARY_DIR}/bin/sql/sql_queries.json")


