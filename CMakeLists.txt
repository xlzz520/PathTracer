#cmake版本
cmake_minimum_required(VERSION 3.16)

#项目名字
project(PathTracer LANGUAGES CXX)

#c++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Suppress warnings of the deprecation of glut functions on macOS. 
if(APPLE)
    add_definitions(-Wno-deprecated-declarations) 
endif()

# Find the packages we need. 
find_package(OpenMP REQUIRED)
find_package(assimp REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)

# OPENGL_INCLUDE_DIR, GLUT_INCLUDE_DIR, OPENGL_LIBRARIES, and GLUT_LIBRARIES are CMake built-in variables defined when the packages are found.
set(INCLUDE_DIRS   ${OPENMP_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIR} ) 
set(LIBRARIES  ${OPENMP_LIBRARIES} ${ASSIMP_LIBRARIES} ) 


# Add the list of include paths to be used to search for include files.
include_directories(${INCLUDE_DIRS})

# 包含  spdlog 的头文件
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(/opt/homebrew/include)


# Search all the .cpp files in the directory where CMakeLists lies and set them to ${SOURCE_FILES}.
# Search all the .h files in the directory where CMakeLists lies and set them to ${INCLUDE_FILES}.
file(GLOB SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp) 
file(GLOB INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)


# 添加 tinyXml 子目录
add_subdirectory(${PROJECT_SOURCE_DIR}/include/tinyxml2)

#编译优化
set(CMAKE_CXX_FLAGS "-O3")

add_executable(PathTracer
    ${SOURCE_FILES} 
    ${INCLUDE_FILES}
)

# Link the executable to the libraries. 
target_link_libraries(PathTracer ${LIBRARIES})

target_link_libraries(
        PathTracer
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        tinyxml2
)
