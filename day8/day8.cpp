#include "aoc.h"
#include "grid.h"
#include <unordered_map>

int main(int argc, char* argv[])
{
    auto file = open_stream(argc, argv);
    if (!file)
        return -1;

    Grid grid = Grid::parse(std::move(file.value()));

    std::array<std::vector<vec>, 256> positions_by_character;

    grid.for_each([&](Grid::enumerated_pair p) {
        auto [c, coord] = p;

        if (c != '.')
            positions_by_character.at(u8(c)).push_back(coord);
    });

    std::unordered_map<vec, bool, vec::hash> antinode_positions;

    for (auto& positions : positions_by_character) {
        for (const vec a : positions) {
            for (const vec b : positions) {
                if (a == b)
                    continue;

                const vec diff = b - a;
                vec iter = a;
                while (grid.is_inbounds(iter)) {
                    antinode_positions.insert({iter, true});
                    iter -= diff;
                }
                iter = b;
                while (grid.is_inbounds(iter)) {
                    antinode_positions.insert({iter, true});
                    iter += diff;
                }
            }
        }
    }

    fmt::println("found {} antinodes", antinode_positions.size());

    return 0;
}
