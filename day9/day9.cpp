#include "aoc.h"
#include "shorthand.h"
#include <cassert>
#include <ranges>
#include <vector>

int main(int argc, char* argv[])
{
    auto maybe_file = open_stream(argc, argv);
    if (!maybe_file)
        return -1;
    auto& file = maybe_file.value();

    std::string line = "2333133121414131402";
    std::getline(file, line);
    file.close();

    auto numview =
        line | std::views::transform([](char c) -> i64 { return c - '0'; });

    std::vector<i64> nums;
    std::copy(std::cbegin(numview), std::cend(numview),
              std::back_inserter(nums));

    std::vector<i64> blocks;
    blocks.reserve(nums.size());

    for (i64 i = 0; i < nums.size(); ++i) {
        i64 num = nums.at(i);
        if (i % 2 == 0) {
            assert(num >= 0 && num < 10);
            for (u64 j = 0; j < num; ++j)
                blocks.push_back(i / 2);
        } else {
            for (u64 j = 0; j < num; ++j)
                blocks.push_back(-1);
        }
    }

    auto backwards_iter = std::rbegin(blocks);
    auto forwards_iter = std::begin(blocks);
    while (true) {
        // find first number at the end thats not negative 1
        backwards_iter = std::find_if(backwards_iter, std::rend(blocks),
                                      [](i64 i) { return i >= 0; });
        forwards_iter = std::find(forwards_iter, std::end(blocks), -1);

        // if they cross, break
        if (forwards_iter - std::begin(blocks) >=
            std::rend(blocks) - backwards_iter)
            break;

        *forwards_iter = *backwards_iter;
        *backwards_iter = -1;
    }

    u64 checksum = 0;
    for (u64 i = 0; i < blocks.size(); ++i) {
        i64 num = blocks.at(i);
        if (num < 0)
            break;

        checksum += num * i;
    }

    fmt::println("checksum: {}", checksum);
    return 0;
}
