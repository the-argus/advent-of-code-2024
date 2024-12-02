#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <optional>

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
    while (std::getline(file, line)) {
        size_t searchidx = 0;
        int prev = 0;
        std::optional<bool> decreasing = {};
        bool safe = true;
        while (searchidx != std::string::npos) {
            size_t newidx = line.find(' ', searchidx);
            auto numstr = line.substr(searchidx,
                                      newidx == std::string::npos ? 0 : newidx);
            searchidx = newidx;

            int num = std::stoi(numstr);
            if (searchidx != 0) {
                if (!decreasing.has_value())
                    decreasing = num < prev;

                if ((num == prev) || (*decreasing && num > prev) ||
                    (!*decreasing && num < prev)) {
                    safe = false;
                    break;
                }

                auto diff = abs(num - prev);
                if (diff < 1 || diff > 3) {
                    safe = false;
                    break;
                }
            }
            prev = num;
        }
        if (safe)
            ++safecount;
    }

    fmt::println("safecount: {}", safecount);
    return 0;
}
