cmake_minimum_required(VERSION 3.7.2 FATAL_ERROR)

set(CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wpedantic -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

file(COPY "${CMAKE_SOURCE_DIR}/figure" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(hangman
	hangman.cpp)
