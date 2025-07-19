cmake_minimum_required(VERSION 3.16)
project(VideoEnhancement)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Add executable
add_executable(video_enhancement main.cpp)

# Link libraries
target_link_libraries(video_enhancement ${OpenCV_LIBS})

# Set compiler flags for optimization
if(MSVC)
    target_compile_options(video_enhancement PRIVATE /O2)
else()
    target_compile_options(video_enhancement PRIVATE -O3 -march=native)
endif()

# Copy input video for testing (if exists)
configure_file(${CMAKE_SOURCE_DIR}/input.mp4 ${CMAKE_BINARY_DIR}/input.mp4 COPYONLY)
