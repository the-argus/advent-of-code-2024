#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <vector>

std::vector<std::vector<char>> parse_word_search(std::ifstream& file)
{
    std::vector<std::vector<char>> out;

    std::string line;
    while (std::getline(file, line)) {
        out.emplace_back(line.begin(), line.end());
    }

    return out;
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

    auto wordsearch = parse_word_search(file);
    file.close();

    const auto width = wordsearch[0].size();
    const auto height = wordsearch.size();

    const auto window_has_xmas = [&wordsearch, width,
                                  height](int64_t x, int64_t y) -> int64_t {
        constexpr auto is_mas = [](auto begin, auto end) -> bool {
            constexpr std::string_view phrase = "MAS";
            return std::equal(begin, end, phrase.begin(), phrase.end()) ||
                   std::equal(begin, end, phrase.rbegin(), phrase.rend());
        };

        std::vector<std::string> kernel;

        kernel.push_back(std::string{
            wordsearch.at(y + 0).at(x + 0),
            wordsearch.at(y + 1).at(x + 1),
            wordsearch.at(y + 2).at(x + 2),
        });
        kernel.push_back(std::string{
            wordsearch.at(y + 2).at(x + 0),
            wordsearch.at(y + 1).at(x + 1),
            wordsearch.at(y + 0).at(x + 2),
        });

        for (const auto& d : kernel)
            if (!is_mas(d.begin(), d.end()))
                return false;

        return true;
    };

    size_t total_appearances = 0;
    for (int64_t y = 0; y < height - 2; ++y)
        for (int64_t x = 0; x < width - 2; ++x)
            total_appearances += window_has_xmas(x, y);

    fmt::println("times xmas found {}", total_appearances);

    return 0;
}
