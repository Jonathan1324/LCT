#include "StringPool.hpp"

const char* StringPool::String::c_str() const
{
    return pool->storage[index].c_str();
}

uint64_t StringPool::String::size() const
{
    return pool->storage[index].size();
}

bool StringPool::String::operator==(const String& other) const
{
    return pool == other.pool && index == other.index;
}

bool StringPool::String::operator==(const std::string& other) const
{
    const String otherString = pool->GetString(other);
    return pool == otherString.pool && index == otherString.index;
}

bool StringPool::String::operator==(const char* other) const
{
    const String otherString = pool->GetString(other);
    return pool == otherString.pool && index == otherString.index;
}

bool StringPool::String::operator==(char other) const
{
    const String otherString = pool->GetString(other);
    return pool == otherString.pool && index == otherString.index;
}

StringPool::String StringPool::String::operator+(const String& other) const
{
    std::string combined = std::string(this->c_str()) + std::string(other.c_str());

    return pool->GetString(combined);
}

StringPool::String StringPool::String::operator+(const std::string& other) const
{
    std::string combined = std::string(this->c_str()) + other;

    return pool->GetString(combined);
}

StringPool::String StringPool::String::operator+(const char* other) const
{
    std::string combined = std::string(this->c_str()) + other;

    return pool->GetString(combined);
}

StringPool::String StringPool::String::operator+(char other) const
{
    std::string combined = std::string(this->c_str()) + other;

    return pool->GetString(combined);
}


StringPool::String& StringPool::String::operator+=(const String& other)
{
    std::string combined = std::string(this->c_str()) + std::string(other.c_str());

    *this = pool->GetString(combined);

    return *this;
}

StringPool::String& StringPool::String::operator+=(const std::string& other)
{
    std::string combined = std::string(this->c_str()) + other;

    *this = pool->GetString(combined);

    return *this;
}

StringPool::String& StringPool::String::operator+=(const char* other)
{
    std::string combined = std::string(this->c_str()) + other;

    *this = pool->GetString(combined);

    return *this;
}

StringPool::String& StringPool::String::operator+=(char other)
{
    std::string combined = std::string(this->c_str()) + other;

    *this = pool->GetString(combined);

    return *this;
}

char StringPool::String::operator[](uint64_t i) const
{
    return pool->storage[index][i];
}

StringPool::String StringPool::GetString(const std::string& str)
{
    auto it = map.find(str);
    if (it != map.end()) {
        return String{this, it->second};
    }
    uint64_t idx = storage.size();
    storage.push_back(str);
    map[str] = idx;
    return String{this, idx};
}

// TODO: Make better

StringPool::String StringPool::GetString(const char* str)
{
    return GetString(std::string(str));
}

StringPool::String StringPool::GetString(char c)
{
    return GetString(std::string() + c);
}
