#include "compiler.h"

void compile(char* program, uint64_t* meta, uint64_t programSize, FILE* out, int performance)
{
    (void)performance;
    for (uint64_t i = 0; i < programSize; i++)
    {
        char op = program[i];
        if (op == '+' || op == '-' || op == '<' || op == '>')
        {
            for (uint64_t j = 0; j < meta[i] % 256; j++)
            {
                fputc(op, out);
            }
        }
        else if (op == '.' || op == ',' || op == '[' || op == ']')
        {
            fputc(op, out);
        }
        else if (op == '=')
        {
            // TODO: Make better
            fputs("[-]", out);
            for (uint64_t j = 0; j < meta[i] % 256; j++)
            {
                fputc('+', out);
            }
        }
    }
}
