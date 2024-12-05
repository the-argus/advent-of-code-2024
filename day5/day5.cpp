#include <cassert>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <ranges>
#include <set>
#include <span>
#include <unordered_map>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        fmt::println("give file pls");
        return -1;
    }

    auto* name = argv[1];

    if (!std::filesystem::exists(name)) {
        fmt::println("Doesn't exist lol");
        return -1;
    }

    std::ifstream file;
    file.open(name);
    if (!file.is_open()) {
        fmt::println("bad open");
        return -1;
    }

    struct before_after
    {
        std::set<int32_t> numbers_that_should_go_before;
        std::set<int32_t> numbers_that_should_go_after;
    };

    std::unordered_map<int32_t, before_after> rules;

    constexpr auto strtoint = [](auto numstr) {
        return std::stoi(std::string(numstr.begin(), numstr.end()));
    };

    constexpr auto has_any_in_common = [](std::span<int32_t> a,
                                          std::set<int32_t> b) -> bool {
        const auto is_in_b = [&b](int32_t i) -> bool { return b.contains(i); };
        return std::ranges::any_of(a.begin(), a.end(), is_in_b);
    };

    std::string line;
    size_t total = 0;
    while (std::getline(file, line)) {
        bool is_rule = std::find(line.begin(), line.end(), '|') != line.end();
        bool is_update = std::find(line.begin(), line.end(), ',') != line.end();

        if (is_rule == is_update)
            continue; // should only be one or the other

        if (is_rule) {
            std::vector<int32_t> as_ints;
            auto split =
                std::views::split(line, '|') | std::views::transform(strtoint);
            std::copy(split.begin(), split.end(), std::back_inserter(as_ints));
            assert(as_ints.size() == 2);

            rules[as_ints[0]].numbers_that_should_go_after.insert(as_ints[1]);
            rules[as_ints[1]].numbers_that_should_go_before.insert(as_ints[0]);
        } else if (is_update) {
            // NOTE: relies on the rules all appearing in the file before the
            // updates
            std::vector<int32_t> update;
            auto nums =
                std::views::split(line, ',') | std::views::transform(strtoint);
            std::copy(nums.begin(), nums.end(), std::back_inserter(update));

            // they want the middle number, not sure what to do if even size
            assert(update.size() % 2 == 1);

            bool good_update = true;
            for (size_t i = 0; i < update.size(); ++i) {
                auto& rule = rules[update[i]];
                std::span before{update.begin(), i};
                std::span after{update.begin() + long(i) + 1, update.end()};

                if (has_any_in_common(before,
                                      rule.numbers_that_should_go_after) ||
                    has_any_in_common(after,
                                      rule.numbers_that_should_go_before)) {
                    good_update = false;
                    break;
                }
            }

            if (good_update) {
                total += update[update.size() / 2];
            }
        }
    }

    fmt::println("addition of middle of good rules {}", total);

    return 0;
}
