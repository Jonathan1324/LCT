#pragma once

#include <string>
#include <StringPool.hpp>

std::string trim(const std::string& str);
std::string toLower(const std::string& input);

inline std::string toLower(const StringPool::String& input)
{
    return toLower(std::string(input.c_str()));
}
