#include <stdio.h>
#include <string.h>
#include <version.h>
#include <stdlib.h>
#include <ctype.h>
#include "tape.h"
#include "interpreter.h"
#include "compiler.h"

void printHelp(const char* programName)
{
    fprintf(stdout, "Usage: %s <input.bf>\n", programName);
}

int main(int argc, const char *argv[])
{
    int transpile = 0;
    const char* inputFile = NULL;
    const char* outputFile = NULL;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printHelp(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            printVersion();
            return 0;
        }
        else if (strcmp(argv[i], "-c") == 0)
        {
            transpile = 1;
        }
        else if (strcmp(argv[i], "-o") == 0)
        {
            if (i + 1 >= argc)
            {
                printHelp(argv[0]);
                return 1;
            }
            outputFile = argv[++i];
        }
        else
        {
            inputFile = argv[i];
        }
    }

    if (!inputFile)
    {
        fprintf(stderr, "Usage: %s [-c] <input.bf> [-o output.bf]\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(inputFile, "r");
    if (!f)
    {
        perror("File open");
        return 1;
    }

    size_t capacity = 1024;
    uint64_t rawProgramSize = 0;
    char *rawProgram = malloc(capacity);
    if (!rawProgram)
    {
        perror("malloc");
        fclose(f);
        return 1;
    }

    char c;
    int inComment = 0;
    uint64_t line = 1;
    while ((c = (char)fgetc(f)) != EOF)
    {
        if (c == '\n') line++;

        if (inComment != 0)
        {
            if (c == '\n') inComment = 0;
            continue;
        }

        if (c == '#')
        {
            inComment = 1;
            continue;
        }

        if (rawProgramSize >= capacity)
        {
            capacity *= 2;
            char *tmp = realloc(rawProgram, capacity);
            if (!tmp)
            {
                perror("realloc");
                free(rawProgram);
                fclose(f);
                return 1;
            }
            rawProgram = tmp;
        }

        switch (c)
        {
            case '>': case '<': case '+': case '-':
            case '.': case ',': case '[': case ']':
            case '=': case '0': case '1': case '2':
            case '3': case '4': case '5': case '6':
            case '7': case '8': case '9':
                rawProgram[rawProgramSize] = (char)c;
                rawProgramSize++;
                break;

            case ' ':
            case '\t':
            case '\r':
            case '\n':
                break;

            default:
                /*
                fprintf(stderr, "Error: Unknown char '%c' at line %zu\n", c, line);
                free(rawProgram);
                fclose(f);
                return 1;
                */
                break;
        }
    }
    fclose(f);

    // Optimize
    uint64_t programSize = 0;
    for (uint64_t i = 0; i < rawProgramSize; i++)
    {
        c = rawProgram[i];
        if (c == '[' || c == ']' || c == '.' || c == ',')
        {
            programSize++;
        }
        else if (c == '+' || c == '-' || c == '<' || c == '>')
        {
            programSize++;
            while (i + 1 < rawProgramSize && rawProgram[i + 1] == c)
                i++;
        }
        else if (c == '=')
        {
            programSize++;
            if (i + 1 > rawProgramSize)
            {
                fprintf(stderr, "Error: No digit after '=' at line %zu\n", line);
                free(rawProgram);
                fclose(f);
                return 1;
            }
            while (i + 1 < rawProgramSize && isdigit(rawProgram[i + 1]))
                i++;
        }
    }

    char* program = malloc(programSize);
    uint64_t* meta = malloc(programSize * sizeof(uint64_t));

    uint64_t* stack = malloc(programSize * sizeof(uint64_t));
    uint64_t sp = 0;

    uint64_t index = 0;
    for (uint64_t i = 0; i < rawProgramSize; i++)
    {
        c = rawProgram[i];
        if (c == '[')
        {
            program[index] = '[';
            stack[sp++] = index;
        }
        else if (c == ']')
        {
            program[index] = ']';
            if (sp == 0)
            {
                fputs("Unmatched ']'", stderr);
                free(rawProgram);
                free(stack);
                free(meta);
                free(program);
                return 1;
            }
            uint64_t j = stack[--sp];
            meta[index] = j;
            meta[j] = index;
        }
        else if (c == '.' || c == ',')
        {
            program[index] = c;
        }
        else if (c == '<' || c == '>' || c == '+' || c == '-')
        {
            uint64_t count = 1;
            while (i + 1 < rawProgramSize && rawProgram[i + 1] == c)
            {
                count++;
                i++;
            }
            program[index] = c;
            meta[index] = count;
        }
        else if (c == '=')
        {
            uint64_t value = 0;
            while (i + 1 < rawProgramSize && isdigit(rawProgram[i + 1]))
            {
                value *= 10;
                value += (unsigned char)rawProgram[i + 1] - (unsigned char)'0';
                i++;
            }
            program[index] = c;
            meta[index] = value;
        }
        else
        {
            // TODO
        }
        index++;
    }
    free(rawProgram);
    free(stack);
    if (sp != 0)
    {
        fputs("Unmatched '['", stderr);
        free(meta);
        free(program);
        return 1;
    }

    if (transpile)
    {
        FILE* out = fopen(outputFile, "w");
        if (!out)
        {
            perror("Output file open");
            free(meta);
            free(program);
            return 1;
        }
        
        compile(program, meta, programSize, out, 0);

        fclose(out);
    }
    else
    {
        interpret(program, meta, programSize);
    }
    
    free(program);
    free(meta);
    return 0;
}