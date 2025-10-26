cmake_minimum_required(VERSION 3.23)

project(MyProgram)

add_executable(MyProgram)

target_sources(MyProgram
    PRIVATE
        src/main.cpp
        src/engine.cpp
        src/fileUtils.cpp
        src/dateUtils.cpp
        src/audioController.cpp

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS 
            include
        FILES
            include/main.hpp
            include/config.hpp
            include/engine.hpp
            include/fileUtils.hpp
            include/dateUtils.hpp
            include/audioController.hpp
)

find_package(SDL3 REQUIRED)
target_link_libraries(MyProgram SDL3::SDL3)