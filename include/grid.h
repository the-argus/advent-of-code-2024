#ifndef __AOC_GRID_H__
#define __AOC_GRID_H__

#include "vec.h"
#include <cassert>
#include <fmt/core.h>
#include <fstream>
#include <span>
#include <vector>

template <typename T> class Array2D
{
  public:
    static constexpr bool is_char_grid = std::is_same_v<T, char>;
    using element_type = T;
    using indexer_type = Vec<i64>;
    using enumerated_pair = std::pair<element_type&, indexer_type>;
    using column_type = std::span<element_type>;
    using const_column_type = std::span<const element_type>;

    /// Parses the file into a grid, and closes it
    static inline Array2D parse(std::ifstream&& file)
        requires is_char_grid;

    static inline Array2D empty_of_dimensions(Vec<u64> dimensions);

    inline element_type& operator[](indexer_type index);
    inline const element_type& operator[](indexer_type index) const;

    [[nodiscard]] inline u64 width() const noexcept { return m_dimensions.x; }
    [[nodiscard]] inline u64 height() const noexcept { return m_dimensions.y; }
    [[nodiscard]] inline column_type row_at(u64 idx);
    [[nodiscard]] inline const_column_type row_at(u64 idx) const;
    [[nodiscard]] inline bool is_inbounds(indexer_type coord) const;

    inline void debug_print() const
        requires is_char_grid;

    // easier than writing an iterator tbh
    // losing out on <algorithm> should be fine?
    template <typename Callable>
        requires std::is_invocable_r_v<void, Callable, enumerated_pair>
    inline constexpr void for_each(const Callable& func)
    {
        for (u64 y = 0; y < m_dimensions.y; ++y) {
            for (u64 x = 0; x < m_dimensions.x; ++x) {
                const indexer_type i{i64(x), i64(y)};
                func(enumerated_pair{(*this)[i], i});
            }
        }
    }

  private:
    inline void bounds_check(indexer_type index) const;
    [[nodiscard]] inline u64 map(indexer_type index) const;

    inline Array2D(std::vector<T>&& contents, Vec<u64> dimensions)
        : m_contents(std::move(contents)), m_dimensions(dimensions)
    {
    }

    std::vector<T> m_contents;
    Vec<u64> m_dimensions;
};

using Grid = Array2D<char>;

template <typename T>
inline auto Array2D<T>::parse(std::ifstream&& file) -> Array2D
    requires is_char_grid
{
    std::string line;
    Vec<u64> dimensions = {};
    std::vector<char> contents;

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        if (line.length() != dimensions.x && dimensions.x != 0) {
            fmt::println(
                "Bad file input, irregular line lengths, cannot parse to grid");
        }

        dimensions.x = line.length();
        std::copy(line.begin(), line.end(), std::back_inserter(contents));
        dimensions.y += 1;
    }

    assert(contents.size() == dimensions.y * dimensions.x);

    if (contents.empty())
        fmt::println("Warning: parsed empty grid");

    file.close();

    return Array2D(std::move(contents), dimensions);
}

template <typename T>
inline auto Array2D<T>::empty_of_dimensions(Vec<u64> dimensions) -> Array2D
{
    std::vector<T> contents;
    contents.resize(dimensions.x * dimensions.y);
    return Array2D(std::move(contents), dimensions);
}

template <typename T>
inline auto Array2D<T>::operator[](indexer_type index) -> element_type&
{
    bounds_check(index);
    return m_contents.at(map(index));
}

template <typename T>
inline auto Array2D<T>::operator[](indexer_type index) const
    -> const element_type&
{
    bounds_check(index);
    return m_contents.at(map(index));
}

template <typename T> inline u64 Array2D<T>::map(indexer_type index) const
{
    return (index.y * m_dimensions.x) + index.x;
}

template <typename T>
inline void Array2D<T>::bounds_check(indexer_type index) const
{
    if (map(index) >= m_contents.size() || !is_inbounds(index)) {
        fmt::println("out of bounds access to {}x{} grid: ({}, {})",
                     m_dimensions.x, m_dimensions.y, index.x, index.y);
        std::abort();
    }
}

template <typename T> inline auto Array2D<T>::row_at(u64 idx) -> column_type
{
    return {&m_contents.at(idx * m_dimensions.x), m_dimensions.x};
}

template <typename T>
inline auto Array2D<T>::row_at(u64 idx) const -> const_column_type
{
    return {&m_contents.at(idx * m_dimensions.x), m_dimensions.x};
}

template <typename T>
inline bool Array2D<T>::is_inbounds(indexer_type coord) const
{
    return coord.x >= 0 && coord.y >= 0 && coord.x < m_dimensions.x &&
           coord.y < m_dimensions.y;
}

template <typename T>
inline void Array2D<T>::debug_print() const
    requires is_char_grid
{
    for (u64 i = 0; i < m_dimensions.x; ++i) {
        const auto row = row_at(i);
        fmt::println("{}", std::string_view(row.begin(), row.end()));
    }
}

#endif
