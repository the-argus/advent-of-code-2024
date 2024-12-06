#include <cassert>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <ranges>
#include <vector>

enum class TileFlags : uint8_t
{
    Air = 0b0000,
    IsObstacle = 0b0001,
    IsVisited = 0b0010,
};

int main(int argc, char* argv[])
{
    if (argc < 2) {
        fmt::println("give file pls");
        return -1;
    }

    auto* name = argv[1];

    if (!std::filesystem::exists(name)) {
        fmt::println("Doesn't exist lol");
        return -1;
    }

    std::ifstream file;
    file.open(name);
    if (!file.is_open()) {
        fmt::println("bad open");
        return -1;
    }

    std::string line;
    using Row = std::vector<uint8_t>;
    std::vector<Row> map;
    std::pair<int64_t, int64_t> guard = {std::numeric_limits<size_t>::max(),
                                         std::numeric_limits<size_t>::max()};
    std::pair<int64_t, int64_t> guard_direction;

    while (std::getline(file, line)) {
        constexpr auto totile = [](char c) {
            return c == '#' ? uint8_t(TileFlags::IsObstacle)
                            : uint8_t(TileFlags::Air);
        };
        auto iter = line | std::views::transform(totile);
        map.push_back(Row(iter.begin(), iter.end()));

        auto idx = std::find_if(line.begin(), line.end(), [](char c) -> bool {
            return c == '>' || c == '<' || c == 'v' || c == '^';
        });

        if (idx == line.end())
            continue;

        assert(guard.first == std::numeric_limits<size_t>::max());

        guard = {idx - line.begin(), map.size() - 1};

        switch (*idx) {
        case '>':
            guard_direction = {1, 0};
            break;
        case '<':
            guard_direction = {-1, 0};
            break;
        case '^':
            guard_direction = {0, -1};
            break;
        case 'v':
            guard_direction = {0, 1};
            break;
        default:
            std::abort();
        }
    }

    // trace guards path through the area
    while (guard.first + guard_direction.first < map[0].size() &&
           guard.first + guard_direction.first >= 0 &&
           guard.second + guard_direction.second < map.size() &&
           guard.second + guard_direction.second >= 0) {

        uint8_t next = map[guard.second + guard_direction.second]
                          [guard.first + guard_direction.first];

        if (!(next & uint8_t(TileFlags::IsObstacle))) {
            uint8_t& current = map[guard.second][guard.first];
            current |= uint8_t(TileFlags::IsVisited);

            guard.first += guard_direction.first;
            guard.second += guard_direction.second;
            continue;
        }

        if (guard_direction == std::pair<int64_t, int64_t>{1, 0}) {
            guard_direction = {0, 1};
        } else if (guard_direction == std::pair<int64_t, int64_t>{0, 1}) {
            guard_direction = {-1, 0};
        } else if (guard_direction == std::pair<int64_t, int64_t>{-1, 0}) {
            guard_direction = {0, -1};
        } else if (guard_direction == std::pair<int64_t, int64_t>{0, -1}) {
            guard_direction = {1, 0};
        } else {
            fmt::println("failure");
            std::abort();
        }
    }
    // mark the last square we walk off as visited
    map[guard.second][guard.first] |= uint8_t(TileFlags::IsVisited);

    size_t total = 0;
    for (const auto& row : map)
        for (uint8_t tile : row)
            total += (tile & uint8_t(TileFlags::IsVisited)) != 0;

    fmt::println("visited tiles: {}", total);

    return 0;
}
