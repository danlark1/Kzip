#pragma once

// NEEDED LIBRARIES
#include <climits>
#include <string>
#include <vector>

enum Node_type { LEAF, BIND };
const size_t CHAR_SIZE = (sizeof(char) * CHAR_BIT);

class Node {
public:
  Node* left;
  Node* right;
  std::vector<std::pair<std::string, Node*> > to_go;
  Node(const std::string& d, const int64_t s, const Node_type = LEAF);  // Construct a list
  Node(Node* l, Node* r, Node_type = BIND);  // Construct a bind node
  ~Node();
  Node();
  int64_t GetFrequency() const;
  std::string GetData() const;
private:
  int64_t frequency;
  std::string data;
  Node_type type;
};
