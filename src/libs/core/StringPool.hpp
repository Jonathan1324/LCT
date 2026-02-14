#pragma once

#include <vector>
#include <unordered_map>
#include <string_view>
#include <string>
#include <inttypes.h>
#include <Architecture.hpp>

class StringPool
{
public:
    struct String
    {
        StringPool* pool;
        uint64_t index;

        const char* c_str() const;
        uint64_t size() const;

        using value_type = char;
        using const_iterator = const char*;
        using iterator = const_iterator;

        iterator begin() const { return c_str(); }
        iterator end() const { return c_str() + size(); }

        const_iterator cbegin() const { return begin(); }
        const_iterator cend() const { return end(); }
        
        bool operator==(const String& other) const;
        bool operator==(const std::string& other) const;
        bool operator==(const char* other) const;
        bool operator==(char other) const;

        inline bool operator!=(const String& other) const { return !(*this == other); }
        inline bool operator!=(const std::string& other) const { return !(*this == other); }
        inline bool operator!=(const char* other) const { return !(*this == other); }
        inline bool operator!=(char other) const { return !(*this == other); }

        String operator+(const String& other) const;
        String& operator+=(const String& other);

        String operator+(const std::string& other) const;
        String& operator+=(const std::string& other);

        String operator+(const char* other) const;
        String& operator+=(const char* other);

        String operator+(char other) const;
        String& operator+=(char other);

        char operator[](uint64_t i) const;
    };

    String GetString(const std::string& str);
    String GetString(const char* str);
    String GetString(char c);

    inline String empty() { return GetString(""); }

private:
    std::unordered_map<std::string, uint64_t> map;
    std::vector<std::string> storage;
};

inline std::ostream& operator<<(std::ostream& os, const StringPool::String& s)
{
    return os << s.c_str();
}
