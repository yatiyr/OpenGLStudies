cmake_minimum_required(VERSION 3.10)
project(OpenGLStudies VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCE_FILES
     ${CMAKE_SOURCE_DIR}/src/*.c
     ${CMAKE_SOURCE_DIR}/src/*.cpp)

file(GLOB_RECURSE HEADER_FILES 
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_SOURCE_DIR}/src/*.hpp)

link_directories(${CMAKE_SOURCE_DIR}/lib)

configure_file(src/helpers/RootDir.h.in src/helpers/RootDir.h)
include_directories(${CMAKE_BINARY_DIR}/src)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


find_package(OpenGL REQUIRED)

find_package(glm REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

find_package(glfw3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

find_package(assimp REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")


# Define the include DIRs
include_directories(
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/include"
     "${CMAKE_SOURCE_DIR}/thirdparty/include"
)

# STB_IMAGE
add_library(STB_IMAGE "${CMAKE_SOURCE_DIR}/thirdparty/stb_image.cpp")

# GLAD
add_library(GLAD "${CMAKE_SOURCE_DIR}/thirdparty/glad.c")

set(LIBS glfw3 GL X11 pthread  Xrandr Xi dl assimp STB_IMAGE GLAD)

target_link_libraries(${PROJECT_NAME} ${LIBS})


file(COPY src/shaders DESTINATION ${CMAKE_BINARY_DIR})