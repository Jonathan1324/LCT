#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include <io/file.hpp>
#include <Architecture.hpp>
#include <Exception.hpp>
#include <StringPool.hpp>
#include "cli/Arguments.hpp"
#include "Context.hpp"

#include "Parser/Tokenizer.hpp"
#include "Parser/Parser.hpp"
#include "Encoder/Encoder.hpp"
#include "OutputWriter/OutputWriter.hpp"
#include <rust.h>

#define CLEANUP                             \
    do {                                    \
    if (parser) delete parser;              \
    if (encoder) delete encoder;            \
    if (outputWriter) delete outputWriter;  \
    if (outputFile != "-" && objectFile) delete objectFile;\
    } while(0)                              \

#define ERROR_HANDLER                       \
    do {                                    \
    /* delete outputFile */                 \
    if (outputFile != "-" && objectFile)    \
    {                                       \
        delete objectFile;                  \
        objectFile = nullptr;               \
    }                                       \
    std::remove(outputFile.c_str());        \
    CLEANUP;                                \
    } while(0)                              \

int handleError(const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return 1;
}

int main(int argc, const char *argv[])
{
    // TODO: remove this once it's finished
    std::cerr << "Warning: Assembler isn't stable yet and still has bugs" << std::endl;

    WarningManager warningManager;
    Context context;
    context.warningManager = &warningManager;

    StringPool stringPool;
    context.stringPool = &stringPool;

    std::vector<std::string> inputFiles;
    std::string outputFile;
    BitMode bitMode;
    Architecture arch;
    Format format;
    bool debug;
    bool doPreprocess;

    std::optional<std::string> depFile;
    std::optional<std::string> depType;

    Parser::Parser* parser = nullptr;
    Encoder::Encoder* encoder = nullptr;
    Output::Writer* outputWriter = nullptr;

    // Parse arguments
    try
    {
        bool stop = parseArguments(argc, argv, inputFiles, outputFile, bitMode, arch, format, debug, doPreprocess, context, depFile, depType);
        if (stop)
            return 0;
        
        if (warningManager.hasWarnings())
        {
            warningManager.printAll(std::cerr);
            warningManager.clear();
        }
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        return 1;
    }
    catch(const std::exception& e)
    {
        return handleError(e);
    }

    context.filename = std::filesystem::path(inputFiles.at(0)).string();

    // Create file handles and tokenize
    std::ostream* objectFile = nullptr;
    Token::Tokenizer tokenizer(context);
    try
    {
        objectFile = openOstream(outputFile, std::ios::out | std::ios::trunc | std::ios::binary);

        tokenizer.clear();

        std::stringstream combinedInput;
        for (const std::string& inputFile : inputFiles)
        {
            std::istream* file = openIstream(inputFile);
            if (!file || !(*file))
                throw Exception::InternalError("Failed to open input file: " + inputFile, -1, -1);

            combinedInput << file->rdbuf();

            if (inputFile != "-")
                delete file;
        }

        std::istream* input = &combinedInput;

        tokenizer.tokenize(input);

        if (input != &combinedInput)
            delete input;

        if (debug)
            tokenizer.print();
        
        if (warningManager.hasWarnings())
        {
            warningManager.printAll(std::cerr);
            warningManager.clear();
        }
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        ERROR_HANDLER;
        return 1;
    }
    catch(const std::exception& e)
    {
        ERROR_HANDLER;
        return handleError(e);
    }

    context.filename = std::filesystem::path(inputFiles.at(0)).string();

    // Parse
    try
    {
        parser = Parser::getParser(context, arch, bitMode);
        if (!parser)
            throw Exception::InternalError("Couldn't get parser", -1, -1);
        
        parser->Parse(tokenizer.getTokens());
        if (debug)
            parser->Print();

        if (warningManager.hasWarnings())
        {
            warningManager.printAll(std::cerr);
            warningManager.clear();
        }
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        ERROR_HANDLER;
        return 1;
    }
    catch(const std::exception& e)
    {
        ERROR_HANDLER;
        return handleError(e);
    }

    // Encode
    try
    {
        encoder = Encoder::getEncoder(context, arch, bitMode, parser);
        if (!encoder)
            throw Exception::InternalError("Couldn't get encoder", -1, -1);

        encoder->Encode();
        encoder->Optimize();
        if (debug)
            encoder->Print();

        if (warningManager.hasWarnings())
        {
            warningManager.printAll(std::cerr);
            warningManager.clear();
        }
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        ERROR_HANDLER;
        return 1;
    }
    catch(const std::exception& e)
    {
        ERROR_HANDLER;
        return handleError(e);
    }

    // Create .o/.bin file
    try
    {
        outputWriter = Output::getWriter(context, arch, bitMode, format, objectFile, parser, encoder);
        if (!outputWriter)
            throw Exception::InternalError("Couldn't get outputWriter", -1, -1);

        outputWriter->Write();
        if (debug)
            outputWriter->Print();

        if (warningManager.hasWarnings())
        {
            warningManager.printAll(std::cerr);
            warningManager.clear();
        }
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        ERROR_HANDLER;
        return 1;
    }
    catch(const std::exception& e)
    {
        ERROR_HANDLER;
        return handleError(e);
    }

    // cleanup
    CLEANUP;
}