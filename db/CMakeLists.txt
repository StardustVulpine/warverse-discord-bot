cmake_minimum_required(VERSION 4.1)
project(wdb_database)
set(CMAKE_CXX_STANDARD 26)

set(LIB_SOURCES
        DBManager.cpp
)
add_library(${PROJECT_NAME} STATIC ${LIB_SOURCES})

set(LIB_LINK_TARGETS

)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_LINK_TARGETS})
add_dependencies(${PROJECT_NAME} ${LIB_LINK_TARGETS})

set(INCLUDE_DIR
    ${CMAKE_SOURCE_DIR}/utils
)
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})