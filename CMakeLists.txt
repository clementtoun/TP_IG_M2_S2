cmake_minimum_required(VERSION 3.22.1)
set(PROJECT_NAME TP_IG_M2_S2)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

find_package(OpenGL REQUIRED)
set(CMAKE_PREFIX_PATH /home/clement/Qt/5.15.2/gcc_64/lib/cmake/Qt5)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

set(BUILD_SHARED_LIBS OFF)
add_subdirectory(./lib/assimp)

FILE(GLOB_RECURSE
        folder_source
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/src/OpenGL/*.cpp
        ${CMAKE_SOURCE_DIR}/src/Scene/*.cpp
        )

FILE(GLOB_RECURSE
        folder_header
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/OpenGL/*.h
        ${CMAKE_SOURCE_DIR}/src/Scene/*.h
        )

SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

add_executable(${PROJECT_NAME} ${folder_source} ${folder_header} src/Scene/DirectionalLight.cpp src/Scene/DirectionalLight.h)

set(EXT_LIBS Qt5::Core Qt5::Widgets assimp)
target_link_libraries(${PROJECT_NAME} ${EXT_LIBS} ${OPENGL_LIBRARIES})