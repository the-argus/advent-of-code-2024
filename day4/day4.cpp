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


    std::set<std::pair<size_t, size_t>> coords;

    // create a sliding 2D window over the wordsearch
    size_t y = 0;
    for (auto row = wordsearch.begin(); row + 4 != wordsearch.end(); ++row) {
        size_t x = 0;
        for (long i = 0; i + 4 < row->size(); ++i) {
            using subrange = std::ranges::subrange<decltype(row->begin())>;

            constexpr auto is_phrase = [](auto begin, auto end) -> bool {
                constexpr std::string_view phrase = "XMAS";
                return std::equal(begin, end, phrase.begin(), phrase.end()) ||
                       std::equal(begin, end, phrase.rbegin(), phrase.rend());
            };

            // check horizontal
            for (long j = 0; j < 4; ++j) {
                std::pair<size_t, size_t> startcoords = {x, y + i};
                if (!coords.contains(startcoords)) {
                    auto crow = row + j;
                    auto subrow_start = crow->begin() + i;
                    auto subrow_end = crow->begin() + i + 4;

                    if (is_phrase(subrow_start, subrow_end)) {
                        total_appearances += 1;
                    }
                }
                coords.insert(startcoords);
            }

            // check vertical
            for (long j = 0; j < 4; ++j) {
                std::pair<size_t, size_t> startcoords = {x, y};
                std::string col;
                for (size_t k; )
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

            x += 1;
        }
        y += 1;
    }

    fmt::println("times xmas found {}", total_appearances);

    return 0;
}
