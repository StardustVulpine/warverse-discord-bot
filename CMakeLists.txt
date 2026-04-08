cmake_minimum_required(VERSION 4.1)
project(warverse_discord_bot)

set(CMAKE_CXX_STANDARD 23)

find_package(DPP REQUIRED)

add_executable(warverse_discord_bot
        main.cpp
        Bot.cpp
        Bot.hpp
        Command.cpp
        Command.hpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE dpp::dpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

configure_file("${CMAKE_SOURCE_DIR}/.token" "${CMAKE_BINARY_DIR}/.token")