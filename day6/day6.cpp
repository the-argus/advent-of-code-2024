#include <cassert>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <ranges>
#include <thread>
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

using vec = std::pair<int64_t, int64_t>;
vec tile_flags_to_direction(uint8_t flags)
{
    switch (flags) {
    case uint8_t(TileFlags::North):
        return {0, -1};
    case uint8_t(TileFlags::South):
        return {0, 1};
    case uint8_t(TileFlags::East):
        return {1, 0};
    case uint8_t(TileFlags::West):
        return {-1, 0};
    default:
        fmt::println("i dont understand why");
        std::abort();
    }
}

std::pair<int64_t, int64_t> tile_flags_to_direction(TileFlags flags)
{
    return tile_flags_to_direction(uint8_t(flags));
}

TileFlags direction_to_tile_flags(std::pair<int64_t, int64_t> direction)
{
    if (direction == vec{1, 0}) {
        return TileFlags::East;
    } else if (direction == vec{0, 1}) {
        return TileFlags::South;
    } else if (direction == vec{-1, 0}) {
        return TileFlags::West;
    } else if (direction == vec{0, -1}) {
        return TileFlags::North;
    } else {
        fmt::println("wut");
        std::abort();
    }
}

struct Visit
{
    vec location;
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

using VisitMap = std::unordered_map<Visit, bool, VisitHasher>;
using Row = std::vector<uint8_t>;

struct ThreadContext
{
    const std::vector<Row>& map;
    const VisitMap& default_visits;
    const std::vector<Visit>& default_visit_sequence;
    vec row_range;
    std::atomic<size_t>* counter;
};

bool is_route_looping_cached(VisitMap& visits,
                             std::vector<Visit>& visit_sequence,
                             const std::vector<Row>& map, vec _guard,
                             vec _guard_direction)
{
    const size_t width = map[0].size();
    const size_t height = map.size();

    vec guard = _guard;
    vec guard_direction = _guard_direction;

    while (guard.first + guard_direction.first < width &&
           guard.first + guard_direction.first >= 0 &&
           guard.second + guard_direction.second < height &&
           guard.second + guard_direction.second >= 0) {

        uint8_t next = map[guard.second + guard_direction.second]
                          [guard.first + guard_direction.first];

        Visit visit{
            .location = guard,
            .direction = uint8_t(direction_to_tile_flags(guard_direction)),
        };

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

        // guard rotates
        if (guard_direction == vec{1, 0}) {
            guard_direction = {0, 1};
        } else if (guard_direction == vec{0, 1}) {
            guard_direction = {-1, 0};
        } else if (guard_direction == vec{-1, 0}) {
            guard_direction = {0, -1};
        } else if (guard_direction == vec{0, -1}) {
            guard_direction = {1, 0};
        } else {
            fmt::println("failure");
            std::abort();
        }
    }
    return false;
};

void thread_entry(std::unique_ptr<ThreadContext> context)
{
    VisitMap visits;
    visits.reserve(context->default_visits.size());
    std::vector<Visit> visit_sequence;
    visit_sequence.reserve(context->default_visit_sequence.size());
    std::vector<Row> map = context->map;
    size_t total_looping = 0;

    // loop over all possible places to put the obstacle
    for (size_t y = context->row_range.first; y < context->row_range.second;
         ++y) {
        for (size_t x = 0; x < map[0].size(); ++x) {
            uint8_t& tile = map[y][x];

            if (tile & uint8_t(TileFlags::IsObstacle) ||
                tile & uint8_t(TileFlags::IsGuard))
                continue;

            // find FIRST instance of hitting this newly obstacle-ed tile
            auto path_point =
                std::find_if(context->default_visit_sequence.begin(),
                             context->default_visit_sequence.end(),
                             [x, y](const Visit& visit) -> bool {
                                 return visit.location.first == x &&
                                        visit.location.second == y;
                             });

            // default path never touches this spot, skip it
            if (path_point == context->default_visit_sequence.end())
                continue;

            // okay, we are placing the tile in a place that gets traversed,
            // rewind to before that point

            visit_sequence.clear();
            std::copy(context->default_visit_sequence.begin(),
                      context->default_visit_sequence.end(),
                      std::back_inserter(visit_sequence));
            visits.clear();
            std::copy(context->default_visits.begin(),
                      context->default_visits.end(),
                      std::inserter(visits, visits.end()));

            const auto invalidated = std::ranges::subrange(
                path_point, context->default_visit_sequence.end());

            for (const Visit& v : invalidated)
                visits.erase(v);

            visit_sequence.erase(visit_sequence.begin() +
                                     (invalidated.begin() -
                                      context->default_visit_sequence.begin()),
                                 visit_sequence.end());

            uint8_t copy = tile;
            tile &= uint8_t(TileFlags::IsObstacle);
            const Visit& start = visit_sequence.empty()
                                     ? context->default_visit_sequence.front()
                                     : visit_sequence.back();
            const auto location = start.location;
            const auto dir = tile_flags_to_direction(start.direction);
            total_looping += is_route_looping_cached(visits, visit_sequence,
                                                     map, location, dir);
            tile = copy;
        }
    }

    context->counter->fetch_add(total_looping);
}

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
    std::vector<Row> map;
    vec guard = {std::numeric_limits<size_t>::max(),
                 std::numeric_limits<size_t>::max()};
    vec guard_direction;

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

    std::unordered_map<Visit, bool, VisitHasher> visits;
    std::vector<Visit> visit_sequence;

    // initialize cache
    bool looping = is_route_looping_cached(visits, visit_sequence, map, guard,
                                           guard_direction);
    assert(!looping); // should not loop in default with no obstacles

    std::atomic<size_t> loops = 0;
    std::vector<std::thread> threads;

    size_t num_threads = std::thread::hardware_concurrency();
    size_t rows_per_thread = std::max(1UL, map.size() / num_threads + 1);
    size_t last_claimed_row = 0;
    for (size_t i = 0; i < num_threads; ++i) {
        const size_t end_row =
            i == num_threads - 1
                ? map.size()
                : std::min(map.size(), last_claimed_row + rows_per_thread);
        fmt::println("deploying thread for {} to {}", last_claimed_row,
                     end_row);
        threads.emplace_back(thread_entry,
                             std::make_unique<ThreadContext>(ThreadContext{
                                 .map = map,
                                 .default_visits = visits,
                                 .default_visit_sequence = visit_sequence,
                                 .row_range = {last_claimed_row, end_row},
                                 .counter = &loops,
                             }));
        if (end_row == map.size())
            break;
        last_claimed_row = end_row;
    }

    for (auto& thread : threads)
        thread.join();
    threads.clear();

    fmt::println("looping paths: {}", loops.load());

    return 0;
}
