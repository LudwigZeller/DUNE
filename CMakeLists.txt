##################################################################
################## Build Script for DepthCamera ##################
##################################################################
cmake_minimum_required(VERSION 3.1.0)
set(CMAKE_CXX_STANDARD 14)
project(DepthCamera)
include(cmake/utils.cmake)

#################### Add files to compile here ###################
set(PROJECT_FILES main.cpp utils.cpp)
##################################################################

######################### Set Constants ##########################
# librealsense2
set(REALSENSE_INCLUDE_DIR "dependencies/librealsense2/include/")
if (MSVC)
    set(REALSENSE_LIB_DIR "dependencies/librealsense2/lib/x64")
    set(REALSENSE_DLL_FILE "dependencies/librealsense2/bin/x64/realsense2.dll")
else()
    set(REALSENSE_LIB_DIR "dependencies/librealsense2/lib/x86")
    set(REALSENSE_DLL_FILE "dependencies/librealsense2/bin/x86/realsense2.dll")
endif ()

# GLFW
set(GLFW_DIR "dependencies/glfw")
set(GLFW_INCLUDE_DIR "dependencies/glfw/include")

# glad
set(GLAD_SRC_DIR "dependencies/glad/src")
set(GLAD_INCLUDE_DIR "dependencies/glad/include")

# freetype
set(FREETYPE_DIR "dependencies/freetype-2.12.1")
set(FREETYPE_INCLUDE_DIR "dependencies/freetype-2.12.1/include")

######################## Locate Libraries ########################
# librealsense2
include_directories(${REALSENSE_INCLUDE_DIR})
link_directories(${REALSENSE_LIB_DIR})

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# GLFW
add_subdirectory(${GLFW_DIR})
include_directories(${GLFW_INCLUDE_DIR})

# glad
include_directories(${GLAD_INCLUDE_DIR})
set(PROJECT_FILES ${PROJECT_FILES} ${GLAD_SRC_DIR}/glad.c)

# freetype
#add_subdirectory(${FREETYPE_DIR})
#include_directories(${FREETYPE_INCLUDE_DIR})

###################### Build, Link and Copy ######################
add_executable(${PROJECT_NAME} ${PROJECT_FILES} ${GLAD_SRC_DIR}/glad.c)
target_link_libraries(${PROJECT_NAME} realsense2.lib ${OPENGL_LIBRARIES} glfw)
copy_files(${REALSENSE_DLL_FILE})
copy_directory(/shader)


