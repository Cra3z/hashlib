#pragma once
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <fstream>
#include "doctest/doctest.h"

namespace hashlib_testing {
    namespace literals {
        inline auto operator""_s(const char* str, std::size_t length) -> std::string {
            return std::string{str, length};
        }
    }
}