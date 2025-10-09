#ifndef HASHLIB_ALL_IN_ONE
#pragma once
#include "core.hpp"
#endif

namespace hashlib {
    namespace detail {
        template<std::size_t Bits>
        class sha3 {
            static_assert(
                Bits == 224 || Bits == 256 ||
                Bits == 384 || Bits == 512,
                "unexpected"
            );
        public:
            static constexpr std::size_t digest_size = Bits / 8;

        public:
            sha3() = default;

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
                    this->update({temp, n});
                }
            }

            template<typename RandomAccessIt, typename Sentinel, enable_if_t<
                is_random_access_iterator<RandomAccessIt>::value &&
                is_sentinel_for<Sentinel, RandomAccessIt>::value &&
                is_byte_like<iter_value_t<RandomAccessIt>>::value
            >* = nullptr>
            auto update(RandomAccessIt first, Sentinel last) -> void {
                std::size_t bytes_count = last - first;
                std::size_t i = 0;

                if (buffer_size_ > 0) {
                    std::size_t to_copy = std::min(bytes_count, block_size - buffer_size_);
                    std::copy_n(first, to_copy, buffer_.begin() + buffer_size_);
                    buffer_size_ += to_copy;
                    i += to_copy;

                    if (buffer_size_ == block_size) {
                        absorb_block_();
                        buffer_size_ = 0;
                    }
                }

                for (; i + block_size <= bytes_count; i += block_size) {
                    std::copy_n(first + i, block_size, buffer_.begin());
                    absorb_block_();
                }

                if (i < bytes_count) {
                    std::copy_n(first + i, bytes_count - i, buffer_.begin());
                    buffer_size_ = bytes_count - i;
                }
            }

            auto do_digest() noexcept -> std::array<byte, digest_size> {
                auto_restorer<sha3> _{*this};
                byte padding[block_size]{};
                if (buffer_size_ == block_size - 1) {
                    padding[0] = 0x86;  // 0x06 | 0x80
                    this->update({padding, 1});
                }
                else {
                    padding[0] = 0x06;
                    if (buffer_size_ == 0) {
                        padding[0] = 0x06;
                        padding[block_size - 1] = 0x80;
                        this->update({padding, block_size});
                    }
                    else {
                        std::size_t padding_size = block_size - buffer_size_;
                        padding[padding_size - 1] = 0x80;
                        this->update({padding, padding_size});
                    }
                }

                return this->squeeze_();
            }

            HASHLIB_ALWAYS_INLINE static auto unit_to_bytes(byte unit) noexcept -> std::array<byte, 1> {
                return {unit};
            }

        private:
            auto absorb_block_() noexcept -> void {
                for (std::size_t i = 0; i < block_size / 8; ++i) {
                    std::uint64_t word = 0;
                    for (std::size_t j = 0; j < 8; ++j) {
                        word |= std::uint64_t(buffer_[i * 8 + j]) << (8 * j);
                    }
                    state_[i] ^= word;
                }

                keccak_f_();
            }

            auto keccak_f_() noexcept -> void {
                static constexpr std::uint64_t RC[24]{
                    0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
                    0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
                    0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
                    0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
                    0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
                    0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
                    0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
                    0x8000000000008080, 0x0000000080000001, 0x8000000080008008
                };

                static constexpr int ROTC[24]{
                    1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 2, 14,
                    27, 41, 56, 8, 25, 43, 62, 18, 39, 61, 20, 44
                };

                static constexpr int PILN[24]{
                    10, 7, 11, 17, 18, 3, 5, 16, 8, 21, 24, 4,
                    15, 23, 19, 13, 12, 2, 20, 14, 22, 9, 6, 1
                };

                for (auto round : RC) {
                    // θ
                    std::uint64_t C[5], D[5];
                    for (std::size_t i = 0; i < 5; ++i) {
                        C[i] = state_[i] ^ state_[i + 5] ^ state_[i + 10] ^ state_[i + 15] ^ state_[i + 20];
                    }
                    for (std::size_t i = 0; i < 5; ++i) {
                        D[i] = C[(i + 4) % 5] ^ rotl64(C[(i + 1) % 5], 1);
                    }
                    for (std::size_t i = 0; i < 5; ++i) {
                        for (std::size_t j = 0; j < 5; ++j) {
                            state_[i + j * 5] ^= D[i];
                        }
                    }

                    // ρ and π
                    std::uint64_t temp = state_[1];
                    for (std::size_t i = 0; i < 24; ++i) {
                        const auto j = PILN[i];
                        std::uint64_t t = state_[j];
                        state_[j] = rotl64(temp, ROTC[i]);
                        temp = t;
                    }

                    // χ
                    for (std::size_t j = 0; j < 5; ++j) {
                        const auto base = j * 5;
                        const auto t0 = state_[base];
                        const auto t1 = state_[base + 1];
                        const auto t2 = state_[base + 2];
                        const auto t3 = state_[base + 3];
                        const auto t4 = state_[base + 4];

                        state_[base]     ^= (~t1) & t2;
                        state_[base + 1] ^= (~t2) & t3;
                        state_[base + 2] ^= (~t3) & t4;
                        state_[base + 3] ^= (~t4) & t0;
                        state_[base + 4] ^= (~t0) & t1;
                    }

                    // ι
                    state_[0] ^= round;
                }
            }

            auto squeeze_() noexcept -> std::array<byte, digest_size> {
                std::array<byte, digest_size> result;
                for (std::size_t i = 0; i < digest_size; ++i) {
                    std::size_t word_index = i / 8;
                    std::size_t byte_index = i % 8;
                    result[i] = static_cast<byte>(state_[word_index] >> (8 * byte_index));
                }
                return result;
            }

            HASHLIB_ALWAYS_INLINE
            static constexpr auto rotl64(std::uint64_t x, int n) noexcept -> std::uint64_t {
                return (x << n) | (x >> (64 - n));
            }

        private:
            static constexpr std::size_t rate_size = 1600 - Bits * 2;
            static constexpr std::size_t block_size = rate_size / 8;
            std::array<std::uint64_t, 25> state_{};
            std::array<byte, block_size> buffer_{};
            std::size_t buffer_size_ = 0;
        };
    }

    HASHLIB_MOD_EXPORT using sha3_224 = context<detail::sha3<224>>;
    HASHLIB_MOD_EXPORT using sha3_256 = context<detail::sha3<256>>;
    HASHLIB_MOD_EXPORT using sha3_384 = context<detail::sha3<384>>;
    HASHLIB_MOD_EXPORT using sha3_512 = context<detail::sha3<512>>;
}