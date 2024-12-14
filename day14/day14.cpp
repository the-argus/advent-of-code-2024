#include "aoc.h"
#include "grid.h"

struct Robot
{
    vec velocity;
};

int main(int argc, char* argv[])
{
    auto file = open_stream(argc, argv).value();

    using Map = Array2D<std::vector<Robot>>;

    constexpr Vec<u64> dimensions = {101, 103};
    constexpr u64 simulation_time = 100;
    Map map = Map::empty_of_dimensions(dimensions);

    std::string line;
    while (std::getline(file, line)) {
        vec position;
        vec velocity;
        // NOLINTNEXTLINE
        std::sscanf(line.c_str(), "p=%zd,%zd v=%zd,%zd", &position.x,
                    &position.y, &velocity.x, &velocity.y);

        map[position].push_back(Robot{.velocity = velocity});
    }
    file.close();

    Map back_map = Map::empty_of_dimensions(dimensions);

    Map* target = &back_map;
    Map* from = &map;
    for (u64 i = 0; i < 1000; ++i) {
        fmt::println("{} seconds passed", i);

        // initial state before simulation
        from->transform<char>([](const Map::enumerated_pair& robots) -> char {
                return robots.first.empty() ? '.' : 'R';
            })
            .debug_print();

        // simulate one second
        from->for_each([dimensions, target](Map::enumerated_pair e) {
            auto [robots, pos] = e;
            if (robots.empty())
                return;
            for (const Robot& r : robots) {
                constexpr vec center =
                    (vec::convert(dimensions) / vec::splat(2));

                vec newpos = (pos + r.velocity) % vec::convert(dimensions);
                if (newpos.x < 0)
                    newpos.x += dimensions.x;
                if (newpos.y < 0)
                    newpos.y += dimensions.y;

                (*target)[newpos].push_back(r);
            }
        });

        // next link in markov chain
        from->clear();
        std::swap(target, from);
    }
}
