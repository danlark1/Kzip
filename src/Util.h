// Copyright 2017-2018, Danila Kutenin
#pragma once

#include <cinttypes>
#include <climits>

#include <string>
#include <utility>

static constexpr size_t CHAR_SIZE = (sizeof(char) * CHAR_BIT);
static constexpr size_t CHAR_SIZE_POWER = 1 << CHAR_SIZE;
static constexpr size_t MAX_CONCAT_SIZE = 1e6;

struct KeyStr {
    size_t size;
    std::string str;
    bool operator==(const KeyStr& other) const {
        return std::pair(size, str) == std::pair(other.size, other.str);
    }
    bool operator<(const KeyStr& other) const {
        return std::pair(size, str) < std::pair(other.size, other.str);
    }
};

struct ValueStr {
    int64_t occur;
    bool taken;
};
