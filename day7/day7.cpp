#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <numeric>
#include <ranges>
#include <vector>

enum class Op
{
    Mul,
    Add,
};

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
    size_t total;
    while (std::getline(file, line)) {

        constexpr auto strtoint = [](auto numstr) -> size_t {
            return std::stol(std::string(numstr.begin(), numstr.end()));
        };

        size_t idx = line.find(':');

        size_t left = strtoint(line.substr(0, idx));
        std::string rightstr = line.substr(idx + 2);
        auto as_ints =
            std::views::split(rightstr, ' ') | std::views::transform(strtoint);

        std::vector<size_t> right;
        std::copy(as_ints.begin(), as_ints.end(), std::back_inserter(right));

        size_t max = 1 << right.size();

        for (size_t i = 0; i < max; ++i) {
            size_t initial = right.at(0);
            for (size_t bit = 1; bit < right.size(); ++bit) {
                bool has = ((1 << (bit - 1)) & i) != 0;
                if (has)
                    initial *= right.at(bit);
                else
                    initial += right.at(bit);
            }
            if (left == initial) {
                total += left;
                break;
            }
        }
    }
    fmt::println("total: {}", total);
}
