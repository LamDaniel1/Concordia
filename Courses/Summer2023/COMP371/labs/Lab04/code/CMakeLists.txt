cmake_minimum_required(VERSION 3.11)

project(Lab_4)

include(ExternalProject)

# Set install directory
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH ${CMAKE_SOURCE_DIR}/dist FORCE)
set(CMAKE_BUILD_TYPE "Debug")
if(MSVC OR XCODE)
    set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "Debug" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)

include(BuildGLEW)
include(BuildGLFW)
include(BuildGLM)

# lab04
set(EXEC lab04)

set(ASSETS assets)

file(GLOB SRC src/*.cpp)

add_executable(${EXEC} ${SRC})

target_include_directories(${EXEC} PRIVATE include)

target_link_libraries(${EXEC} OpenGL::GL glew_s glfw glm)

list(APPEND BIN ${EXEC})
# end lab04

# install files to install location
# install(TARGETS ${BIN} DESTINATION ${CMAKE_INSTALL_PREFIX})
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})
install(DIRECTORY ${ASSETS} DESTINATION ${CMAKE_INSTALL_PREFIX})

