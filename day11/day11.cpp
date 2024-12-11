#include "aoc.h"
#include "shorthand.h"
#include <ranges>
#include <vector>

struct Node
{
    u64 value;
    std::vector<std::unique_ptr<Node>> children;

    u64 populate_recursive_to_depth(u64 depth);
};

u64 Node::populate_recursive_to_depth(u64 depth)
{
    if (depth == 0)
        return 1;

    if (value == 0) {
        value = 1;
        return populate_recursive_to_depth(depth - 1);
    } else if (auto str = std::to_string(value); str.length() % 2 == 0) {
        auto lefti = str | std::views::take(str.length() / 2);
        auto righti = str | std::views::drop(str.length() / 2);
        std::string left(std::begin(lefti), std::end(lefti));
        std::string right(std::begin(righti), std::end(righti));

        children.push_back(
            std::make_unique<Node>(Node{.value = u64(std::stol(left))}));
        children.push_back(
            std::make_unique<Node>(Node{.value = u64(std::stol(right))}));
    } else {
        value = value * 2024;
        return populate_recursive_to_depth(depth - 1);
    }

    u64 total = 0;
    for (auto& child : children)
        total += child->populate_recursive_to_depth(depth - 1);
    children.clear();
    return total;
}

int main(int argc, char* argv[])
{
    auto mstream = open_stream(argc, argv);
    if (!mstream)
        return -1;

    auto& file = mstream.value();
    std::string line;
    std::getline(file, line);

    constexpr auto strtonode = [](auto numstr) {
        u64 i = std::stol(std::string(numstr.begin(), numstr.end()));
        return std::make_unique<Node>(Node{.value = i});
    };
    auto nodes =
        line | std::views::split(' ') | std::views::transform(strtonode);

    u64 total = 0;
    for (const std::unique_ptr<Node>& node : nodes)
        total += node->populate_recursive_to_depth(75);

    fmt::println("got nodes at max depth: {}", total);

    file.close();
    return 0;
}
