cmake_minimum_required(VERSION 4.1)
set(CMAKE_CXX_STANDARD 23)
project(warverse_discord_bot)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_options(-Wall -Wextra -Werror)
    add_compile_definitions(DEBUG)
endif ()

# Dependencies
find_package(DPP REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(SQLite3 REQUIRED)

# Libraries
add_subdirectory(commands)

# Sources
add_subdirectory(src ${CMAKE_BINARY_DIR}/bin)


