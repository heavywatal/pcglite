// pcglite: a lightweight subset of pcg-cpp.
//
// Copyright 2024 Watal M. Iwasaki
// SPDX-License-Identifier: MIT

/*
 * PCG Random Number Generation for C++
 *
 * Copyright 2014-2022 Melissa O'Neill <oneill@pcg-random.org>,
 *                     and the PCG Project contributors.
 *
 * SPDX-License-Identifier: (Apache-2.0 OR MIT)
 *
 * Licensed under the Apache License, Version 2.0 (provided in
 * LICENSE-APACHE.txt and at http://www.apache.org/licenses/LICENSE-2.0)
 * or under the MIT license (provided in LICENSE-MIT.txt and at
 * http://opensource.org/licenses/MIT), at your option. This file may not
 * be copied, modified, or distributed except according to those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 * For additional information about the PCG random number generation scheme,
 * visit http://www.pcg-random.org/.
 */

#pragma once
#ifndef PCGLITE_PCGLITE_HPP_
#define PCGLITE_PCGLITE_HPP_

#include <charconv>
#include <cstdint>
#include <cstring>
#include <ios>
#include <limits>
#include <type_traits>

namespace pcglite {

namespace detail {

template <class T> inline
constexpr T unsigned_rotr(const T x, const unsigned s) noexcept {
    constexpr unsigned dig = std::numeric_limits<T>::digits;
    return (x >> s) | (x << ((-s) % dig));
}

constexpr unsigned floor_log2(unsigned x) noexcept {
    return x == 1 ? 0 : 1 + floor_log2(x >> 1);
}

constexpr __uint128_t constexpr_uint128(uint64_t high, uint64_t low) noexcept {
    return (static_cast<__uint128_t>(high) << 64u) + low;
}

template <class T> inline constexpr T default_multiplier;
template <class T> inline constexpr T default_increment;
template <class T> inline constexpr T default_state;

template <> inline constexpr uint64_t default_multiplier<uint64_t>
  = 6364136223846793005ULL;
template <> inline constexpr uint64_t default_increment<uint64_t>
  = 1442695040888963407ULL;
template <> inline constexpr uint64_t default_state<uint64_t>
  = 0x4d595df4d0f33173ULL;

template <> inline constexpr __uint128_t default_multiplier<__uint128_t>
  = constexpr_uint128(2549297995355413924ULL, 4865540595714422341ULL);
template <> inline constexpr __uint128_t default_increment<__uint128_t>
  = constexpr_uint128(6364136223846793005ULL, 1442695040888963407ULL);
template <> inline constexpr __uint128_t default_state<__uint128_t>
  = constexpr_uint128(0xb8dc10e158a92392ULL, 0x98046df007ec0a53ULL);

template <class state_type, int N = 2>
union SeedSeqData {
    state_type as_state_type[N];
    uint32_t as_uint32_t[N * sizeof(state_type) / sizeof(uint32_t)];
};

} // namespace detail


template <class UIntType>
class permuted_congruential_engine {
  public:
    using result_type = UIntType;
    using state_type = std::conditional_t<std::is_same_v<UIntType, uint32_t>,
                                          uint64_t, __uint128_t>;

    static constexpr result_type min() {return 0u;}
    static constexpr result_type max() {return std::numeric_limits<result_type>::max();}
    static constexpr state_type default_seed = 0xcafef00dd15ea5e5ULL;
    static constexpr state_type multiplier = detail::default_multiplier<state_type>;

    // constructors
    permuted_congruential_engine() = default;
    permuted_congruential_engine(const permuted_congruential_engine&) = default;
    permuted_congruential_engine(permuted_congruential_engine&&) = default;
    explicit permuted_congruential_engine(state_type s) noexcept {seed(s);}
    explicit permuted_congruential_engine(state_type s, state_type inc) noexcept {seed(s, inc);}
    template <class SeedSeq, std::enable_if_t<!std::is_convertible_v<SeedSeq, state_type>>>
    explicit permuted_congruential_engine(SeedSeq& q) {seed(q);}

    void seed(state_type s) noexcept {
        state_ = s;
        state_ += increment_;
        bump();
    }
    void seed(state_type s, state_type inc) noexcept {
        increment_ = (inc << 1u) | 1u;
        seed(s);
    }
    template <class SeedSeq>
    std::enable_if_t<!std::is_convertible_v<SeedSeq, state_type>>
    seed(SeedSeq& q) {
        detail::SeedSeqData<state_type> data{};
        constexpr int len = sizeof(data.as_uint32_t) / sizeof(uint32_t);
        q.generate(data.as_uint32_t, data.as_uint32_t + len);
        seed(data.as_state_type[1], data.as_state_type[0]);
    }

    result_type operator()() noexcept {
        if constexpr (sizeof(state_type) <= 8) {
            auto res = output();
            bump();
            return res;
        } else {
            bump();
            return output();
        }
    }

    void discard(unsigned long long n) noexcept {
        state_ = advance(n);
    }

  private:
    state_type increment_ = detail::default_increment<state_type>;
    state_type state_ = detail::default_state<state_type>;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

  public: // for pcg-test.cpp
    result_type operator()(result_type upper_bound) noexcept {
        const result_type threshold = (max() - min() - upper_bound + 1u) % upper_bound;
        while (true) {
            result_type r = (*this)() - min();
            if (r >= threshold) return r % upper_bound;
        }
    }

    void backstep(state_type delta) noexcept {
        discard(-delta);
    }

    state_type operator-(const permuted_congruential_engine& old) const noexcept {
        return distance(old.state_);
    }

