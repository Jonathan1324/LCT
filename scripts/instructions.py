instructions = [
#    {
#        "mnemonic": "AAA",
#        "name": "ASCII Adjust After Addition",
#        "description": "Adjusts AL after adding two ASCII digits to form a valid BCD result.",
#        "asm": ["aaa"],
#        "hex": ["37"]
#    },
#    {
#        "mnemonic": "AAD",
#        "name": "ASCII Adjust AX Before Division",
#        "description": "Adjusts AX by converting two unpacked BCD digits in AH and AL into a binary number in AL, clearing AH before a division.",
#        "asm": ["aad", "aad <imm8>"],
#        "hex": ["D5 0A", "D5 <imm8>"]
#    }
]

def generate_markdown(instructions):
    md = ["# Instructions\n"]
    for instr in instructions:
        md.append(f"## {instr['mnemonic']} – {instr['name']}\n")
        md.append(f"{instr['description']}\n")

        max_len = max(len(a) for a in instr['asm'])
        pad = 4

        md.append("```hex")
        for h, a in zip(instr['hex'], instr['asm']):
            padding = " " * (max_len - len(h) + pad)
            md.append(f"{h}{padding}; {a}")
        md.append("```\n")
    return "\n".join(md)

markdown_text = generate_markdown(instructions)
with open("docs/lasm/x86.md", "w") as f:
    f.write(markdown_text)
