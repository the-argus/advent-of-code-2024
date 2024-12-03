#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <ranges>
#include <set>
#include <vector>

bool is_safe(const std::vector<int>& nums)
{
    if (!(std::ranges::is_sorted(nums, std::ranges::less{}) ||
          std::ranges::is_sorted(nums, std::ranges::greater{}))) {
        return false;
    }

    auto uniques = std::set(std::ranges::begin(nums), std::ranges::end(nums));
    if (uniques.size() != nums.size()) {
        return false;
    }

    for (auto it = nums.begin(); it + 1 != nums.end(); ++it) {
        int diff = abs(*it - *(it + 1));
        if (diff < 1 || diff > 3)
            return false;
    }

    return true;
}

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
    std::vector<int> nums;
    while (std::getline(file, line)) {
        nums.clear();

        auto strtoint = [](auto numstr) {
            return std::stoi(std::string(numstr.begin(), numstr.end()));
        };

        auto as_ints =
            std::views::split(line, ' ') | std::views::transform(strtoint);

        std::ranges::copy(as_ints, std::back_inserter(nums));

        // early out
        if (is_safe(nums)) {
            safecount++;
            continue;
        }

        std::vector<int> removed;
        for (long i = 0; i < nums.size(); ++i) {
            std::copy(nums.begin(), nums.end(), std::back_inserter(removed));
            removed.erase(removed.begin() + i);
            if (is_safe(removed)) {
                safecount++;
                break;
            }
            removed.clear();
        }
    }

    fmt::println("safecount: {}", safecount);
    return 0;
}
