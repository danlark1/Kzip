// Copyright 2017, Danila Kutenin

#include "Node.h"

#include <algorithm>
#include <string>

Node::Node()
    : left(nullptr)
    , right(nullptr)
    , frequency(0)
{
}

Node::Node(const std::string& Data, const int64_t freq) {  // make a leaf
    data = std::move(Data);
    frequency = freq;
    left = nullptr;
    right = nullptr;
    std::fill(to_go.begin(), to_go.end(), std::make_pair("", nullptr));
}

Node::Node(Node* l, Node* r) {  // make a bind
    data.clear();
    data.shrink_to_fit();
    frequency = l->GetFrequency() + r->GetFrequency();
    left = l;
    right = r;
    std::fill(to_go.begin(), to_go.end(), std::make_pair("", nullptr));
}

Node::~Node() {
    if (left) {
        delete left;
    }
    if (right) {
        delete right;
    }
}

int64_t Node::GetFrequency() const {  // frequency of a node
    return frequency;
}

std::string Node::GetData() const {
    return data;
}
