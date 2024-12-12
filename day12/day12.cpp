#include "aoc.h"
#include "grid.h"
#include <unordered_map>

constexpr std::array neighbors = {vec{1, 0}, vec{-1, 0}, vec{0, 1}, vec{0, -1}};

int main(int argc, char* argv[])
{
    Grid grid = Grid::parse(open_stream(argc, argv).value());

    std::unordered_map<vec, u64, vec::hash> position_to_region;
    std::vector<std::vector<vec>> regions;

    std::function<void(vec, char, u64)> explore;
    explore = [&](vec tile, char region, u64 region_idx) {
        if (!grid.is_inbounds(tile) || grid[tile] != region ||
            position_to_region.contains(tile))
            return;

        position_to_region[tile] = region_idx;
        regions.at(region_idx).push_back(tile);

        for (auto v : neighbors)
            explore(tile + v, region, region_idx);
    };

    grid.for_each([&](Grid::enumerated_pair e) {
        auto [c, pos] = e;
        if (!position_to_region.contains(pos))
            regions.push_back({});
        explore(pos, c, regions.size() - 1);
    });

    u64 total = 0;
    for (const auto& region : regions) {

        u64 sides = 0;
        // tile -> { up/down/left/right: side ID }
        std::unordered_map<vec, std::array<std::optional<u64>, 4>, vec::hash>
            tile_side_ids;

        for (vec a : region) {
            char c = grid[a];

            for (u8 r = 0; r < u8(neighbors.size()); ++r) {
                const vec relationship = neighbors[r];
                const vec n = a + relationship;

                // skip if there is not an edge between us and neighbor
                if (grid.is_inbounds(n) && grid[n] == c)
                    continue;

                {
                    const auto& ids = tile_side_ids[a];
                    // if this edge already established for this tile,
                    // we have already found this edge
                    if (ids.at(r)) {
                        continue;
                    }
                }

                // there is edge, make iterators to move out in either direction
                const vec iter_a = relationship.x == 0 ? vec{1, 0} : vec{0, 1};
                const vec iter_b =
                    relationship.x == 0 ? vec{-1, 0} : vec{0, -1};

                for (const vec delta : {iter_a, iter_b}) {
                    vec iter = a;
                    while (grid.is_inbounds(iter) && grid[iter] == c) {

                        const vec iters_neighbor = iter + relationship;

                        // check if edge even exists for this spot, if not end
                        // side
                        if (grid.is_inbounds(iters_neighbor) &&
                            grid[iters_neighbor] == c)
                            break;

                        // insert and get
                        auto& ids = tile_side_ids[iter];
                        ids.at(r) = sides;

                        iter += delta;
                    }
                }

                ++sides;
            }
        }
        total += sides * region.size();
    }

    fmt::println("total region cost {}", total);
    return 0;
}
