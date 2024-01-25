cmake_minimum_required(VERSION 3.5)

project(test4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
        main.cpp
        data.json
)

add_executable(test4 ${PROJECT_SOURCES})

file(COPY_FILE ${PROJECT_SOURCE_DIR}/data.json ${CMAKE_CURRENT_BINARY_DIR}/data.json)
