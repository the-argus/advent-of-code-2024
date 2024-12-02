#include <filesystem>
#include <fmt/core.h>
#include <fstream>
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

    std::vector<int> leftnums;
    std::vector<int> rightnums;

    std::string line;
    while (std::getline(file, line)) {
        size_t idx = line.find(' ');
        auto left(line.substr(0, idx));
        auto right(line.substr(idx));

        leftnums.push_back(std::stoi(left));
        rightnums.push_back(std::stoi(right));
    }

    std::stable_sort(std::begin(leftnums), std::end(leftnums));
    std::stable_sort(std::begin(rightnums), std::end(rightnums));

    size_t totaldiff = 0;
    for (size_t i = 0; i < leftnums.size(); ++i) {
        totaldiff += abs(leftnums.at(i) - rightnums.at(i));
    }

    fmt::println("{}", totaldiff);
    return 0;
}
