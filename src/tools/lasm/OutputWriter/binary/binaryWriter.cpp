#include "BinaryWriter.hpp"
#include <limits>
#include <memory>
#include <cstring>
#include <algorithm>

Binary::Writer::Writer(const Context &_context, Architecture _arch, BitMode _bits, Format _format, std::ostream *_file, const Parser::Parser *_parser, const Encoder::Encoder *_encoder)
    : ::Output::Writer::Writer(_context, _arch, _bits, _format, _file, _parser, _encoder)
{
}

void Binary::Writer::Write()
{
    std::vector<Encoder::Section> sections = encoder->getSections();

    auto itText = std::find_if(sections.begin(), sections.end(),
                           [](const Encoder::Section &s){ return s.name == ".text"; });
    if (itText != sections.end() && itText != sections.begin()) {
        std::rotate(sections.begin(), itText, itText + 1);
    }

    std::vector<Encoder::Section> uninitialized;

    std::unordered_map<std::string, uint64_t> sectionOffsets;
    std::unordered_map<std::string, std::vector<uint8_t>*> sectionBuffers;

    uint64_t off = 0;
    for (Encoder::Section& section : sections)
    {
        if (section.align == 0)
            throw Exception::InternalError(std::string("Alignment not set for section '") + section.name.c_str() + "'", -1, -1);

        uint64_t align = section.align;
        if (align < 4) align = 4; // TODO: minimal alignment of 4, check if useful

        off = (off + align - 1) / align * align;

        sectionBuffers[section.name.c_str()] = &section.buffer;

        if (!section.isInitialized)
        {
            uninitialized.push_back(section);
            continue;
        }
        
        sectionOffsets[section.name.c_str()] = off;

        off += static_cast<uint64_t>(section.size());
    }
    for (const Encoder::Section& section : uninitialized)
    {
        uint64_t align = section.align;
        if (align < 4) align = 4; // TODO: minimal alignment of 4, check if useful

        off = (off + align - 1) / align * align;

        sectionOffsets[section.name.c_str()] = off;

        off += static_cast<uint64_t>(section.size());
    }

    for (const Encoder::Relocation& relocation : encoder->getRelocations())
    {
        if (relocation.isExtern) throw Exception::SemanticError("Can't use external labels with binary output", -1, -1);

        auto itBuf = sectionBuffers.find(relocation.section.c_str());
        if (itBuf == sectionBuffers.end()) throw Exception::InternalError("Section wasn't found", -1, -1);
        std::vector<uint8_t>* sectionBuffer = itBuf->second;

        auto it = sectionOffsets.find(relocation.usedSection.c_str());
        if (it == sectionOffsets.end()) throw Exception::InternalError("Used section wasn't found", -1, -1);
        const uint64_t& sectionOffset = it->second;
        const int64_t value = sectionOffset + relocation.addend;

        auto itRelocationSection = sectionOffsets.find(relocation.section.c_str());
        if (itRelocationSection == sectionOffsets.end()) 
            throw Exception::InternalError("Relocation section offset not found", -1, -1);
        const uint64_t relocationSectionOffset = itRelocationSection->second;

        const uint64_t& offset = relocation.offsetInSection;
        
        switch (relocation.type)
        {
            case Encoder::RelocationType::PC_Relative:
            {
                const int64_t P = static_cast<int64_t>(relocationSectionOffset + offset);
                const int64_t patchedValue = value - P;

                switch (relocation.size)
                {
                    case Encoder::RelocationSize::Bit8:
                    {
                        if (patchedValue < std::numeric_limits<int8_t>::min() ||
                            patchedValue > std::numeric_limits<int8_t>::max())
                            throw Exception::OverflowError("Relocation would overflow", -1, -1);

                        const int8_t val = static_cast<int8_t>(patchedValue);
                        std::memcpy(sectionBuffer->data() + offset, &val, sizeof(uint8_t));
                        break;
                    }
                    case Encoder::RelocationSize::Bit16:
                    {
                        if (patchedValue < std::numeric_limits<int16_t>::min() ||
                            patchedValue > std::numeric_limits<int16_t>::max())
                            throw Exception::OverflowError("Relocation would overflow", -1, -1);

                        const int16_t val = static_cast<int16_t>(patchedValue);
                        std::memcpy(sectionBuffer->data() + offset, &val, sizeof(uint16_t));
                        break;
                    }
                    case Encoder::RelocationSize::Bit24:
                    {
                        constexpr int32_t min24 = -(1 << 23);
                        constexpr int32_t max24 =  (1 << 23) - 1;

                        if (patchedValue < min24 || patchedValue > max24)
                            throw Exception::OverflowError("Relocation would overflow (24-bit)", -1, -1);

                        int32_t val = static_cast<int32_t>(patchedValue);

                        uint8_t bytes[3];
                        bytes[0] = static_cast<uint8_t>(val & 0xFF);
                        bytes[1] = static_cast<uint8_t>((val >> 8) & 0xFF);
                        bytes[2] = static_cast<uint8_t>((val >> 16) & 0xFF);

                        std::memcpy(sectionBuffer->data() + offset, bytes, 3);
                        break;
                    }
                    case Encoder::RelocationSize::Bit32:
                    {
                        if (patchedValue < std::numeric_limits<int32_t>::min() ||
                            patchedValue > std::numeric_limits<int32_t>::max())
                            throw Exception::OverflowError("Relocation would overflow (32-bit)", -1, -1);

                        const int32_t val = static_cast<int32_t>(patchedValue);
                        std::memcpy(sectionBuffer->data() + offset, &val, sizeof(uint32_t));
                        break;
                    }
                    case Encoder::RelocationSize::Bit64:
                    {
                        if (patchedValue < std::numeric_limits<int64_t>::min() ||
                            patchedValue > std::numeric_limits<int64_t>::max())
                            throw Exception::OverflowError("Relocation would overflow (64-bit)", -1, -1);

                        const int64_t val = static_cast<int64_t>(patchedValue);
                        std::memcpy(sectionBuffer->data() + offset, &val, sizeof(uint64_t));
                        break;
                    }
                }
                break;
            }

            case Encoder::RelocationType::Absolute:
            {
                switch (relocation.size)
                {
                    case Encoder::RelocationSize::Bit8:
                    {
                        // TODO
                        //if (value < std::numeric_limits<int8_t>::min() ||
                        //    value > std::numeric_limits<int8_t>::max())
                        //    throw Exception::OverflowError("Relocation would overflow", -1, -1);
                        
                        const int8_t val = static_cast<int8_t>(value);
                        std::memcpy(sectionBuffer->data() + offset, &val, sizeof(uint8_t));
                        break;
                    }
                    case Encoder::RelocationSize::Bit16:
                    {
                        // TODO
                        //if (value < std::numeric_limits<int16_t>::min() ||
                        //    value > std::numeric_limits<int16_t>::max())
                        //    throw Exception::OverflowError("Relocation would overflow", -1, -1);

                        const int16_t val = static_cast<int16_t>(value);
                        std::memcpy(sectionBuffer->data() + offset, &val, sizeof(uint16_t));
                        break;
                    }
                    case Encoder::RelocationSize::Bit24:
                    {
                        // TODO
                        //constexpr int32_t min24 = -(1 << 23);      // -8388608
                        //constexpr int32_t max24 =  (1 << 23) - 1;  //  8388607
                        //
                        //if (value < min24 || value > max24)
                        //    throw Exception::OverflowError("Relocation would overflow (24-bit)", -1, -1);

                        int32_t val = static_cast<int32_t>(value);

                        // Little-endian
                        uint8_t bytes[3];
                        bytes[0] = static_cast<uint8_t>(val & 0xFF);
                        bytes[1] = static_cast<uint8_t>((val >> 8) & 0xFF);
                        bytes[2] = static_cast<uint8_t>((val >> 16) & 0xFF);

                        std::memcpy(sectionBuffer->data() + offset, bytes, 3);
                        break;
                    }
                    case Encoder::RelocationSize::Bit32:
                    {
                        // TODO
                        //if (value < std::numeric_limits<int32_t>::min() ||
                        //    value > std::numeric_limits<int32_t>::max())
                        //    throw Exception::OverflowError("Relocation would overflow (32-bit)", -1, -1);

                        const int32_t val = static_cast<int32_t>(value);
                        std::memcpy(sectionBuffer->data() + offset, &val, sizeof(uint32_t));
                        break;
                    }
                    case Encoder::RelocationSize::Bit64:
                    {
                        // TODO
                        //if (value < std::numeric_limits<int64_t>::min() ||
                        //    value > std::numeric_limits<int64_t>::max())
                        //    throw Exception::OverflowError("Relocation would overflow (64-bit)", -1, -1);

                        const int64_t val = static_cast<int64_t>(value);
                        std::memcpy(sectionBuffer->data() + offset, &val, sizeof(uint64_t));
                        break;
                    }
                }
                break;
            }

            default: throw Exception::InternalError("Unknown relocation type", -1, -1); break;
        }
    }

    // TODO
    for (const Encoder::Section& section : sections)
    {
        if (section.buffer.empty()) continue;

        if (section.align == 0)
            throw Exception::InternalError(std::string("Alignment not set for section '") + section.name.c_str() + "'", -1, -1);

        if (!section.isInitialized) continue;

        std::streampos pos = file->tellp();
        if (pos == -1)
            throw Exception::IOError("Failed to get file position", -1, -1);

        uint64_t offset = static_cast<uint64_t>(pos);

        uint64_t align = section.align;
        if (align < 4) align = 4; // TODO: minimal alignment of 4, check if useful

        uint64_t padding = (align - (offset % align)) % align;

        if (padding > 0)
        {
            std::vector<char> pad(padding, 0);
            file->write(pad.data(), pad.size());
        }

        file->write(reinterpret_cast<const char *>(section.buffer.data()), section.buffer.size());
    }

    for (const Encoder::Section& section : uninitialized)
    {
        // TODO
        (void)section;
    }
}
