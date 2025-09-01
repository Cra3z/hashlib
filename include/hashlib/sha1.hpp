#ifndef HASHLIB_ALL_IN_ONE
#pragma once
#include "core.hpp"
#endif

namespace hashlib {
    namespace detail {
        class sha1 {
        public:
            static constexpr std::size_t digest_size = 20;

        public:
            sha1() noexcept : h_{0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0} {}

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
            auto do_digest() noexcept -> std::array<std::uint32_t, 5> {
                auto_restorer<sha1> _{*this};
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
            auto process_(const std::array<std::uint32_t, 80>& w) noexcept -> void {
                auto a = h_[0], b = h_[1], c = h_[2], d = h_[3], e = h_[4];

                for (std::size_t i = 0; i < 80; ++i) {
                    std::uint32_t f, k;

                    if (i < 20) {
                        f = d ^ (b & (c ^ d));
                        k = 0x5a827999;
                    } else if (i < 40) {
                        f = b ^ c ^ d;
                        k = 0x6ed9eba1;
                    } else if (i < 60) {
                        f = (b & c) | (b & d) | (c & d);
                        k = 0x8f1bbcdc;
                    } else {
                        f = b ^ c ^ d;
                        k = 0xca62c1d6;
                    }

                    auto temp = rotl32(a, 5) + f + e + k + w[i];
                    e = d;
                    d = c;
                    c = rotl32(b, 30);
                    b = a;
                    a = temp;
                }

                h_[0] += a;
                h_[1] += b;
                h_[2] += c;
                h_[3] += d;
                h_[4] += e;
            }

            template<typename RandomAccessIt>
            static auto w_table_(RandomAccessIt it) noexcept -> std::array<std::uint32_t, 80> {
                static_assert(is_random_access_iterator<RandomAccessIt>::value, "unexpected");
                std::array<std::uint32_t, 80> w; // NOLINT(*-pro-type-member-init)
                for (std::size_t i = 0; i < 16; ++i) {
                    w[i] = (std::uint32_t(it[i * 4]) << 24) |
                           (std::uint32_t(it[i * 4 + 1]) << 16) |
                           (std::uint32_t(it[i * 4 + 2]) << 8) |
                           (std::uint32_t(it[i * 4 + 3]));
                }
                for (std::size_t i = 16; i < 80; ++i) {
                    const auto temp = w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16];
                    w[i] = rotl32(temp, 1);
                }

                return w;
            }

            HASHLIB_ALWAYS_INLINE
            static constexpr auto rotl32(std::uint32_t x, int n) noexcept -> std::uint32_t {
                return (x << n) | (x >> (32 - n));
            }

        private:
            std::array<byte, 64> buffer_{};
            std::size_t buffer_size_ = 0;
            std::uint64_t total_size_ = 0;
            std::array<std::uint32_t, 5> h_;
        };
    }

    HASHLIB_MOD_EXPORT using sha1 = context<detail::sha1>;
}
