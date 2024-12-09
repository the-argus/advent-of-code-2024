#ifndef __AOC_H__
#define __AOC_H__

#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <optional>

inline std::optional<std::ifstream> open_stream(int argc, char* argv[])
{
    if (argc < 2) {
        fmt::println("give file pls");
        return {};
    }

    auto* name = argv[1];

    if (!std::filesystem::exists(name)) {
        fmt::println("Doesn't exist lol");
        return {};
    }

    std::ifstream file;
    file.open(name);
    if (!file.is_open()) {
        fmt::println("bad open");
        return {};
    }

    return file;
}

#endif
