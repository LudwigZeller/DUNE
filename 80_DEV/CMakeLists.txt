##################################################################
################## Build Script for DepthCamera ##################
##################################################################
cmake_minimum_required(VERSION 3.1.0)
set(CMAKE_CXX_STANDARD 17)
project(DepthCamera)
include(cmake/utils.cmake)

#################### Add files to compile here ###################
set(PROJECT_FILES main.cpp
        Utilities/utils.cpp
        Utilities/Window.cpp
        Utilities/Texture.cpp)
##################################################################

set(OpenGL_GL_PREFERENCE LEGACY)

######################### Set Constants ##########################
# librealsense2
#set(REALSENSE_INCLUDE_DIR "dependencies/librealsense2/include/")
#if (MSVC)
#    set(REALSENSE_LIB_DIR "dependencies/librealsense2/lib/x64")
#    set(REALSENSE_DLL_FILE "dependencies/librealsense2/bin/x64/realsense2.dll")
#else ()
#    set(REALSENSE_LIB_DIR "dependencies/librealsense2/lib/x86")
#    set(REALSENSE_DLL_FILE "dependencies/librealsense2/bin/x86/realsense2.dll")
#endif ()

# GLFW
#set(GLFW_DIR "dependencies/glfw")
#set(GLFW_INCLUDE_DIR "dependencies/glfw/include")

# glad
set(GLAD_SRC_DIR "dependencies/glad/src")
set(GLAD_INCLUDE_DIR "dependencies/glad/include")

# OpenCV
#set(OpenCV_DIR "/usr/local/bin")
#set(OpenCV_DLL_DIR "/usr/local/lib")

######################## Add Libraries ########################
# librealsense2
find_package(realsense2 REQUIRED)
#link_directories(${REALSENSE_LIB_DIR})

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# GLFW
find_package(glfw3 3.3 REQUIRED)
#include_directories(${GLFW_INCLUDE_DIR})

# glad
include_directories(${GLAD_INCLUDE_DIR})
set(PROJECT_FILES ${PROJECT_FILES} ${GLAD_SRC_DIR}/glad.c)

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


###################### Build, Link and Copy ######################
add_executable(${PROJECT_NAME} ${PROJECT_FILES} ${GLAD_SRC_DIR}/glad.c)
target_link_libraries(${PROJECT_NAME} ${realsense2_LIBRARY} ${OPENGL_LIBRARIES} glfw dl ${OpenCV_LIBS}) #opencv_world460.lib
#copy_files(${REALSENSE_DLL_FILE})
#copy_directory(/shader)
#copy_directory_dll(${OpenCV_DLL_DIR})