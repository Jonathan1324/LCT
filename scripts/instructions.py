from dataclasses import dataclass, field
from enum import Enum, auto
from typing import List

class Flag(Enum):
    CF = auto()
    PF = auto()
    AF = auto()
    ZF = auto()
    SF = auto()
    OF = auto()

@dataclass
class Instruction:
    mnemonic: str
    name: str
    description: str
    asm: List[str]
    hex: List[str]
    affected_flags: List[Flag] = field(default_factory=list)

    def to_markdown(self, pad: int = 4) -> str:
        max_hex_len = max(len(h) for h in self.hex)
        lines = [
            f"### {self.mnemonic} - {self.name}",
            "",
            f"{self.description}",
            ""
        ]

        if self.affected_flags:
            flag_list = ", ".join(flag.name for flag in self.affected_flags)
            lines.append(f"Affected flags: {flag_list}")
            lines.append("")
        
        lines.append("```hex")

        for hex, asm in zip(self.hex, self.asm):
            spacing = " " * (max_hex_len - len(hex) + pad)
            lines.append(f"{hex}{spacing}; {asm}")

        lines.append("```")

        lines.append("")

        return "\n".join(lines)

ASCII_Instruction = [
    Instruction(
        mnemonic="AAA",
        name="ASCII Adjust After Addition",
        description="Adjusts AL after adding two ASCII digits to form a valid BCD result.",
        asm=["aaa"],
        hex=["37"],
        affected_flags=[Flag.AF, Flag.CF]
    ),
    Instruction(
        mnemonic="AAD",
        name="ASCII Adjust AX Before Division",
        description="Adjusts AX by converting two unpacked BCD digits in AH and AL into a binary number in AL, clearing AH before a division.",
        asm=["aad", "aad <imm8>"],
        hex=["D5 0A", "D5 <imm8>"],
        affected_flags=[Flag.SF, Flag.ZF, Flag.PF]
    ),
    Instruction(
        mnemonic="AAM",
        name="ASCII Adjust AX After Multiply",
        description="Adjusts AX after multiplying two unpacked BCD digits to form unpacked BCD digits in AH and AL.",
        asm=["aam", "aam <imm8>"],
        hex=["D4 0A", "D4 <imm8>"],
        affected_flags=[Flag.SF, Flag.ZF, Flag.PF]
    ),
    Instruction(
        mnemonic="AAS",
        name="ASCII Adjust AL After Subtraction",
        description="Adjusts AL after subtracting two ASCII digits to form a valid BCD result.",
        asm=["aas"],
        hex=["3F"],
        affected_flags=[Flag.AF, Flag.CF]
    )
]

ALU_Instruction = [
    Instruction(
        mnemonic="ADC",
        name="Add With Carry",
        description="Adds the destination operand, the source operand, and the carry flag and stores the result in the destination operand",
        asm=[
            "adc al, <imm8>",
            "adc ax|eax|rax, <imm16|32>",
            "adc r/m8, imm8",
            "adc r/m16|32|64, <imm16|32>",
            "adc r/m16|32|64, <imm8>",
            "adc r/m8, r8",
            "adc r/m16|32|64, r16|32|64",
            "adc r8, r/m8",
            "adc r16|32|64, r/m16|32|64"
        ],
        hex=[
            "14 <imm8>",
            "(66) (REX.W) 15 <imm16|32>",
            "80 <ModR/M /2> <imm8>",
            "(66) (REX.W) 81 <ModR/M /2> <imm16|32>",
            "(66) (REX.W) 83 <ModR/M /2> <imm8>",
            "10 <ModR/M>",
            "(66) (REX.W) 11 <ModR/M>",
            "12 <ModR/M>",
            "(66) (REX.W) 13 <ModR/M>"
        ],
        affected_flags=[Flag.OF, Flag.SF, Flag.ZF, Flag.AF, Flag.CF, Flag.PF]
    ),
    Instruction(
        mnemonic="ADCX",
        name="Unsigned Integer Addition of Two Operands With Carry Flag",
        description="Adds the destination operand, the source operand, and the carry flag and stores the result in the destination operand",
        asm=[
            "adcx r32|64, r/m32|64"
        ],
        hex=[
            "66 (REX.W) 0F 38 F6 <ModR/M>"
        ],
        affected_flags=[Flag.CF]
    ),
    Instruction(
        mnemonic="ADD",
        name="Add",
        description="Adds the destination operand and the source operand and stores the result in the destination operand",
        asm=[
            "add al, <imm8>",
            "add ax|eax|rax, <imm16|32>",
            "add r/m8, imm8",
            "add r/m16|32|64, <imm16|32>",
            "add r/m16|32|64, <imm8>",
            "add r/m8, r8",
            "add r/m16|32|64, r16|32|64",
            "add r8, r/m8",
            "add r16|32|64, r/m16|32|64"
        ],
        hex=[
            "04 <imm8>",
            "(66) (REX.W) 05 <imm16|32>",
            "80 <ModR/M /0> <imm8>",
            "(66) (REX.W) 81 <ModR/M /0> <imm16|32>",
            "(66) (REX.W) 83 <ModR/M /0> <imm8>",
            "00 <ModR/M>",
            "(66) (REX.W) 01 <ModR/M>",
            "02 <ModR/M>",
            "(66) (REX.W) 03 <ModR/M>"
        ],
        affected_flags=[Flag.OF, Flag.SF, Flag.ZF, Flag.AF, Flag.CF, Flag.PF]
    ),
]

md_lines = ["# Instructions", ""]

md_lines.extend(["## ASCII", ""])
for instr in ASCII_Instruction:
    md_lines.append(instr.to_markdown())

md_lines.extend(["## ALU", ""])
for instr in ALU_Instruction:
    md_lines.append(instr.to_markdown())

markdown_text = "\n".join(md_lines)

with open("docs/lasm/x86.md", "w") as f:
    f.write(markdown_text)
