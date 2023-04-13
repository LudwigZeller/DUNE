##################################################################
################## Build Script for DepthCamera ##################
##################################################################
cmake_minimum_required(VERSION 3.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS)
project(DepthCamera)
include(cmake/utils.cmake)

#################### Add files to compile here ###################
set(PROJECT_FILES
        main.cpp
        utils/Utils.cpp
        utils/Window.cpp
        pipeline/Pipeline.cpp
        webserver/Webserver.cpp)
##################################################################

set(OpenGL_GL_PREFERENCE LEGACY)

######################### Set Constants ##########################
# glad
set(GLAD_SRC_DIR "dependencies/glad/src")
set(GLAD_INCLUDE_DIR "dependencies/glad/include")

######################## Add Libraries ########################
# librealsense2
find_package(realsense2 REQUIRED)

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# GLFW
find_package(glfw3 3.3 REQUIRED)

# glad
include_directories(${GLAD_INCLUDE_DIR})
set(PROJECT_FILES ${PROJECT_FILES} ${GLAD_SRC_DIR}/glad.c)

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


###################### Build, Link and Copy ######################
add_executable(${PROJECT_NAME} ${PROJECT_FILES} ${GLAD_SRC_DIR}/glad.c)
target_link_libraries(${PROJECT_NAME} ${realsense2_LIBRARY} ${OPENGL_LIBRARIES} glfw dl ${OpenCV_LIBS})
makeasset(DUNELOGO dunelogo DuneLogo)
makeasset(TREE tree Tree)
