#include "aoc.h"
#include "grid.h"
#include <unordered_map>

struct PathNode;

using PeakMap = std::unordered_map<vec, PathNode*, vec::hash>;

// double linked directed graph
// i think this is acyclic? but if its not and we refcycle, whatever
struct PathNode
{
    vec pos;
    std::optional<PeakMap> explored;
};

int main(int argc, char* argv[])
{
    auto mfile = open_stream(argc, argv);
    if (!mfile)
        return -1;

    Grid grid = Grid::parse(std::move(mfile.value()));

    std::unordered_map<vec, std::unique_ptr<PathNode>, vec::hash> all_pathnodes;
    PeakMap trailheads;
    PeakMap peaks;

    grid.for_each([&](Grid::enumerated_pair e) {
        auto [c, pos] = e;
        if (c == '0' || c == '9') {
            auto p = std::make_unique<PathNode>(PathNode{.pos = pos});
            auto* weakp = p.get();
            all_pathnodes.insert({vec(pos), std::move(p)});
            (c == '0' ? trailheads : peaks).insert({vec(pos), weakp});
        }
    });

    std::function<PeakMap(PathNode*)> explore;
    explore = [&](PathNode* p) -> PeakMap {
        if (grid[p->pos] == '9')
            p->explored = {{p->pos, p}};

        if (p->explored)
            return p->explored.value();

        constexpr std::array neighbors = {vec{1, 0}, vec{-1, 0}, vec{0, 1},
                                          vec{0, -1}};

        PeakMap peaks_found;

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

            PeakMap found = explore(nptr);
            std::copy(std::begin(found), std::end(found),
                      std::inserter(peaks_found, peaks_found.end()));
        }
        return peaks_found;
    };

    u64 total = 0;
    for (const auto& pair : trailheads)
        total += explore(pair.second).size();

    fmt::println("total trailhead scores: {}", total);

    return 0;
}
