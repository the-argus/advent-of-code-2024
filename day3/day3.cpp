#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <ranges>

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
    int total = 0;
    bool enabled = true;
    while (std::getline(file, line)) {

        constexpr auto strtoint = [](auto numstr) {
            return std::stoi(std::string(numstr.begin(), numstr.end()));
        };

        for (auto it = line.begin(); it + 4 != line.end(); ++it) {
            if (std::string_view(it, it + 4) == "do()") {
                enabled = true;
                continue;
            }

            if (line.end() - it > 7)
                if (std::string_view(it, it + 7) == "don't()") {
                    enabled = false;
                    continue;
                }

            if (std::string(it, it + 4) != "mul(")
                continue;

            auto end = std::find(it + 4, line.end(), ')');
            if (end == line.end())
                break;

            auto params = std::string_view{it + 4, end};

            bool is_valid = true;
            for (char c : params) {
                if ((c < '0' || c > '9') && c != ',') {
                    is_valid = false;
                    break;
                }
            }
            if (!is_valid)
                continue;

            try {
                auto as_ints = std::views::split(params, ',') |
                               std::views::transform(strtoint);
                std::optional<int> amt = {};
                for (int i : as_ints) {
                    if (!amt.has_value()) {
                        amt = i;
                    } else {
                        amt = amt.value() * i;
                    }
                }
                if (enabled) {
                    total += *amt;
                }
            } catch (...) {
                continue;
            }
        }
    }

    fmt::println("total: {}", total);

    return 0;
}
