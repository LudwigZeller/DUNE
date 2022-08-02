##################################################################
################## Build Script for DepthCamera ##################
##################################################################
cmake_minimum_required(VERSION 3.1.0)
set(CMAKE_CXX_STANDARD 14)
project(DepthCamera)
include(cmake/utils.cmake)

#################### Add files to compile here ###################
set(PROJECT_FILES main.cpp)
##################################################################

######################### Set Constants ##########################
set(REALSENSE_INCLUDE_DIR "dependencies/librealsense2/include/")
set(REALSENSE_LIB_DIR "dependencies/librealsense2/lib/x64")
set(REALSENSE_DLL_FILE "dependencies/librealsense2/bin/x64/realsense2.dll")

set(GLFW_DIR "dependencies/glfw")
set(GLFW_INCLUDE_DIR "dependencies/glfw/include")

######################## Locate Libraries ########################
# librealsense2
include_directories(${REALSENSE_INCLUDE_DIR})
link_directories(${REALSENSE_LIB_DIR})

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# Glfw
add_subdirectory(${GLFW_DIR})
include_directories(${GLFW_INCLUDE_DIR})

###################### Build, Link and Copy ######################
add_executable(${PROJECT_NAME} ${PROJECT_FILES})
target_link_libraries(${PROJECT_NAME} realsense2.lib ${OPENGL_LIBRARIES} glfw)
copy_files(${REALSENSE_DLL_FILE})


