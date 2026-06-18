cmake_minimum_required(VERSION 4.1)
set(CMAKE_CXX_STANDARD 23)
project(wdb)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_options(-Wall -Wextra -Werror)
    add_compile_options(-Wno-format-nonliteral)
    add_compile_definitions(DEBUG)
endif ()

# Dependencies
find_package(DPP REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(SQLiteCpp REQUIRED)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG f091b2c061b307ee89b164c39976fc9202a1c79d) # Version 1.14.2 (https://github.com/libcpr/cpr/releases/tag/1.14.2)
set(CPR_USE_SYSTEM_CURL ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(cpr)

# Libraries
add_subdirectory(db)

# Sources
add_subdirectory(src ${CMAKE_BINARY_DIR}/bin)



