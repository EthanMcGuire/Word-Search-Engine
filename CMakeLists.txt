cmake_minimum_required(VERSION 3.23)

project(MyProgram)

add_executable(MyProgram)

target_include_directories(MyProgram PRIVATE include/engine)
target_include_directories(MyProgram PRIVATE include/external)

target_sources(MyProgram
    PRIVATE
        src/main.cpp
        src/engine.cpp
        src/fileUtils.cpp
        src/dateUtils.cpp
        src/audioController.cpp
        src/timer.cpp
        src/countdownTimer.cpp
        src/gameManager.cpp
        src/stringUtils.cpp
        src/jsonUtils.cpp

        src/room.cpp
        src/camera.cpp
        src/cameraLerp.cpp
        src/eventDispatcher.cpp
        src/assetManager.cpp
        src/texture.cpp
        src/spriteAtlas.cpp
        src/bitmapFont.cpp
        src/random.cpp
        src/collision.cpp
        src/draw.cpp
        src/object.cpp
        src/objectManager.cpp
        src/parameterVariant.cpp
        src/objectFactory.cpp
        src/objectDrawer.cpp
        src/collisionDrawer.cpp
        src/renderableObject.cpp
	src/objGameClock.cpp
	src/objTitleScreen.cpp
	src/objWordSearchController.cpp
	src/objWordSearchBox.cpp
	src/nineSlice.cpp
	src/drawingSurface.cpp
	src/objRose.cpp
	src/objLeaderboard.cpp
	src/textInputHandler.cpp
	src/scoreTable.cpp
)

# Copy assets to build directory
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

# Copy link libraries for windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	# Copy SDL libaries to build directory
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3.dll" DESTINATION "${CMAKE_BINARY_DIR}")
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3_ttf.dll" DESTINATION "${CMAKE_BINARY_DIR}")
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3_image.dll" DESTINATION "${CMAKE_BINARY_DIR}")
endif()

# Build lib
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)
target_link_libraries(MyProgram SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf SDL3::SDL3)
