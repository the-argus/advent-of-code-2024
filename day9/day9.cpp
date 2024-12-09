#include "aoc.h"
#include "shorthand.h"
#include <cassert>
#include <list>
#include <ranges>
#include <vector>

struct Block
{
    u64 size;
    i64 id = -1;
    [[nodiscard]] inline bool is_free() const { return id < 0; }
    static inline Block free_block(u64 size) { return {.size = size}; }
};

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

    std::list<Block> blocks;

    for (i64 i = 0; i < nums.size(); ++i) {
        i64 num = nums.at(i);
        if (i % 2 == 0) {
            assert(num >= 0);
            blocks.push_back(Block{.size = u64(num), .id = i / 2});
        } else {
            blocks.push_back(Block::free_block(num));
        }
    }

    auto back_iter = std::rbegin(blocks);
    while (back_iter != std::rend(blocks)) {
        if (back_iter->is_free()) {
            ++back_iter;
            continue;
        }

        // NOTE: back_iter.base() offset by one so search includes back_iter
        auto replacement = std::find_if(
            std::begin(blocks), back_iter.base(), [back_iter](const Block& b) {
                return b.is_free() && b.size >= back_iter->size;
            });

        if (replacement == back_iter.base()) {
            ++back_iter;
            continue;
        }

        // put block before free block
        blocks.insert(replacement, *back_iter);
        // mark original block as free
        *back_iter = Block::free_block(back_iter->size);
        // shrink free block by however much this new one is
        replacement->size -= back_iter->size;
        ++back_iter;
    }

    u64 checksum = 0;
    u64 idx = 0;
    for (Block block : blocks) {
        if (!block.is_free()) {
            for (u64 i = idx; i < idx + block.size; ++i)
                checksum += block.id * i;
        }
        idx += block.size;
    }

    fmt::println("checksum: {}", checksum);
    return 0;
}
