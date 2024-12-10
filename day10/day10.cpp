#include "aoc.h"
#include "grid.h"
#include <unordered_map>

struct PathNode;

using PeakMap = std::unordered_map<vec, PathNode*, vec::hash>;

struct PathNode
{
    vec pos;
    std::optional<u64> explored;
};

int main(int argc, char* argv[])
{
    auto mfile = open_stream(argc, argv);
    if (!mfile)
        return -1;

    Grid grid = Grid::parse(std::move(mfile.value()));

    std::unordered_map<vec, std::unique_ptr<PathNode>, vec::hash> all_pathnodes;
    PeakMap trailheads;

    grid.for_each([&](Grid::enumerated_pair e) {
        auto [c, pos] = e;
        if (c == '0') {
            auto p = std::make_unique<PathNode>(PathNode{.pos = pos});
            auto* weakp = p.get();
            all_pathnodes.insert({vec(pos), std::move(p)});
            trailheads.insert({vec(pos), weakp});
        }
    });

    std::function<u64(PathNode*)> explore;
    explore = [&](PathNode* p) -> u64 {
        if (grid[p->pos] == '9')
            p->explored = 1;

        // we have already counted the number of unique paths
        if (p->explored)
            return p->explored.value();

        constexpr std::array neighbors = {vec{1, 0}, vec{-1, 0}, vec{0, 1},
                                          vec{0, -1}};

        u64 peaks_found = 0;

        // check all neighbors for uphill ones
        for (const vec& offset : neighbors) {
            vec neighbor = p->pos + offset;
            if (!grid.is_inbounds(neighbor))
                continue;

            // make sure neighbor is one up from us
            if (grid[neighbor] - grid[p->pos] != 1)
                continue;

            PathNode* nptr = nullptr;
            if (!all_pathnodes.contains(neighbor)) {
                auto newp =
                    std::make_unique<PathNode>(PathNode{.pos = neighbor});
                nptr = newp.get();
                all_pathnodes.insert({neighbor, std::move(newp)});
            } else {
                nptr = all_pathnodes.at(neighbor).get();
            }

            peaks_found += explore(nptr);
        }
        p->explored = peaks_found;
        return p->explored.value();
    };

    u64 total = 0;
    for (const auto& pair : trailheads)
        total += explore(pair.second);

    fmt::println("total trailhead scores: {}", total);

    return 0;
}
