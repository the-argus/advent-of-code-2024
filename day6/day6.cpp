#include <cassert>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <ranges>
#include <unordered_map>
#include <vector>

enum class TileFlags : uint8_t
{
    // clang-format off
    Air =           0b0000000,
    IsObstacle =    0b0000001,
    IsVisited =     0b0000010,
    North =         0b0000100,
    South =         0b0001000,
    East =          0b0010000,
    West =          0b0100000,
    IsGuard =       0b1000000
    // clang-format on
};

struct Visit
{
    std::pair<int64_t, int64_t> location;
    uint8_t direction;

    bool operator==(const Visit& other) const
    {
        return location.first == other.location.first &&
               location.second == other.location.second &&
               direction == other.direction;
    }
};

struct VisitHasher
{
    std::size_t operator()(Visit const& v) const
    {
        return std::hash<int64_t>()(v.location.first &
                                        (v.location.second << 24) ^
                                    uint8_t(v.direction));
    }
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
        map[guard.second][guard.first] &= uint8_t(TileFlags::IsGuard);

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

    using vec = std::pair<int64_t, int64_t>;

    std::unordered_map<Visit, bool, VisitHasher> visits;
    std::vector<Visit> visit_sequence;

    const auto is_route_looping =
        [&visits, &visit_sequence](vec _guard, vec _guard_direction,
                                   const std::vector<Row>& map) -> bool {
        vec guard = _guard;
        vec guard_direction = _guard_direction;

        while (guard.first + guard_direction.first < map[0].size() &&
               guard.first + guard_direction.first >= 0 &&
               guard.second + guard_direction.second < map.size() &&
               guard.second + guard_direction.second >= 0) {

            uint8_t next = map[guard.second + guard_direction.second]
                              [guard.first + guard_direction.first];

            Visit visit{.location = guard};

            if (guard_direction == std::pair<int64_t, int64_t>{1, 0}) {
                visit.direction = uint8_t(TileFlags::East);
            } else if (guard_direction == std::pair<int64_t, int64_t>{0, 1}) {
                visit.direction = uint8_t(TileFlags::South);
            } else if (guard_direction == std::pair<int64_t, int64_t>{-1, 0}) {
                visit.direction = uint8_t(TileFlags::West);
            } else if (guard_direction == std::pair<int64_t, int64_t>{0, -1}) {
                visit.direction = uint8_t(TileFlags::North);
            } else {
                fmt::println("wut");
                std::abort();
            }

            visit_sequence.push_back(visit);

            if (visits.contains(visit)) {
                // we have already been here and will continue to loop
                return true;
            }
            visits.insert({visit, true});

            if (!(next & uint8_t(TileFlags::IsObstacle))) {
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
        return false;
    };

    size_t loops = 0;

    for (size_t y = 0; y < map.size(); ++y) {
        for (size_t x = 0; x < map[0].size(); ++x) {

            uint8_t& tile = map[y][x];

            if (tile & uint8_t(TileFlags::IsObstacle) ||
                tile & uint8_t(TileFlags::IsGuard))
                continue;

            auto path_point =
                std::find_if(visit_sequence.begin(), visit_sequence.end(),
                             [x, y](const Visit& visit) -> bool {
                                 return visit.location.first == x &&
                                        visit.location.second == y;
                             });

            // okay, we are placing the tile in a place that gets traversed,
            // rewind to before that point
            if (path_point != visit_sequence.end()) {
                auto invalidated =
                    std::ranges::subrange(path_point, visit_sequence.end());
                for (const Visit& v : invalidated)
                    visits.erase(v);
                visit_sequence.erase(invalidated.begin(), invalidated.end());
            }

            uint8_t copy = tile;
            tile &= uint8_t(TileFlags::IsObstacle);
            const auto location =
                visit_sequence.empty() ? guard : visit_sequence.back().location;
            auto dir = guard_direction;
            if (!visit_sequence.empty()) {
                switch (visit_sequence.back().direction) {
                case uint8_t(TileFlags::North):
                    dir = {0, -1};
                    break;
                case uint8_t(TileFlags::South):
                    dir = {0, 1};
                    break;
                case uint8_t(TileFlags::East):
                    dir = {1, 0};
                    break;
                case uint8_t(TileFlags::West):
                    dir = {-1, 0};
                    break;
                default:
                    fmt::println("i dont understand why");
                    std::abort();
                }
            }
            bool looping = is_route_looping(location, dir, map);
            fmt::println("looping: {}", looping);
            loops += looping;
            tile = copy;
        }
    }

    fmt::println("looping paths: {}", loops);

    return 0;
}
