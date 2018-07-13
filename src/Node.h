// Copyright 2017-2018, Danila Kutenin
#pragma once

#include "Util.h"

#include <array>
#include <string>

class Node {
public:
    Node* left;
    Node* right;
    std::array<std::pair<std::string, Node*>, 1 << CHAR_BIT> to_go;
    Node(const std::string&, const int64_t);
    Node(Node*, Node*);
    Node(const Node&) = default;
    Node(Node&&) = default;
    ~Node();
    Node();
    int64_t GetFrequency() const;
    std::string GetData() const;

private:
    int64_t frequency;
    std::string data;
};

class Comp {
public:
    bool operator()(const std::pair<Node*, int64_t>& l, const std::pair<Node*, int64_t>& r) {
        if (l.first->GetFrequency() == r.first->GetFrequency()) {
            return r.second > l.second;
        }
        return l.first->GetFrequency() > r.first->GetFrequency();
    }
};
