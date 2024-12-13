#include "aoc.h"
#include "shorthand.h"
#include <array>
#include <cassert>

static constexpr f64 FEPSILON = 0.001;

static bool roughly_equals(f64 a, f64 b) { return std::abs(a - b) < FEPSILON; }

struct vec
{
    f64 x;
    f64 y;
    [[nodiscard]] f64 len() const { return std::sqrt((x * x) + (y * y)); }
    [[nodiscard]] vec normalized() const
    {
        const f64 l = len();
        return {x / l, y / l};
    }
    [[nodiscard]] f64 dot(const vec& other) const
    {
        return x * other.x + y * other.y;
    }
    [[nodiscard]] vec mul(const vec& other) const
    {
        return {x * other.x, y * other.y};
    }
    [[nodiscard]] vec sub(const vec& other) const
    {
        return {x - other.x, y - other.y};
    }
    [[nodiscard]] vec add(const vec& other) const
    {
        return {x + other.x, y + other.y};
    }
    [[nodiscard]] vec scale(f64 s) const { return {x * s, y * s}; }
    [[nodiscard]] bool roughly_equals(const vec& other) const
    {
        return ::roughly_equals(x, other.x) && ::roughly_equals(y, other.y);
    }
};

struct mat
{
    std::array<std::array<f64, 2>, 2> columns = {
        std::array{1.0, 0.0},
        std::array{0.0, 1.0},
    };
    mat(std::array<vec, 2> c) : columns({{c[0].x, c[0].y}, {c[1].x, c[1].y}}) {}

    [[nodiscard]] vec mul(const vec& other) const
    {
        return {
            other.x * columns[0][0] + other.y * columns[1][0],
            other.x * columns[0][1] + other.y * columns[1][1],
        };
    }

    [[nodiscard]] mat inverse() const
    {
        const f64 det_inverse =
            (columns[0][0] * columns[1][1] - columns[0][1] * columns[1][0]);
        assert(det_inverse != 0.0);
        const f64 det = 1.0 / det_inverse;
        return mat({
            vec{columns[1][1], -columns[0][1]}.scale(det),
            vec{-columns[1][0], columns[0][0]}.scale(det),
        });
    }
};

static_assert(sizeof(mat) == 4 * sizeof(f64));

int main(int argc, char* argv[])
{
    auto file = open_stream(argc, argv).value();

    u64 total = 0;
    while (!file.eof()) {
        // resultcache.clear();
        std::string buttona;
        std::string buttonb;
        std::string prize_str;
        std::getline(file, buttona);
        std::getline(file, buttonb);
        std::getline(file, prize_str);
        if (!file.eof())
            file.ignore(1, '\n');

        i32 a_x;
        i32 a_y;
        // NOLINTNEXTLINE
        std::sscanf(buttona.c_str(), "Button A: X+%d, Y+%d", &a_x, &a_y);
        i32 b_x;
        i32 b_y;
        // NOLINTNEXTLINE
        std::sscanf(buttonb.c_str(), "Button B: X+%d, Y+%d", &b_x, &b_y);

        u64 prize_x;
        u64 prize_y;
        // NOLINTNEXTLINE
        std::sscanf(prize_str.c_str(), "Prize: X=%zu, Y=%zu", &prize_x,
                    &prize_y);

        // part 2
        constexpr u64 error = 10000000000000UL;
        prize_x += error;
        prize_y += error;

        // assert not linearly dependent
        {
            const f64 slope_a = f64(a_y) / f64(a_x);
            const f64 slope_b = f64(b_y) / f64(b_x);
            assert(!roughly_equals(slope_a, slope_b));
        }

        const vec prize = {f64(prize_x), f64(prize_y)};
        const vec a_onemove = {f64(a_x), f64(a_y)};
        const vec b_onemove = {f64(b_x), f64(b_y)};

        // transform matrix described by a and b
        const mat space({a_onemove, b_onemove});
        const vec transformed = space.inverse().mul(prize);

        if (!roughly_equals(transformed.x, std::round(transformed.x)))
            continue;

        if (!roughly_equals(transformed.y, std::round(transformed.y)))
            continue;

        total += u64(std::round(transformed.y)) +
                 3UL * u64(std::round(transformed.x));
    }

    fmt::println("result: {}", total);
}
