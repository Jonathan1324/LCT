#pragma once

#include <string>
#include <optional>
#include <Architecture.hpp>
#include "../Context.hpp"

bool parseArguments(int argc, const char *argv[],
                    std::vector<std::string>& inputs, std::string& output,
                    BitMode& bits, Architecture& arch, Format& format,
                    bool& debug, bool& preprocess, Context& context,
                    std::optional<std::string>& depFile, std::optional<std::string>& depType);
