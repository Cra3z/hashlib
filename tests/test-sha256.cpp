#include <hashlib/sha224_and_sha256.hpp>
#include "common.h"

TEST_CASE("testing sha256") {
    using namespace hashlib_testing::literals;
    hashlib::sha256 sha256;

    SUBCASE("empty string") {
        CHECK_EQ(sha256.hexdigest(), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    }

    SUBCASE("basic string") {
        sha256.update("hello world"s);
        CHECK_EQ(sha256.hexdigest(), "b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9");
    }

    SUBCASE("multiple updates") {
        sha256.update("hello"s);
        sha256.update(" "s);
        sha256.update("world"s);
        CHECK_EQ(sha256.hexdigest(), "b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9");
    }

    SUBCASE("long string") {
        std::string input(1000, 'a');  // 1000 'a' characters
        sha256.update(input);
        CHECK_EQ(sha256.hexdigest(), "41edece42d63e8d9bf515a9ba6932e1c20cbc9f5a5d134645adb5db1b9737ea3");
    }

    SUBCASE("files") {
        std::string dir = HASHLIB_TEST_DIR"/files/sha256/";
        auto filenames = {
            "12e3d508453dba4ac11545a1a4f5d684058752f27b1a87c59b4dd270ce6f0c8e",
            "193442bab43399feb8a9f755b67197563e69a4a5e24eb9d25a801e14199f4d93",
            "551b4599583cac123c4b26e1fa9d1a2009bb2c8700aa925d917f18fb30cef0eb",
            "6daf2e956ed820815f64d61bd78d35b162507009bf7e73e46767d58cb409df12",
            "86c7242ddf4762df914d53adeb7fc9eeecd4ba0fbdbc98d38925b9d115512deb",
            "931a305319903cfbbec6bcac57ec4b9a00893079e7a998caf44250cb949fab67",
            "c61f9e7c137384497ed0864e649f13dbc82d2608b2c8b573aff13f7a857a4bd8",
            "da1fe846db926bb2522bb8253cd6cf12608787737d54e055493c6389f9a81a67"
        };
        for (auto filename : filenames) {
            SUBCASE(filename) {
                std::ifstream file{dir + filename, std::ios::in | std::ios::binary};
                REQUIRE(file.is_open());
                sha256.update(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});
                CHECK_EQ(sha256.hexdigest(), filename);
            }
        }
    }
}