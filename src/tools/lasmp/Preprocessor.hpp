#pragma once

#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include "Context.hpp"

struct Definition
{
    std::string name;
    std::string value;
};

class PreProcessor
{
public:
    PreProcessor(const Context& _context);
    ~PreProcessor() = default;

    enum class DepType {
        Normal,
        Makefile,
        MSVC
    };

    void Process(std::ostream* output, std::istream* input, const std::filesystem::path& file, DepType type, uint64_t indent = 0);
    void Print();

    void DumpDependencies(std::ostream* output, const std::string& outFile, DepType type);

private:
    const Context& context;

    std::unordered_map<std::string, Definition> definitions;

    std::string ProcessLine(const std::string& line);

    std::unordered_set<std::string> dependencies;

    struct Dependency {
        std::string file;
        uint64_t indent;
    };

    std::vector<Dependency> dependencyFiles;
};
