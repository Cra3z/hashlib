#pragma once
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <fstream>
#include "doctest/doctest.h"

namespace hashlib_testing {
    namespace literals {
#ifdef __cpp_lib_string_udls
        using std::string_literals::operator""s;
#else
        inline auto operator""s(const char* str, std::size_t length) -> std::string {
            return std::string{str, length};
        }
#endif
    }
}