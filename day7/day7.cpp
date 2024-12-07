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

        std::vector<uint8_t> ops;
        ops.resize(right.size() - 1);
        std::fill(ops.begin(), ops.end(), 0);

        auto possible_combos = size_t(std::pow(3, ops.size()));
        for (size_t i = 0; i < possible_combos; ++i) {
            // count in base 3
            for (uint8_t& slot : ops) {
                if (slot < 2) {
                    slot++;
                    break;
                } else {
                    slot = 0;
                    continue;
                }
            }

            size_t accum = right[0];
            for (size_t j = 1; j < right.size(); ++j) {
                switch (ops[j - 1]) {
                case 2: {
                    accum += right[j];
                    break;
                }
                case 1: {
                    accum *= right[j];
                    break;
                }
                case 0: {
                    accum = strtoint(std::to_string(accum) +
                                     std::to_string(right[j]));
                    break;
                }
                default:
                    fmt::println("wut");
                    std::abort();
                }
            }

            if (accum == left) {
                total += left;
                break;
            }
        }
    }
    fmt::println("total: {}", total);
}
