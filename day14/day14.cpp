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

    std::array<u64, 4> robot_counts = {};

    map.for_each([dimensions, simulation_time,
                  &robot_counts](Map::enumerated_pair e) {
        auto [robots, pos] = e;
        if (robots.empty())
            return;
        for (const Robot& r : robots) {
            const vec delta = r.velocity * vec::splat(simulation_time);

            constexpr vec center = (vec::convert(dimensions) / vec::splat(2));

            vec newpos = (pos + delta) % vec::convert(dimensions);
            if (newpos.x < 0)
                newpos.x += dimensions.x;
            if (newpos.y < 0)
                newpos.y += dimensions.y;

            assert(newpos.x < dimensions.x && newpos.x >= 0);
            assert(newpos.y < dimensions.y && newpos.y >= 0);

            // check if robot exactly on boundaries
            if (newpos.x == center.x || newpos.y == center.y)
                continue;

            // quadrants
            robot_counts.at(((newpos.y > center.y) * 2) +
                            (newpos.x > center.x))++;
        }
    });

    const u64 total =
        robot_counts[0] * robot_counts[1] * robot_counts[2] * robot_counts[3];
    fmt::println("safety factor {}", total);
}
