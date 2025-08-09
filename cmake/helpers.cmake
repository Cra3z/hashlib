include_guard(GLOBAL)

function(hashlib_generate_single_header)
    set(OUTPUT_FILE "${PROJECT_BINARY_DIR}/hashlib.hpp")
    file(WRITE ${OUTPUT_FILE}
"
#pragma once
#define HASHLIB_ALL_IN_ONE
#define HASHLIB_BUILD_SINGLE_HEADER
#ifdef __has_include
#if __has_include(<version>)
#include <version>
#endif
#endif
#include <algorithm>
#ifdef __cpp_lib_endian
#include <bit>
#endif
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>
"
    )
    set(HEADERS config.hpp core.hpp md5.hpp sha1.hpp sha224_and_sha256.hpp sha384_and_sha512.hpp)
    foreach(HEADER IN LISTS HEADERS)
        file(READ "${PROJECT_SOURCE_DIR}/include/hashlib/${HEADER}" _CONTENT)
        file(APPEND ${OUTPUT_FILE} "${_CONTENT}\n")
    endforeach()
endfunction()

function(hashlib_generate_cxx20_module)
    set(OUTPUT_FILE "${PROJECT_BINARY_DIR}/hashlib.cppm")
    file(WRITE ${OUTPUT_FILE}
"
module;
#include <cassert>
#define HASHLIB_ALL_IN_ONE
#define HASHLIB_BUILD_MODULE
export module hashlib;
export import std;
"
    )
    set(HEADERS config.hpp core.hpp md5.hpp sha1.hpp sha224_and_sha256.hpp sha384_and_sha512.hpp)
    foreach(HEADER IN LISTS HEADERS)
        file(READ "${PROJECT_SOURCE_DIR}/include/hashlib/${HEADER}" _CONTENT)
        file(APPEND ${OUTPUT_FILE} "${_CONTENT}\n")
    endforeach()
endfunction()