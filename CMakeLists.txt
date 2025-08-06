cmake_minimum_required(VERSION 3.22)

project(
    hashlib
    VERSION 1.0.0
    DESCRIPTION "A C++ header-only hash algorithm library"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/permissive->")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>")

option(HASHLIB_TESTS "enable tests for hashlib" OFF)
option(HASHLIB_BUILD_SINGLE_HEADER "build hashlib single header" OFF)
option(HASHLIB_BUILD_MODULE "build hashlib cxx20 module" OFF)

include(CMakePackageConfigHelpers)
include(cmake/helpers.cmake)

add_library(
    ${PROJECT_NAME}
    INTERFACE
)

target_include_directories(
    ${PROJECT_NAME}
    INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${PROJECT_NAME}/include>
)

if (HASHLIB_BUILD_MODULE)
    hashlib_generate_cxx20_module()
endif()

if (HASHLIB_BUILD_SINGLE_HEADER)
    hashlib_generate_single_header()
endif()

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include
    DESTINATION ${PROJECT_NAME}
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    DESTINATION ${PROJECT_NAME}/lib/${CMAKE_BUILD_TYPE}
)

install(
    EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}-targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${PROJECT_NAME}/lib/cmake/${PROJECT_NAME}
)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in
    ${PROJECT_NAME}-config.cmake
    INSTALL_DESTINATION ${PROJECT_NAME}/lib/cmake/${PROJECT_NAME}
)

install(
    FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    DESTINATION ${PROJECT_NAME}/lib/cmake/${PROJECT_NAME}
)

if (HASHLIB_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()