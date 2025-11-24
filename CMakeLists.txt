cmake_minimum_required(VERSION 3.22)

project(
    hashlib
    VERSION 1.1.1
    DESCRIPTION "A C++ header-only hash algorithm library"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/permissive->")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>")

option(HASHLIB_TESTS "enable tests for hashlib" ON)
option(HASHLIB_BUILD_SINGLE_HEADER "generate hashlib single header" OFF)
option(HASHLIB_BUILD_MODULE "generate hashlib cxx20 module" OFF)

include(CMakePackageConfigHelpers)
include(cmake/helpers.cmake)

add_library(
    ${PROJECT_NAME}
    INTERFACE
)

add_library(
    ${PROJECT_NAME}::${PROJECT_NAME}
    ALIAS
    ${PROJECT_NAME}
)

set(
    HASHLIB_PUBLIC_HEADERS
    "${PROJECT_SOURCE_DIR}/include/hashlib/config.hpp"
    "${PROJECT_SOURCE_DIR}/include/hashlib/core.hpp"
    "${PROJECT_SOURCE_DIR}/include/hashlib/md5.hpp"
    "${PROJECT_SOURCE_DIR}/include/hashlib/sha1.hpp"
    "${PROJECT_SOURCE_DIR}/include/hashlib/sha2.hpp"
    "${PROJECT_SOURCE_DIR}/include/hashlib/sha3.hpp"
)

target_sources(
    ${PROJECT_NAME}
    INTERFACE
    FILE_SET hashlib_public_headers
    TYPE HEADERS
    BASE_DIRS ${PROJECT_SOURCE_DIR}/include
    FILES ${HASHLIB_PUBLIC_HEADERS}
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
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    FILE_SET hashlib_public_headers DESTINATION ${PROJECT_NAME}/include
)

install(
    EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}-targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${PROJECT_NAME}/lib/cmake/${PROJECT_NAME}
)

export(
    EXPORT ${PROJECT_NAME}-targets
    NAMESPACE ${PROJECT_NAME}::
    FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-targets.cmake
)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in
    ${PROJECT_NAME}-config.cmake
    INSTALL_DESTINATION ${PROJECT_NAME}/lib/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
    ${PROJECT_NAME}-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    ARCH_INDEPENDENT
)

install(
    FILES
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    DESTINATION ${PROJECT_NAME}/lib/cmake/${PROJECT_NAME}
)

if (HASHLIB_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()