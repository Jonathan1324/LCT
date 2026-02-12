#pragma once

#include <cstdint>

namespace x86
{
    enum Instructions : uint64_t
    {
        // CONTROL
        NOP,
        HLT,

        // INTERRUPT
        INT, IRET, IRETQ, IRETD,
        SYSCALL, SYSRET,
        SYSENTER, SYSEXIT,

        // FLAGS
        CLC, STC, CMC,
        CLD, STD,
        CLI, STI,
        LAHF, SAHF,

        // STACK
        PUSH, POP,
        PUSHA, POPA,
        PUSHAD, POPAD,
        PUSHF, POPF,
        PUSHFD, POPFD,
        PUSHFQ, POPFQ,

        // DATA
        MOV,

        // ALU
        ADD, ADC, SUB, SBB, CMP, TEST, AND, OR, XOR,
        MUL, IMUL, DIV, IDIV,

        SHL, SHR, SAL, SAR,
        ROL, ROR, RCL, RCR,

        NOT, NEG, INC, DEC,

        // CONTROL FLOW
        // TODO
        JMP, JE, JNE, JG, JGE, JL, JLE,
        JA, JAE, JB, JBE,
        CALL, RET
    };
}
