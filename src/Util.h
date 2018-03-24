// Copyright 2017, Danila Kutenin
#pragma once

#include "Node.h"

#include <cinttypes>
#include <string>
#include <utility>

class Comp {
public:
    bool operator()(const std::pair<Node*, int64_t>& l, const std::pair<Node*, int64_t>& r) {
        if (l.first->GetFrequency() == r.first->GetFrequency()) {
            return r.second > l.second;
        }
        return l.first->GetFrequency() > r.first->GetFrequency();
    }
};

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
