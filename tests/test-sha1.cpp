#include <hashlib/sha1.hpp>
#include "common.h"

TEST_CASE("testing sha1") {
    using namespace hashlib_testing::literals;
    hashlib::sha1 sha1;

    SUBCASE("empty string") {
        CHECK_EQ(sha1.hexdigest(), "da39a3ee5e6b4b0d3255bfef95601890afd80709");
    }

    SUBCASE("basic string") {
        sha1.update("hello world"_s);
        CHECK_EQ(sha1.hexdigest(), "2aae6c35c94fcfb415dbe95f408b9ce91ee846ed");
    }

    SUBCASE("multiple updates") {
        sha1.update("hello"_s);
        sha1.update(" "_s);
        sha1.update("world"_s);
        CHECK_EQ(sha1.hexdigest(), "2aae6c35c94fcfb415dbe95f408b9ce91ee846ed");
    }

    SUBCASE("binary data") {
        unsigned char data[] = {0x00, 0xff, 0x55, 0xaa, 0x12, 0x34, 0x56, 0x78};
        sha1.update({data, sizeof(data)});
        CHECK_EQ(sha1.hexdigest(), "f9d9a450e6e14895936f8dc796e30209528de337");
    }

    SUBCASE("files") {
        std::string dir = HASHLIB_TEST_DIR"/files/sha1/";
        auto filenames = {
            "3d52d355fbf91d56b83eedd4644b715d9aa49bae",
            "416cb0897ef2182a7ab84676dc6b564634d71dcc",
            "644be18a14f35bdd5db69ab263ad6cb0caea6107",
            "859b38d005b91f31ac04266d81c6f99700d39608",
            "8776da5134b4e78100a44851eb52477ce13577b8",
            "8d5d7d2f990ecf429e2ad0979a07d1ad41c9003f",
            "f5621345b8439151aeb50f317d9e5866392b2b6e",
            "fa50a410b7e97f1ebf24b3653c3d1f0785db6b18"
        };
        for (auto filename : filenames) {
            SUBCASE(filename) {
                std::ifstream file{dir + filename, std::ios::in | std::ios::binary};
                REQUIRE(file.is_open());
                sha1.update(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});
                CHECK_EQ(sha1.hexdigest(), filename);
            }
        }
    }
}