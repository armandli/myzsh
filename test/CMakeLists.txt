find_package(GTest CONFIG QUIET)
if(NOT GTest_FOUND)
  include(FetchContent)
  FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif()
include(GoogleTest)

add_library(myzsh_test_support STATIC
  support/test_support.cpp)
target_include_directories(myzsh_test_support PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(myzsh_test_support PUBLIC myzsh_core GTest::gtest)

file(GLOB MYZSH_UNIT_TESTS CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/unit/*_test.cpp)
add_executable(unit_tests ${MYZSH_UNIT_TESTS})
target_link_libraries(unit_tests PRIVATE
  myzsh_test_support GTest::gtest_main myzsh_warnings)
# PRE_TEST: listing tests must not run at build time while most code is stubs.
gtest_discover_tests(unit_tests DISCOVERY_MODE PRE_TEST
  PROPERTIES TIMEOUT 30)

add_executable(integration_tests
  integration/ztst_runner.cpp
  integration/integration_test.cpp)
target_link_libraries(integration_tests PRIVATE
  myzsh_test_support GTest::gtest_main myzsh_warnings)
target_compile_definitions(integration_tests PRIVATE
  MYZSH_BINARY="$<TARGET_FILE:myzsh>"
  MYZSH_ZTST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/integration/cases")
add_dependencies(integration_tests myzsh)
gtest_discover_tests(integration_tests DISCOVERY_MODE PRE_TEST
  PROPERTIES TIMEOUT 60)
