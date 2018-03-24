// Copyright 2017, Danila Kutenin
#pragma once

#include <climits>
#include <string>
#include <vector>

enum Node_type { LEAF, BIND };
static constexpr size_t CHAR_SIZE = (sizeof(char) * CHAR_BIT);

class Node {
public:
    Node* left;
    Node* right;
    std::vector<std::pair<std::string, Node*>> to_go;
    Node(const std::string&, const int64_t, const Node_type = LEAF);
    Node(Node*, Node*, Node_type = BIND);
    Node(const Node&) = default;
    Node(Node&&) = default;
    ~Node();
    Node();
    int64_t GetFrequency() const;
    std::string GetData() const;

private:
    int64_t frequency;
    std::string data;
    Node_type type;
};
