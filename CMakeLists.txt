cmake_minimum_required(VERSION 3.28)

project(myzsh VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(MYZSH_BUILD_TESTS "Build unit and integration tests" ON)
option(MYZSH_SANITIZE "Build with address and undefined-behaviour sanitizers" OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

add_library(myzsh_warnings INTERFACE)
target_compile_options(myzsh_warnings INTERFACE
  -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wno-sign-conversion
  -Wno-unused-parameter
  # Stubs leave members unused; drop these once the implementation lands.
  -Wno-unused-private-field -Wno-unused-const-variable)

if(MYZSH_SANITIZE)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

add_subdirectory(src)

if(MYZSH_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
