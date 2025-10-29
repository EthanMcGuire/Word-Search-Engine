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
        src/timer.cpp
        src/countdownTimer.cpp
        src/gameController.cpp
        src/stringUtils.cpp

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
            include/timer.hpp
            include/countdownTimer.hpp
            include/gameController.hpp
            include/stringUtils.hpp
)

# Copy assets to build directory
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

# Copy SDL libaries to build directory
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3.dll" DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3_ttf.dll" DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3_image.dll" DESTINATION "${CMAKE_BINARY_DIR}")

# Build lib
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)
target_link_libraries(MyProgram SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf SDL3::SDL3)