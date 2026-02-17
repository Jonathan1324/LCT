#pragma once

#include <string>
#include <iostream>
#include <ostream>
#include <vector>
#include <cinttypes>

class Warning
{
public:
    enum class Type
    {
        None,
        Argument
    };

    Warning(Type _type, const std::string& _message, int64_t _line, int64_t _column);

    const char* what() const noexcept;
    Type getType() const noexcept;

    void print(std::ostream& os = std::cerr) const;


    static Warning GeneralWarning(const std::string& message, int64_t line = -1, int64_t column = -1);
    static Warning ArgumentWarning(const std::string& message, int64_t line = -1, int64_t column = -1);

private:
    Type type;
    std::string message;
    int64_t line;
    int64_t column;

    std::string typeToString() const;
};

class WarningManager
{
public:
    WarningManager();

    void add(const Warning& w);
    void printAll(std::ostream& os = std::cerr) const;
    bool hasWarnings() const;
    void clear();

private:
    std::vector<Warning> warnings_;
};
