#ifndef TEMP_FILE_PATH_H
#define TEMP_FILE_PATH_H

#include <filesystem>
#include <random>
#include <string>

inline std::string temp_file_path(const std::string& prefix) {
    auto path = std::filesystem::temp_directory_path();
    path.append(prefix);

    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::filesystem::path full;
    do {
        full = path;
        full += std::to_string(rng());
    } while (std::filesystem::exists(full));

    return full;
}

#endif
