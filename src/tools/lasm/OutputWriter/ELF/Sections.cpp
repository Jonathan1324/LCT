#include "ELFWriter.hpp"
#include "Headers.hpp"

uint64_t ELF::Writer::getSectionFlags(StringPool::String name)
{
    if (name == ".text")
        return SectionFlags::S_ALLOC | SectionFlags::S_EXECINSTR;
    else if (name == ".data")
        return SectionFlags::S_ALLOC | SectionFlags::S_WRITE;

    else if (name == ".bss")
        return SectionFlags::S_ALLOC | SectionFlags::S_WRITE;

    else if (name == ".rodata")
        return SectionFlags::S_ALLOC;

    else if (name == ".tdata")
        return SectionFlags::S_ALLOC | SectionFlags::S_WRITE | SectionFlags::S_TLS;

    else if (name == ".comment")
        return 0;

    return SectionFlags::S_ALLOC;
}

uint32_t ELF::Writer::getSectionType(StringPool::String name)
{
    if (name == ".bss")
        return SectionType::NoBits;
    else
        return SectionType::ProgBits; // TODO
}
