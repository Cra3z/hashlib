#include <hashlib/md5.hpp>
#include "common.h"

TEST_CASE("testing md5") {
    using namespace hashlib_testing::literals;
    hashlib::md5 md5;

    SUBCASE("empty string") {
        CHECK_EQ(md5.hexdigest(), "d41d8cd98f00b204e9800998ecf8427e");
    }

    SUBCASE("basic string") {
        md5.update("hello world"_s);
        CHECK_EQ(md5.hexdigest(), "5eb63bbbe01eeed093cb22bb8f5acdc3");
    }

    SUBCASE("multiple updates") {
        md5.update("hello"_s);
        md5.update(" "_s);
        md5.update("world"_s);
        CHECK_EQ(md5.hexdigest(), "5eb63bbbe01eeed093cb22bb8f5acdc3");
    }

    SUBCASE("binary data") {
        unsigned char data[] = {0x11, 0xff, 0x1a, 0x55, 0x1a, 0x12, 0x34, 0x1f, 0x56, 0x78};
        md5.update({data, sizeof(data)});
        CHECK_EQ(md5.hexdigest(), "5866dd029297014ce2ac5bb21825255a");
    }

    SUBCASE("files") {
        std::string dir = HASHLIB_TEST_DIR"/files/md5/";
        auto filenames = {
            "06c9519ce09c6b0b90d8bc57ae667688",
            "44aac8e812baf7fb5b9f500e9b3cc12c",
            "5bb6ec72ba20abddaeb425399bffc5ce",
            "9ad0e07895da17fded0c5f5a8272cf71",
            "ba23df1f1b697a53f46b08ca1c639431",
            "d78f663754530057bd3f594cd4af3ebf",
            "ebda2fb81a1136a1f445b187291b9bd4",
            "f152cc565a09eca8fd18cdf8dc08a0a3"
        };
        for (auto filename : filenames) {
            SUBCASE(filename) {
                std::ifstream file{dir + filename, std::ios::in | std::ios::binary};
                REQUIRE(file.is_open());
                md5.update(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});
                CHECK_EQ(md5.hexdigest(), filename);
            }
        }
    }
}