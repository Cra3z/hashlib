#include <hashlib/sha224_and_sha256.hpp>
#include "common.h"

TEST_CASE("testing sha224") {
    using namespace hashlib_testing::literals;
    hashlib::sha224 sha224;

    SUBCASE("empty string") {
        CHECK_EQ(sha224.hexdigest(), "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f");
    }

    SUBCASE("basic string") {
        sha224.update("hello world"s);
        CHECK_EQ(sha224.hexdigest(), "2f05477fc24bb4faefd86517156dafdecec45b8ad3cf2522a563582b");
    }

    SUBCASE("multiple updates") {
        sha224.update("The quick brown fox "s);
        sha224.update("jumps over "s);
        sha224.update("the lazy dog"s);
        CHECK_EQ(sha224.hexdigest(), "730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525");
    }

    SUBCASE("files") {
        std::string dir = HASHLIB_TEST_DIR"/files/sha224/";
        auto filenames = {
            "080d5e1e816ab258c96f47d76049fa8d2ff1f861e706078229138e54",
            "2003c6f20ebbb0c41490ad75083b134e114e837d6fa110935322f37e",
            "6169b6346f300c32966339ceff40f729d860b3e0abb0cf93b11bb38a",
            "76629075cf4e4d751724bd6fd99b837645c36530255226da5857d6d1",
            "78e76fed61f66837e98146087ee533ddba4eb1c98c9de86fc816418a",
            "9146d5f24db8c3bfc1ae0bad5865454d7a8b017cec113dd17b60630c",
            "d53108cf312e9c24367c4b0b8bcdadcafd2cd91e6ea851d44b216d0c",
            "ea572f8f5ff4b1adf0d8f5e5f361015319fc9941062e82bb9e848fae"
        };
        for (auto filename : filenames) {
            SUBCASE(filename) {
                std::ifstream file{dir + filename, std::ios::in | std::ios::binary};
                REQUIRE(file.is_open());
                sha224.update(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});
                CHECK_EQ(sha224.hexdigest(), filename);
            }
        }
    }
}