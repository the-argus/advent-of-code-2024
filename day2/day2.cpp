#include <cassert>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <optional>
#include <set>
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

    std::string line;
    size_t safecount = 0;
    std::set<int> uniques;
    std::vector<int> nums;
    while (std::getline(file, line)) {
        nums.clear();
        uniques.clear();

        size_t searchidx = 0;
        while (searchidx != std::string::npos && searchidx < line.length()) {
            size_t newidx = line.find(' ', searchidx);
            bool found = newidx != std::string::npos;
            auto numstr =
                line.substr(searchidx, found ? newidx - searchidx
                                             : line.length() - searchidx);

            int num = std::stoi(numstr);
            nums.push_back(num);
            if (found)
                searchidx = newidx + 1;
            else
                break;
        }

        uniques = std::set<int>(nums.cbegin(), nums.cend());
        if (uniques.size() < nums.size())
            continue; // duplicates

        auto lessthan = [](int a, int b) -> bool { return a < b; };
        auto greaterthan = [](int a, int b) -> bool { return a > b; };
        if (!(std::is_sorted(nums.cbegin(), nums.cend(), lessthan) ||
              std::is_sorted(nums.cbegin(), nums.cend(), greaterthan))) {
            continue;
        }

        std::optional<int> prev = {};
        bool safe = true;
        for (int i : nums) {
            if (prev.has_value()) {
                auto diff = abs(i - *prev);
                if (diff < 1 || diff > 3) {
                    safe = false;
                    break;
                }
            }
            prev = i;
        }
        safecount += safe ? 1 : 0;
    }

    fmt::println("safecount: {}", safecount);
    return 0;
}
