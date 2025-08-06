#include <hashlib/sha384_and_sha512.hpp>
#include "common.h"

TEST_CASE("testing sha384") {
    using namespace hashlib_testing::literals;
    hashlib::sha384 sha384;

    SUBCASE("empty string") {
        CHECK_EQ(sha384.hexdigest(), "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b");
    }

    SUBCASE("basic string") {
        sha384.update("hello world"s);
        CHECK_EQ(sha384.hexdigest(), "fdbd8e75a67f29f701a4e040385e2e23986303ea10239211af907fcbb83578b3e417cb71ce646efd0819dd8c088de1bd");
    }

    SUBCASE("multiple updates") {
        sha384.update("The"s);
        sha384.update(" quick "s);
        sha384.update("brown fox"s);
        CHECK_EQ(sha384.hexdigest(), "2e45933dd1a1e6a6928a732d58abeb180c225e5e7b99c64eb6f233a7b99ee4635c17f44ca544cf620cf4289deb4c08cf");
    }

    SUBCASE("files") {
        std::string dir = HASHLIB_TEST_DIR"/files/sha384/";
        auto filenames = {
            "3d9fb989158595d0ea5fb7ae30318147b1ed764fd0c96acfb6290d777df7e4c8fb12908b25f739914a4cf01f72eabf20",
            "3ecd1372c63ff8030d3ba6853e122ced929dadd84c735df93c524874ab17d327f3bba408b30a4cd2e608445a1a979b87",
            "7e9aa3a0619051792dc536001276513b57530d27686d1277f527d9002fdaed83b79143b9372813d2b19dfe039193d390",
            "8f1dde3380cd394a6fca6496be749e53a72352c78e19403b4d2fa7226da6659c9ab973027f9e47f6d4b5bb36f84f1473",
            "aeee0a86f63b20bea8ce9d0f8e0bc80965c76d88e006fd35c47dd181d34d481b7219a2ddf5103e9e9ac56c1145d21e9e",
            "b2675436699b15de689bd1a02fdf87bd7700138506f9c414fd9662d901fbb11ff91db09f2fcf700dc74d53a6b5b8973e",
            "c60c9d701c33503426b56a006e1296e571e69d5643793f08e91ec383941fb6d0ccf1269912fb471aaf07aca5f6a49ca1",
            "eb54f85260b61517c730eabb2ddef6bda18af808b4839974d701ba262c3a93c569c37775f1a40b5df212612322b651fa"
        };
        for (auto filename : filenames) {
            SUBCASE(filename) {
                std::ifstream file{dir + filename, std::ios::in | std::ios::binary};
                REQUIRE(file.is_open());
                sha384.update(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});
                CHECK_EQ(sha384.hexdigest(), filename);
            }
        }
    }
}