cmake_minimum_required(VERSION 3.25)
set(MAIN_EXECUTABLE "vulkan-raii")
project(${MAIN_EXECUTABLE} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # For the LSP

# Default to a debug build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message("-- Detected build type: ${CMAKE_BUILD_TYPE}")

file(GLOB_RECURSE SOURCES src/*.cpp)

# Create the main executable
add_executable(${MAIN_EXECUTABLE}  ${SOURCES})

# Enable additional warnings
# target_compile_options(${MAIN_EXECUTABLE} PRIVATE -Wall)

# Configure dependencies
add_subdirectory(Vulkan-Headers-sdk-1.3.250.1)
target_link_libraries(${MAIN_EXECUTABLE} Vulkan::Headers)
add_subdirectory(Vulkan-Loader-sdk-1.3.250.1)
target_link_libraries(${MAIN_EXECUTABLE} Vulkan::Vulkan)

add_subdirectory(glm-0.9.9.8)
target_link_libraries(${MAIN_EXECUTABLE} glm::glm)

add_subdirectory(glfw-3.3.8)
target_link_libraries(${MAIN_EXECUTABLE} glfw)

add_subdirectory(fmt-10.1.0)
target_link_libraries(${MAIN_EXECUTABLE} fmt::fmt)