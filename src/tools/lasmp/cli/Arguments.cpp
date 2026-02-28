#include "Arguments.hpp"

#include <cstring>
#include <Exception.hpp>
#include <version.h>
#include <util/string.hpp>

void printHelp(const char* name, std::ostream& s)
{
    s << "Usage: " << name << " <input> (-o <output>) [--depfile <file>] [--deptype <type>] [--depout <target file name for depfile>]" << std::endl;
}

bool parseArguments(
    int argc, const char *argv[],
    std::string& input, std::string& output,
    bool& debug, std::optional<std::string>& depfile,
    std::optional<std::string>& depOut, PreProcessor::DepType& depType,
    const Context& context
) {
    if (argc < 2)
    {
        throw Exception::ArgumentError("No input file specified", -1, -1, "command-line");
    }

    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)
    {
        printVersion();
        return true;
    }
    else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        printHelp(argv[0], std::cout);
        return true;
    }

    debug = false;
    depfile = std::nullopt;
    depOut = std::nullopt;
    depType = PreProcessor::DepType::Normal;

    bool inputSet = false;
    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]).compare("-o") == 0)
        {
            if (i + 1 < argc)
                output = argv[++i];
            else
                throw Exception::ArgumentError("Missing output file after '-o'", -1, -1, "command-line");
        }
        else if (std::string(argv[i]).compare("--depfile") == 0)
        {
            if (i + 1 < argc)
                depfile = argv[++i];
            else
                throw Exception::ArgumentError("Missing dependency file after '-depfile'", -1, -1, "command-line");
        }
        else if (std::string(argv[i]).compare("--depout") == 0)
        {
            if (i + 1 < argc)
                depOut= argv[++i];
            else
                throw Exception::ArgumentError("Missing output file after '-depout'", -1, -1, "command-line");
        }
        else if (std::string(argv[i]).compare("--deptype") == 0)
        {
            if (i + 1 < argc)
            {
                i++;
                if (std::strcmp(argv[i], "normal") == 0)
                    depType = PreProcessor::DepType::Normal;
                else if (std::strcmp(argv[i], "make") == 0 || std::strcmp(argv[i], "makefile") == 0)
                    depType = PreProcessor::DepType::Makefile;
                else if (std::strcmp(argv[i], "msvc") == 0)
                    depType = PreProcessor::DepType::MSVC;
                else
                    throw Exception::ArgumentError("Invalid dependency type", -1, -1, "command-line");
            }
            else
                throw Exception::ArgumentError("Missing dependency type after '-deptype'", -1, -1, "command-line");
        }
        else if (std::string(argv[i]).compare("--debug") == 0 || std::string(argv[i]).compare("-d") == 0)
        {
            debug = true;
        }
        else if (argv[i][0] == '-' && argv[i][1] != '\0')
        {
            context.warningManager->add(Warning::ArgumentWarning("Unknown option: " + std::string(argv[i])));
        }
        else
        {
            if (inputSet)
                throw Exception::ArgumentError("Too many input streams specified", -1, -1, "command-line");
            input = argv[i];
            inputSet = true;
        }
    }

    if (input.empty())
        throw Exception::ArgumentError("No input file entered", -1, -1, "command-line");

    if (output.empty())
    {
        output = "-";
    }

    return false;
}