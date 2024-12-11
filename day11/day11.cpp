#include "aoc.h"
#include "shorthand.h"
#include <ranges>
#include <unordered_map>

using pair = std::pair<u64, u64>;

struct pairhash
{
    std::size_t operator()(pair const& v) const
    {
        return std::hash<u64>()(v.first ^ v.second);
    }
};

u64 populate_recursive_to_depth(const pair args)
{
    static std::unordered_map<pair, u64, pairhash> resultcache;
    auto [value, depth] = args;

    if (depth == 0)
        return 1;

    if (resultcache.contains(args))
        return resultcache.at(args);

    if (value == 0) {
        resultcache[args] = populate_recursive_to_depth({1, depth - 1});
    } else if (auto str = std::to_string(value); str.length() % 2 == 0) {

        const auto lefti = str | std::views::take(str.length() / 2);
        const auto righti = str | std::views::drop(str.length() / 2);

        const std::string left(std::begin(lefti), std::end(lefti));
        const std::string right(std::begin(righti), std::end(righti));

        const pair largs = {std::stol(left), depth - 1};
        const pair rargs = {std::stol(right), depth - 1};

        const u64 lres = populate_recursive_to_depth(largs);
        resultcache[args] = lres + populate_recursive_to_depth(rargs);
    } else {
        resultcache[args] =
            populate_recursive_to_depth({value * 2024, depth - 1});
    }

    u64 result = resultcache.at(args);
    return result;
}

int main(int argc, char* argv[])
{
    auto mstream = open_stream(argc, argv);
    if (!mstream)
        return -1;

    auto& file = mstream.value();
    std::string line;
    std::getline(file, line);

    constexpr auto strtoint = [](auto numstr) {
        return std::stol(std::string(numstr.begin(), numstr.end()));
    };
    auto root = line | std::views::split(' ') | std::views::transform(strtoint);

    u64 total = 0;
    for (i64 i : root)
        total += populate_recursive_to_depth({i, 75});

    fmt::println("got nodes at max depth: {}", total);

    file.close();
    return 0;
}
