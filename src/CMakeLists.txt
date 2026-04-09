cmake_minimum_required(VERSION 4.1)
set(CMAKE_CXX_STANDARD 23)

set(EXECUTABLES_LIST
        main.cpp
        Bot.cpp
)
add_executable(${PROJECT_NAME} ${EXECUTABLES_LIST})

set(LIB_LINK_TARGETS
        dpp::dpp
        nlohmann_json::nlohmann_json
        ${SQLite3_LIBRARIES}
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_LINK_TARGETS})

set(LIB_DEPENDENCY_LIST
        wdb_commands
)
add_dependencies(${PROJECT_NAME} ${LIB_DEPENDENCY_LIST})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR} ${SQLite3_INCLUDE_DIRS})