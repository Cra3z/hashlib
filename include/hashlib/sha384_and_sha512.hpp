#ifndef HASHLIB_ALL_IN_ONE
#pragma once
#include "core.hpp"
#endif

namespace hashlib {
    namespace detail {
        class sha384_512_base {
        protected:
            sha384_512_base(const std::array<std::uint64_t, 8>& init_state) noexcept : h_(init_state) {}

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
                byte temp[256];
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
                    std::size_t to_copy = std::min(bytes_count, 128 - buffer_size_);
                    std::copy_n(first, to_copy, buffer_.data() + buffer_size_);
                    buffer_size_ += to_copy;
                    i += to_copy;
                    if (buffer_size_ == 128) {
                        process_block_(buffer_.data());
                        buffer_size_ = 0;
                    }
                }

                for (; i + 127 < bytes_count; i += 128) {
                    process_block_(reinterpret_cast<const byte*>(&first[i]));
                }

                if (i < bytes_count) {
                    std::copy_n(std::next(first, i), bytes_count - i, buffer_.data());
                    buffer_size_ = bytes_count - i;
                }
            }

        protected:
            auto do_digest() noexcept -> std::array<std::uint64_t, 8> {
                auto_restorer<sha384_512_base> _{*this};
                auto buffer_size = buffer_size_;
                auto total_size = total_size_;
                byte padding[256]{};
                padding[0] = 0x80;
                std::size_t padding_size = (buffer_size < 112) ? (112 - buffer_size) : (240 - buffer_size);
                update({padding, padding_size});

                std::uint64_t bits_high = (total_size >> 61);
                std::uint64_t bits_low = (total_size << 3);
                for (std::size_t i = 0; i < 8; ++i) {
                    padding[7 - i] = bits_high & 0xff;
                    bits_high >>= 8;
                }
                for (std::size_t i = 0; i < 8; ++i) {
                    padding[15 - i] = bits_low & 0xff;
                    bits_low >>= 8;
                }
                update({padding, 16});
                return h_;
            }

            HASHLIB_ALWAYS_INLINE
            static auto unit_to_bytes(std::uint64_t unit) noexcept -> std::array<byte, 8> {
                byte* byte_ptr = reinterpret_cast<byte*>(&unit);
                if HASHLIB_CXX17_CONSTEXPR (is_little_endian()) {
                    return {
                        byte_ptr[7], byte_ptr[6], byte_ptr[5], byte_ptr[4],
                        byte_ptr[3], byte_ptr[2], byte_ptr[1], byte_ptr[0]
                    };
                }
                else {
                    return {
                        byte_ptr[0], byte_ptr[1], byte_ptr[2], byte_ptr[3],
                        byte_ptr[4], byte_ptr[5], byte_ptr[6], byte_ptr[7],
                    };
                }
            }

