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

    const auto count_occurrences_in_window =
        [&wordsearch, width, height](int64_t x, int64_t y) -> int64_t {
        int64_t result = 0;

        constexpr auto is_phrase = [](auto begin, auto end) -> bool {
            constexpr std::string_view phrase = "XMAS";
            return std::equal(begin, end, phrase.begin(), phrase.end()) ||
                   std::equal(begin, end, phrase.rbegin(), phrase.rend());
        };

        std::vector<std::string> kernel;

        bool hgood = x + 4 <= width;
        bool vgood = y + 4 <= height;

        if (hgood && vgood) {
            // diagonals
            kernel.push_back(std::string{
                wordsearch[y + 0][x + 0],
                wordsearch[y + 1][x + 1],
                wordsearch[y + 2][x + 2],
                wordsearch[y + 3][x + 3],
            });
            kernel.push_back(std::string{
                wordsearch[y + 3][x + 0],
                wordsearch[y + 2][x + 1],
                wordsearch[y + 1][x + 2],
                wordsearch[y + 0][x + 3],
            });
        }

        if (hgood)
            kernel.push_back(std::string{
                wordsearch[y].begin() + x,
                wordsearch[y].begin() + x + 4,
            });

        if (vgood)
            kernel.push_back(std::string{
                wordsearch[y + 0][x],
                wordsearch[y + 1][x],
                wordsearch[y + 2][x],
                wordsearch[y + 3][x],
            });

        for (const auto& d : kernel)
            result += is_phrase(d.begin(), d.end());

        return result;
    };

    size_t total_appearances = 0;
    for (int64_t y = 0; y < wordsearch.size(); ++y)
        for (int64_t x = 0; x < wordsearch[0].size(); ++x)
            total_appearances += count_occurrences_in_window(x, y);

    fmt::println("times xmas found {}", total_appearances);

    return 0;
}
