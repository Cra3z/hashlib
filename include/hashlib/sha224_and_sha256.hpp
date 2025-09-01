#ifndef HASHLIB_ALL_IN_ONE
#pragma once
#include "core.hpp"
#endif

namespace hashlib {
    namespace detail {
        class sha224_256_base {
        protected:
            sha224_256_base(const std::array<std::uint32_t, 8>& init_state) noexcept : h_(init_state) {}

        public:
            auto update(span<const byte> bytes) noexcept -> void {
                update(bytes.begin(), bytes.end());
            }

            template<typename InputIt, typename Sentinel, enable_if_t<
                is_input_iterator<InputIt>::value &&
                !is_random_access_iterator<InputIt>::value &&
                is_sentinel_for<Sentinel, InputIt>::value &&
                is_byte_like<iter_value_t<InputIt>>::value
            >* = nullptr>
            auto update(InputIt first, Sentinel last) -> void {
                byte temp[128];
                for (auto it = first; it != last;) {
                    std::size_t n = 0;
                    for (; it != last && n < sizeof(temp); ++it, ++n) {
                        temp[n] = static_cast<byte>(*it);
                    }
                    update({temp, n});
                }
            }

            template<typename RandomAccessIt, typename Sentinel, enable_if_t<
                is_random_access_iterator<RandomAccessIt>::value &&
                is_sentinel_for<Sentinel, RandomAccessIt>::value &&
                is_byte_like<iter_value_t<RandomAccessIt>>::value
            >* = nullptr>
            auto update(RandomAccessIt first, Sentinel last) -> void {
                std::size_t bytes_count = last - first;
                total_size_ += bytes_count;
                std::size_t i = 0;

                if (buffer_size_ > 0) {
                    std::size_t to_copy = std::min(bytes_count, 64 - buffer_size_);
                    std::copy_n(first, to_copy, buffer_.data() + buffer_size_);
                    buffer_size_ += to_copy;
                    i += to_copy;
                    if (buffer_size_ == 64) {
                        process_(w_table_(buffer_.data()));
                        buffer_size_ = 0;
                    }
                }

                for (; i + 63 < bytes_count; i += 64) {
                    (process_)((w_table_)(std::next(first, i)));
                }

                if (i < bytes_count) {
                    std::copy_n(std::next(first, i), bytes_count - i, buffer_.data());
                    buffer_size_ = bytes_count - i;
                }
            }

        protected:
            auto do_digest() noexcept -> std::array<std::uint32_t, 8> {
                auto_restorer<sha224_256_base> _{*this};
                auto buffer_size = buffer_size_;
                auto total_size = total_size_;
                byte padding[128]{};
                padding[0] = 0x80;
                std::size_t padding_size = (buffer_size < 56) ? (56 - buffer_size) : (120 - buffer_size);
                update({padding, padding_size});
                std::uint64_t bits = total_size * 8;
                for (std::size_t i = 0; i < 8; ++i) {
                    padding[7 - i] = bits & 0xff;
                    bits >>= 8;
                }
                update({padding, 8});
                return h_;
            }

            HASHLIB_ALWAYS_INLINE
            static auto unit_to_bytes(std::uint32_t unit) noexcept -> std::array<byte, 4> {
                byte* byte_ptr = reinterpret_cast<byte*>(&unit);
                if HASHLIB_CXX17_CONSTEXPR (is_little_endian()) {
                    return {byte_ptr[3], byte_ptr[2], byte_ptr[1], byte_ptr[0]};
                }
                else {
                    return {byte_ptr[0], byte_ptr[1], byte_ptr[2], byte_ptr[3]};
                }
            }

        private:
            auto process_(const std::array<std::uint32_t, 64>& w) noexcept -> void {
                static constexpr std::uint32_t k[64] = {
                    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
                    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
                    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
                    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
                    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
                    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
                    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
                    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
                    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
                    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
                    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
                    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
                    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
                };

                auto a = h_[0], b = h_[1], c = h_[2], d = h_[3],
                     e = h_[4], f = h_[5], g = h_[6], h = h_[7];

                for (std::size_t i = 0; i < 64; ++i) {
                    const auto S1 = rotr32(e, 6) ^ rotr32(e, 11) ^ rotr32(e, 25);
                    const auto ch = (e & f) ^ ((~e) & g);
                    const auto temp1 = h + S1 + ch + k[i] + w[i];
                    const auto S0 = rotr32(a, 2) ^ rotr32(a, 13) ^ rotr32(a, 22);
                    const auto maj = (a & b) ^ (a & c) ^ (b & c);
                    const auto temp2 = S0 + maj;

                    h = g;
                    g = f;
                    f = e;
                    e = d + temp1;
                    d = c;
                    c = b;
                    b = a;
                    a = temp1 + temp2;
                }

                h_[0] += a;
                h_[1] += b;
                h_[2] += c;
                h_[3] += d;
                h_[4] += e;
                h_[5] += f;
                h_[6] += g;
                h_[7] += h;
            }

            template<typename RandomAccessIt>
            static auto w_table_(RandomAccessIt it) noexcept -> std::array<std::uint32_t, 64> {
                static_assert(is_random_access_iterator<RandomAccessIt>::value, "unexpected");
                std::array<std::uint32_t, 64> w; // NOLINT(*-pro-type-member-init)
                for (std::size_t i = 0; i < 16; ++i) {
                    w[i] = (std::uint32_t(it[i * 4]) << 24) |
                           (std::uint32_t(it[i * 4 + 1]) << 16) |
                           (std::uint32_t(it[i * 4 + 2]) << 8) |
                           (std::uint32_t(it[i * 4 + 3]));
                }
                for (std::size_t i = 16; i < 64; ++i) {
                    const auto s0 = rotr32(w[i-15], 7) ^ rotr32(w[i-15], 18) ^ (w[i-15] >> 3);
                    const auto s1 = rotr32(w[i-2], 17) ^ rotr32(w[i-2], 19) ^ (w[i-2] >> 10);
                    w[i] = w[i-16] + s0 + w[i-7] + s1;
                }
                return w;
            }

            HASHLIB_ALWAYS_INLINE
            static constexpr auto rotr32(std::uint32_t x, int n) noexcept -> std::uint32_t {
                return (x >> n) | (x << (32 - n));
            }

        protected:
            std::array<byte, 64> buffer_{};
            std::size_t buffer_size_ = 0;
            std::uint64_t total_size_ = 0;
            std::array<std::uint32_t, 8> h_;
        };

        struct sha256 : sha224_256_base {
            static constexpr std::size_t digest_size = 32;

            sha256() noexcept : sha224_256_base({
                0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
            }) {}

        };

        struct sha224 : sha224_256_base {
            static constexpr std::size_t digest_size = 28;

            sha224() noexcept : sha224_256_base({
                0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
                0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4
            }) {}
        };
    }

    HASHLIB_MOD_EXPORT using sha224 = context<detail::sha224>;
    HASHLIB_MOD_EXPORT using sha256 = context<detail::sha256>;
}