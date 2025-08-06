#include <hashlib/sha384_and_sha512.hpp>
#include "common.h"

TEST_CASE("testing sha512") {
    using namespace hashlib_testing::literals;
    hashlib::sha512 sha512;

    SUBCASE("empty string") {
        CHECK_EQ(sha512.hexdigest(), "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
    }

    SUBCASE("basic string") {
        sha512.update("hello world"s);
        CHECK_EQ(sha512.hexdigest(), "309ecc489c12d6eb4cc40f50c902f2b4d0ed77ee511a7c7a9bcd3ca86d4cd86f989dd35bc5ff499670da34255b45b0cfd830e81f605dcf7dc5542e93ae9cd76f");
    }

    SUBCASE("multiple updates with clear") {
        sha512.update("test1"s);
        sha512.clear();
        sha512.update("hello"s);
        sha512.update(" "s);
        sha512.update("world"s);
        CHECK_EQ(sha512.hexdigest(), "309ecc489c12d6eb4cc40f50c902f2b4d0ed77ee511a7c7a9bcd3ca86d4cd86f989dd35bc5ff499670da34255b45b0cfd830e81f605dcf7dc5542e93ae9cd76f");
    }

    SUBCASE("binary data with zeros") {
        std::vector<hashlib::byte> data(128, 0);  // 128 zero bytes
        sha512.update(data);
        CHECK_EQ(sha512.hexdigest(), "ab942f526272e456ed68a979f50202905ca903a141ed98443567b11ef0bf25a552d639051a01be58558122c58e3de07d749ee59ded36acf0c55cd91924d6ba11");
    }

    SUBCASE("files") {
        std::string dir = HASHLIB_TEST_DIR"/files/sha512/";
        auto filenames = {
            "15341a0518c9289fdefd762799d534d05aa16dd61ab12f2eff993a146e74876d6b4ee3ca0f1b17e1495fe8885eca4e7082b51dcd05b9ae9ac9c4fff8d1ad9a01",
            "1d5d8096763b41a72a47516645caff6784981175f23e80e1f50627bb19294247b4b7c712af6769026ba098c2b081e482608dcd0acb9623119bf505a4c695d662",
            "4e8082289244e9e105588e29a1e9f21c7a6455a013a4001a1eaa7249de628fa49b05a248e97bfe0de0a11b69b3cb3a1b6cee0773d2ba819204851453466cb50f",
            "6d04969a37fd7b28ef9510012f0ee80aaa172bc114f6c39cd7692627f3051d9ef2dac0b434119def797efaf1d4de37cfdc09e3260f2fe499a1868abc0b645964",
            "a98e0ca6b3a0b61c65d697498ae573d81cff80f50a376ba5b31b705da2bad340f7d98845e938d899c78591354c0e5638b7f1cc2857801c367d21937a1da6baf0",
            "aa57fc610f351d61a50d32fc588a2289659a5fb25a1a67a5fd1587b91b34ff7e390f610018a99ca77c78e7207340e6c1c1016215180d5faefba709151013bb74",
            "c9f76628fec04e025f12dc81749c2bac0bfb6a2e821991d29f7cca510bd0f0cab1f31ae46ca2baa48f02db2cb1fb921e6b280ba168815f7978fac2071ca61b85",
            "cf8f8c903f326664c93ce0586e744edf84b654ebb6feec8c857eeff453bdf22ffcbc847e01bfa1f2d0f54f53849045d33b6ead657118de57444ef30ee31cc39e"
        };
        for (auto filename : filenames) {
            SUBCASE(filename) {
                std::ifstream file{dir + filename, std::ios::in | std::ios::binary};
                REQUIRE(file.is_open());
                sha512.update(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});
                CHECK_EQ(sha512.hexdigest(), filename);
            }
        }
    }
}