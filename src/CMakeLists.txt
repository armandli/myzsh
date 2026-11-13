file(GLOB_RECURSE MYZSH_CORE_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
list(REMOVE_ITEM MYZSH_CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# Everything except main() lives in a library so the tests can link it.
add_library(myzsh_core STATIC ${MYZSH_CORE_SOURCES})
target_include_directories(myzsh_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(myzsh_core PUBLIC
  MYZSH_VERSION="${PROJECT_VERSION}")
target_link_libraries(myzsh_core PRIVATE myzsh_warnings)

add_executable(myzsh main.cpp)
target_link_libraries(myzsh PRIVATE myzsh_core myzsh_warnings)
