#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <ranges>
#include <set>
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

    size_t total_appearances = 0;

    size_t x = 0;
    size_t y = 0;

    std::set<std::pair<size_t, size_t>> coords;

    // create a sliding 2D window over the wordsearch
    for (auto row = wordsearch.begin(); row + 4 != wordsearch.end(); ++row) {
        for (long i = 0; i + 4 < row->size(); ++i) {
            using subrange = std::ranges::subrange<decltype(row->begin())>;
            std::array<subrange, 4> window = {
                subrange{row->begin() + i, row->begin() + i + 4},
                subrange{(row + 1)->begin() + i, (row + 1)->begin() + i + 4},
                subrange{(row + 2)->begin() + i, (row + 2)->begin() + i + 4},
                subrange{(row + 3)->begin() + i, (row + 3)->begin() + i + 4},
            };

            constexpr auto is_phrase = [](auto begin, auto end) -> bool {
                constexpr std::string_view phrase = "XMAS";
                return std::equal(begin, end, phrase.begin(), phrase.end()) ||
                       std::equal(begin, end, phrase.rbegin(), phrase.rend());
            };

            // check horizontal
            for (auto row : window) {
                if (coords.contains({x, y})) {
                    if (is_phrase(row.begin(), row.end())) {
                        total_appearances += 1;
                    }
                }
            }

            // check vertical
            for (long i = 0; i < window[0].size(); ++i) {
                std::string col;
                for (auto row : window)
                    col += row[i];

                if (is_phrase(col.begin(), col.end()))
                    total_appearances += 1;
            }

            // check diagonals
            std::array diagonals = {
                std::string{
                    window[0][0],
                    window[1][1],
                    window[2][2],
                    window[3][3],
                },
                std::string{
                    window[3][0],
                    window[2][1],
                    window[1][2],
                    window[0][3],
                },
            };

            for (const auto& d : diagonals)
                if (is_phrase(d.begin(), d.end()))
                    total_appearances += 1;
        }
    }

    fmt::println("times xmas found {}", total_appearances);

    return 0;
}