    static constexpr unsigned period_pow2() {return std::numeric_limits<state_type>::digits;}
    static constexpr unsigned streams_pow2() {return period_pow2() - 1u;}

  private:
    result_type output() const noexcept {
        constexpr unsigned st_digits = std::numeric_limits<state_type>::digits;
        constexpr unsigned res_digits = std::numeric_limits<result_type>::digits;
        constexpr unsigned spare_digits = st_digits - res_digits;
        constexpr unsigned op_digits = detail::floor_log2(res_digits);
        constexpr unsigned bottom_spare = sizeof(state_type) <= 8 ? spare_digits - op_digits : 0u;
        constexpr unsigned xshift = (spare_digits + res_digits - bottom_spare) / 2u;
        constexpr unsigned rshift = st_digits - op_digits;
        state_type internal = state_;
        internal ^= (internal >> xshift);
        result_type result = internal >> bottom_spare;
        const unsigned rot = state_ >> rshift;
        return detail::unsigned_rotr(result, rot);
    }

    state_type advance(state_type delta) const noexcept {
        state_type cur_mult = multiplier;
        state_type cur_plus = increment_;
        state_type acc_mult = 1u;
        state_type acc_plus = 0u;
        while (delta > 0u) {
            if (delta & 1u) {
                acc_mult *= cur_mult;
                acc_plus *= cur_mult;
                acc_plus += cur_plus;
            }
            cur_plus *= (cur_mult + 1u);
            cur_mult *= cur_mult;
            delta >>= 1u;
        }
        return acc_mult * state_ + acc_plus;
    }

    state_type distance(state_type cur_state) const noexcept {
        state_type cur_mult = multiplier;
        state_type cur_plus = increment_;
        state_type the_bit = 1u;
        state_type delta = 0u;
        while (state_ != cur_state) {
            if ((state_ & the_bit) != (cur_state & the_bit)) {
                cur_state *= cur_mult;
                cur_state += cur_plus;
                delta |= the_bit;
            }
            cur_plus *= (cur_mult + 1u);
            cur_mult *= cur_mult;
            the_bit <<= 1u;
        }
        return delta;
    }

    void bump() noexcept {
        state_ *= multiplier;
        state_ += increment_;
    }

    template <class T>
    friend bool
    operator==(const permuted_congruential_engine<T>&, const permuted_congruential_engine<T>&) noexcept;

    template <class T>
    friend bool
    operator!=(const permuted_congruential_engine<T>&, const permuted_congruential_engine<T>&) noexcept;

    template <class CharT, class Traits, class T>
    friend std::basic_ostream<CharT, Traits>&
    operator<<(std::basic_ostream<CharT, Traits>&, const permuted_congruential_engine<T>&);

    template <class CharT, class Traits, class T>
    friend std::basic_istream<CharT, Traits>&
    operator>>(std::basic_istream<CharT, Traits>&, permuted_congruential_engine<T>&);
};

template <class T> bool
operator==(const permuted_congruential_engine<T>& x, const permuted_congruential_engine<T>& y) noexcept {
    return (x.state_ == y.state_) && (x.increment_ == y.increment_);
}

template <class T> bool
operator!=(const permuted_congruential_engine<T>& x, const permuted_congruential_engine<T>& y) noexcept {
    return !(x == y);
}

template <class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& ost, __uint128_t x) {
    auto fillch = ost.fill('0');
    auto flags = ost.flags(std::ios_base::hex);
    uint64_t high(x >> 64u);
    if (high) {ost << high;}
    ost.width(16);
    ost << static_cast<uint64_t>(x);
    ost.fill(fillch);
    ost.flags(flags);
    return ost;
}

template <class CharT, class Traits>
std::basic_istream<CharT, Traits>&
operator>>(std::basic_istream<CharT, Traits>& ist, __uint128_t& x) {
    uint64_t high{}, low{};
    char buffer[33];
    ist.getline(buffer, 33, ' ');
    const auto size = std::strlen(buffer); // ist.gcount() includes trailing \0
    const auto end = buffer + size;
    const auto begin_low = (size > 16) ? (end - 16) : buffer;
    std::from_chars(buffer, begin_low, high, 16);
    std::from_chars(begin_low, end, low, 16);
    x = detail::constexpr_uint128(high, low);
    return ist;
}

template <class CharT, class Traits, class T>
inline std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& ost, const permuted_congruential_engine<T>& x) {
    auto fillch = ost.fill();
    auto flags = ost.flags(std::ios_base::dec | std::ios_base::left);
    ost << x.multiplier << ' '
        << x.increment_ << ' '
        << x.state_;
    ost.fill(fillch);
    ost.flags(flags);
    return ost;
}

template <class CharT, class Traits, class T>
std::basic_istream<CharT, Traits>&
operator>>(std::basic_istream<CharT, Traits>& ist, permuted_congruential_engine<T>& x) {
    auto flags = ist.flags(std::ios_base::dec | std::ios_base::skipws);
    typename permuted_congruential_engine<T>::state_type multiplier{}, increment{}, state{};
    ist >> multiplier >> increment >> state;
    if (!ist.fail()) {
        if (multiplier != x.multiplier) ist.clear(std::ios_base::failbit);
        x.increment_ = increment;
        x.state_ = state;
    }
    ist.flags(flags);
    return ist;
}

using pcg32 = permuted_congruential_engine<uint32_t>;
using pcg64 = permuted_congruential_engine<uint64_t>;

} // namespace pcglite

#endif // PCGLITE_PCGLITE_HPP_
