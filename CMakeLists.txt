# COMPILE SHADERS:
# $ glslc ./src/shaders/VertShader.vert -o build/vertshader.spv
# $ glslc ./src/shaders/FragShader.frag -o ./build/fragshader.spv

cmake_minimum_required(VERSION 3.25)
set(MAIN_EXECUTABLE "vulkan-raii")
project(${MAIN_EXECUTABLE} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Makes clangd happy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    # Ensure the compile_commands.json includes the standard library
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

# Default to a debug build
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()
message("-- Detected build type: ${CMAKE_BUILD_TYPE}")

# Create the main executable
file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(${MAIN_EXECUTABLE} ${SOURCES})

# Enable additional warnings
target_compile_options(${MAIN_EXECUTABLE} PRIVATE -Wall)

# Configure dependencies
add_subdirectory(Vulkan-Headers-sdk-1.3.250.1)
target_link_libraries(${MAIN_EXECUTABLE} PRIVATE Vulkan::Headers)
add_subdirectory(Vulkan-Loader-sdk-1.3.250.1)
target_link_libraries(${MAIN_EXECUTABLE} PRIVATE Vulkan::Vulkan)

add_subdirectory(./external/glm)
target_link_libraries(${MAIN_EXECUTABLE} PRIVATE glm::glm)

add_subdirectory(./external/fmt)
target_link_libraries(${MAIN_EXECUTABLE} PRIVATE fmt::fmt)

# Dont implicitly include OpenGL headers
target_compile_definitions(${MAIN_EXECUTABLE} PRIVATE GLFW_INCLUDE_NONE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(./external/glfw)
target_link_libraries(${MAIN_EXECUTABLE} PRIVATE glfw)