        private:
            void process_block_(const byte* block) noexcept {
                static constexpr std::uint64_t k[80] = {
                    0x428a2f98d728ae22ull, 0x7137449123ef65cdull, 0xb5c0fbcfec4d3b2full, 0xe9b5dba58189dbbcull,
                    0x3956c25bf348b538ull, 0x59f111f1b605d019ull, 0x923f82a4af194f9bull, 0xab1c5ed5da6d8118ull,
                    0xd807aa98a3030242ull, 0x12835b0145706fbeull, 0x243185be4ee4b28cull, 0x550c7dc3d5ffb4e2ull,
                    0x72be5d74f27b896full, 0x80deb1fe3b1696b1ull, 0x9bdc06a725c71235ull, 0xc19bf174cf692694ull,
                    0xe49b69c19ef14ad2ull, 0xefbe4786384f25e3ull, 0x0fc19dc68b8cd5b5ull, 0x240ca1cc77ac9c65ull,
                    0x2de92c6f592b0275ull, 0x4a7484aa6ea6e483ull, 0x5cb0a9dcbd41fbd4ull, 0x76f988da831153b5ull,
                    0x983e5152ee66dfabull, 0xa831c66d2db43210ull, 0xb00327c898fb213full, 0xbf597fc7beef0ee4ull,
                    0xc6e00bf33da88fc2ull, 0xd5a79147930aa725ull, 0x06ca6351e003826full, 0x142929670a0e6e70ull,
                    0x27b70a8546d22ffcull, 0x2e1b21385c26c926ull, 0x4d2c6dfc5ac42aedull, 0x53380d139d95b3dfull,
                    0x650a73548baf63deull, 0x766a0abb3c77b2a8ull, 0x81c2c92e47edaee6ull, 0x92722c851482353bull,
                    0xa2bfe8a14cf10364ull, 0xa81a664bbc423001ull, 0xc24b8b70d0f89791ull, 0xc76c51a30654be30ull,
                    0xd192e819d6ef5218ull, 0xd69906245565a910ull, 0xf40e35855771202aull, 0x106aa07032bbd1b8ull,
                    0x19a4c116b8d2d0c8ull, 0x1e376c085141ab53ull, 0x2748774cdf8eeb99ull, 0x34b0bcb5e19b48a8ull,
                    0x391c0cb3c5c95a63ull, 0x4ed8aa4ae3418acbull, 0x5b9cca4f7763e373ull, 0x682e6ff3d6b2b8a3ull,
                    0x748f82ee5defb2fcull, 0x78a5636f43172f60ull, 0x84c87814a1f0ab72ull, 0x8cc702081a6439ecull,
                    0x90befffa23631e28ull, 0xa4506cebde82bde9ull, 0xbef9a3f7b2c67915ull, 0xc67178f2e372532bull,
                    0xca273eceea26619cull, 0xd186b8c721c0c207ull, 0xeada7dd6cde0eb1eull, 0xf57d4f7fee6ed178ull,
                    0x06f067aa72176fbaull, 0x0a637dc5a2c898a6ull, 0x113f9804bef90daeull, 0x1b710b35131c471bull,
                    0x28db77f523047d84ull, 0x32caab7b40c72493ull, 0x3c9ebe0a15c9bebcull, 0x431d67c49c100d4cull,
                    0x4cc5d4becb3e42b6ull, 0x597f299cfc657e2aull, 0x5fcb6fab3ad6faecull, 0x6c44198c4a475817ull
                };

                std::uint64_t w[80];

                for (std::size_t i = 0; i < 16; ++i) {
                    w[i] = (std::uint64_t(block[i * 8]) << 56) |
                           (std::uint64_t(block[i * 8 + 1]) << 48) |
                           (std::uint64_t(block[i * 8 + 2]) << 40) |
                           (std::uint64_t(block[i * 8 + 3]) << 32) |
                           (std::uint64_t(block[i * 8 + 4]) << 24) |
                           (std::uint64_t(block[i * 8 + 5]) << 16) |
                           (std::uint64_t(block[i * 8 + 6]) << 8) |
                           (std::uint64_t(block[i * 8 + 7]));
                }

                for (std::size_t i = 16; i < 80; ++i) {
                    const auto s0 = rotr64(w[i-15], 1) ^ rotr64(w[i-15], 8) ^ (w[i-15] >> 7);
                    const auto s1 = rotr64(w[i-2], 19) ^ rotr64(w[i-2], 61) ^ (w[i-2] >> 6);
                    w[i] = w[i-16] + s0 + w[i-7] + s1;
                }

                auto a = h_[0], b = h_[1], c = h_[2], d = h_[3],
                     e = h_[4], f = h_[5], g = h_[6], h = h_[7];

                for (std::size_t i = 0; i < 80; ++i) {
                    const auto S1 = rotr64(e, 14) ^ rotr64(e, 18) ^ rotr64(e, 41);
                    const auto ch = (e & f) ^ (~e & g);
                    const auto temp1 = h + S1 + ch + k[i] + w[i];
                    const auto S0 = rotr64(a, 28) ^ rotr64(a, 34) ^ rotr64(a, 39);
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

            HASHLIB_ALWAYS_INLINE
            static constexpr std::uint64_t rotr64(std::uint64_t x, int n) noexcept {
                return (x >> n) | (x << (64 - n));
            }

        protected:
            std::array<byte, 128> buffer_{};
            std::size_t buffer_size_ = 0;
            std::uint64_t total_size_ = 0;
            std::array<std::uint64_t, 8> h_;
        };

        struct sha512 : sha384_512_base {
            static constexpr std::size_t digest_size = 64;

            sha512() noexcept : sha384_512_base({
                0x6a09e667f3bcc908ull, 0xbb67ae8584caa73bull,
                0x3c6ef372fe94f82bull, 0xa54ff53a5f1d36f1ull,
                0x510e527fade682d1ull, 0x9b05688c2b3e6c1full,
                0x1f83d9abfb41bd6bull, 0x5be0cd19137e2179ull
            }) {}
        };

        struct sha384 : sha384_512_base {
            static constexpr std::size_t digest_size = 48;

            sha384() noexcept : sha384_512_base({
                0xcbbb9d5dc1059ed8ull, 0x629a292a367cd507ull,
                0x9159015a3070dd17ull, 0x152fecd8f70e5939ull,
                0x67332667ffc00b31ull, 0x8eb44a8768581511ull,
                0xdb0c2e0d64f98fa7ull, 0x47b5481dbefa4fa4ull
            }) {}
        };
    }

    HASHLIB_MOD_EXPORT using sha384 = context<detail::sha384>;
    HASHLIB_MOD_EXPORT using sha512 = context<detail::sha512>;
}