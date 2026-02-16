#pragma once

#include <filesystem>
#include <Exception.hpp>

struct Context
{
    WarningManager* warningManager;
    std::filesystem::path current_path;
    std::filesystem::path filename;
    std::vector<std::filesystem::path> include_paths;
};
