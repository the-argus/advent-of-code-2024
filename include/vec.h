#ifndef __AOC_VEC_H__
#define __AOC_VEC_H__

#include "shorthand.h"
#include <cmath>
#include <functional>
#include <type_traits>

template <typename T>
    requires std::is_arithmetic_v<T>
struct Vec
{
    T x;
    T y;

    static constexpr inline Vec splat(T x) { return {x, x}; }

    template <typename U>
    static inline constexpr Vec convert(const Vec<U>& other)
        requires(std::is_integral_v<T> && std::is_integral_v<U>) ||
                (std::is_floating_point_v<T> && std::is_floating_point_v<U>)
    {
        return {T(other.x), T(other.y)};
    }

    template <typename U>
    static inline constexpr Vec round(const Vec<U>& other)
        requires std::is_integral_v<T> && std::is_signed_v<T> &&
                 std::is_floating_point_v<U>
    {
        return {T(std::round(other.x)), T(std::round(other.y))};
    }

    inline Vec operator++(int) = delete;
    inline Vec& operator++() = delete;
    inline Vec operator--(int) = delete;
    inline Vec& operator--() = delete;

    [[nodiscard]] f64 len() const
        requires std::is_floating_point_v<T>
    {
        return std::sqrt((x * x) + (y * y));
    }
    [[nodiscard]] Vec normalized() const
        requires std::is_floating_point_v<T>
    {
        const T l = len();
        return {x / l, y / l};
    }

    constexpr inline Vec& operator+=(const Vec& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    constexpr inline friend Vec operator+(Vec lhs, const Vec& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    constexpr inline Vec& operator-=(const Vec& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    constexpr inline friend Vec operator-(Vec lhs, const Vec& rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    constexpr inline Vec& operator*=(const Vec& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    constexpr inline friend Vec operator*(Vec lhs, const Vec& rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    constexpr inline Vec& operator/=(const Vec& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }

    constexpr inline friend Vec operator/(Vec lhs, const Vec& rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    constexpr inline Vec& operator%=(const Vec& rhs)
    {
        if constexpr (std::is_integral_v<T>) {
            x %= rhs.x;
            y %= rhs.y;
        } else if (std::is_same_v<T, f32>) {
            x = fmodf32(x, rhs.x);
            y = fmodf32(y, rhs.y);
        } else if (std::is_same_v<T, f64>) {
            x = fmodf64(x, rhs.x);
            y = fmodf64(y, rhs.y);
        } else {
            static_assert(false, "unable to instantiate modulus operator");
        }
        return *this;
    }

    constexpr inline friend Vec operator%(Vec lhs, const Vec& rhs)
    {
        lhs %= rhs;
        return lhs;
    }

    constexpr inline bool operator==(const Vec& other) const
    {
        return x == other.x && y == other.y;
    }

    struct hash
    {
        constexpr std::size_t operator()(Vec const& v) const
        {
            return std::hash<T>()(v.x ^ (v.y << 32));
        }
    };
};

using vec = Vec<i64>;
using vecf = Vec<f64>;

#endif
