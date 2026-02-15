from dataclasses import dataclass
from typing import List

@dataclass
class Instruction:
    mnemonic: str
    name: str
    description: str
    asm: List[str]
    hex: List[str]

    def to_markdown(self, pad: int = 4) -> str:
        max_hex_len = max(len(h) for h in self.hex)
        lines = [
            f"### {self.mnemonic} - {self.name}",
            "",
            f"{self.description}",
            ""
        ]
        
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
        hex=["37"]
    ),
    Instruction(
        mnemonic="AAD",
        name="ASCII Adjust AX Before Division",
        description="Adjusts AX by converting two unpacked BCD digits in AH and AL into a binary number in AL, clearing AH before a division.",
        asm=["aad", "aad <imm8>"],
        hex=["D5 0A", "D5 <imm8>"]
    ),
    Instruction(
        mnemonic="AAM",
        name="ASCII Adjust AX After Multiply",
        description="Adjusts AX after multiplying two unpacked BCD digits to form unpacked BCD digits in AH and AL.",
        asm=["aam", "aam <imm8>"],
        hex=["D4 0A", "D4 <imm8>"]
    ),
    Instruction(
        mnemonic="AAS",
        name="ASCII Adjust AL After Subtraction",
        description="Adjusts AL after subtracting two ASCII digits to form a valid BCD result.",
        asm=["aas"],
        hex=["3F"]
    )
]

ALU_Instruction = [
    Instruction(
        mnemonic="ADC",
        name="Add With Carry",
        description="Adds the destination operand, the source operand, and the carry flag and stores the result in the destination operand",
        asm=[
            "adc al, <imm8>",
            "adc ax/eax/rax, <imm16/32/64>"
        ],
        hex=[
            "14 <imm8>",
            "(66) (REX) 15 <imm16/32/64>"
        ]
    ),
]

md_lines = ["# Instructions", ""]
md_lines.extend(["## ASCII", ""])
for instr in ASCII_Instruction:
    md_lines.append(instr.to_markdown())

markdown_text = "\n".join(md_lines)

with open("docs/lasm/x86.md", "w") as f:
    f.write(markdown_text)
