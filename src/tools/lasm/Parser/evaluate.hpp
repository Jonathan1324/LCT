#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>
#include <StringPool.hpp>

uint64_t evalInteger(StringPool::String str, size_t size, int64_t lineNumber, int64_t column);
