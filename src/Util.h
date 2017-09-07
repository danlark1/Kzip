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
    return size == other.size && str == other.str;
  }
  bool operator<(const KeyStr& other) const {
    return size < other.size || (size == other.size && str < other.str);
  }
};

struct ValueStr {
  int64_t occur;
  bool taken;
};

