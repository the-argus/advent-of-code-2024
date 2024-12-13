#include "aoc.h"
#include "shorthand.h"

int main(int argc, char* argv[])
{
    auto file = open_stream(argc, argv).value();

    u64 total = 0;
    while (!file.eof()) {
        // resultcache.clear();
        std::string buttona;
        std::string buttonb;
        std::string prize;
        std::getline(file, buttona);
        std::getline(file, buttonb);
        std::getline(file, prize);
        if (!file.eof())
            file.ignore(1, '\n');

        i32 a_x;
        i32 a_y;
        // NOLINTNEXTLINE
        std::sscanf(buttona.c_str(), "Button A: X+%d, Y+%d", &a_x, &a_y);
        i32 b_x;
        i32 b_y;
        // NOLINTNEXTLINE
        std::sscanf(buttonb.c_str(), "Button B: X+%d, Y+%d", &b_x, &b_y);

        i32 prize_x;
        i32 prize_y;
        // NOLINTNEXTLINE
        std::sscanf(prize.c_str(), "Prize: X=%d, Y=%d", &prize_x, &prize_y);

        u64 a_times_best = 0;
        u64 b_times_best = 0;
        std::optional<u64> best_tokens;
        for (u64 a_times = 0; a_times < 100; ++a_times) {
            for (u64 b_times = 0; b_times < 100; ++b_times) {

                if ((a_times * a_x) + (b_times * b_x) != prize_x)
                    continue;
                if ((a_times * a_y) + (b_times * b_y) != prize_y)
                    continue;

                u64 tokens = 3 * a_times + b_times;
                if (!best_tokens || tokens < best_tokens.value()) {
                    best_tokens = tokens;
                    a_times_best = a_times;
                    b_times_best = b_times;
                }
            }
        }

        if (best_tokens)
            total += *best_tokens;
    }

    fmt::println("result: {}", total);
}
