cmake_minimum_required(VERSION 3.10)
project(test_raw_data_sender)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(test_raw_data_sender test_raw_data_sender.cpp)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(test_raw_data_sender ws2_32)
endif()

# Enable warnings
if(MSVC)
    target_compile_options(test_raw_data_sender PRIVATE /W4)
else()
    target_compile_options(test_raw_data_sender PRIVATE -Wall -Wextra -pedantic)
endif()

# Install target
install(TARGETS test_raw_data_sender DESTINATION bin)
