#pragma once

#include <string>
#include <optional>
#include "../Context.hpp"
#include "../Preprocessor.hpp"

bool parseArguments(
    int argc, const char *argv[],
    std::string& input, std::string& output,
    bool& debug, std::optional<std::string>& depfile,
    std::optional<std::string>& depOut, PreProcessor::DepType& depType,
    const Context& context
);
