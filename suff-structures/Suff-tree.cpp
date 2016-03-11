#include <map>
#include <algorithm>
#include <cstdio>
#include <iostream>


//positions are characterized by the vertex and how many
//levels we have to jump up
struct Position {
  int32_t vertex, len_up;
  Position(int32_t v, int32_t up) {
    this->vertex = v;
    this->len_up = up;
  }
};

//Node of suffix tree
struct Node {
  int32_t parent, left, right, link; //[left, right)
  std::map<unsigned char, int32_t> next;
  Node(int32_t l, int32_t r, int32_t p, int32_t suflink = -1) {
    this->parent = p;
    this->left = l;
    this->right = r; 
    this->link = suflink;
  }
};

class suff_tree {
public:
  suff_tree(std::string& str) {
    s = str;
    n = str.size();

    //vertices cant be more than 2 * n.
    for (size_t i = 0; i < 2 * n; ++i) {
      num_of_lists.push_back(0);
      sum_str.push_back(0);
    }
    cur = 1;
    Position pos(0, 0);
    st.push_back(Node(0, 0, -1));
    for (size_t i = 0; i < n; ++i) {
      pos = extend_ukkonen(pos, i);
    }
  }

  ~suff_tree() {
    sum_str.clear();
    num_of_lists.clear();
    st.clear();
    s.clear();
  }

  void solve() {
    dfs(0);
    for (size_t i = 1; i < st.size(); ++i) {
      std::cout << s.substr(st[i].right - sum_str[i], sum_str[i]) << " " << num_of_lists[i] << std::endl;
    }
  }


private:
  //states of a suffix tree
  std::vector<Node> st;
  std::string s; //string

  //number of lists and sum of a string to this vertex
  std::vector<int32_t> num_of_lists;
  std::vector<int32_t> sum_str;

  //cur --- how many vertices are now into the suffix tree
  //n --- size of a suffix tree
  int32_t cur;
  size_t n;

  //length of an edge
  int32_t get_length(int32_t v) {
    return (st[v].right - st[v].left);
  }

  //we need to go as much as we can
  Position go(Position pos, int32_t l, int32_t r) {
    while (true) {

      //if left == right => return
      if (l == r) {
        return pos;
      }

      //i made it as it was in the instruction at neerc ifmo
      int32_t stv = pos.vertex;
      if (pos.len_up == get_length(stv)) {
        if (st[stv].next.count(s[l]) == 1) {
          pos = Position(st[stv].next[s[l]], 0);
        } else {
          pos = Position(-1, 0);
        }
        if (pos.vertex == -1) {
          return pos;
        }
      } else {
        if (s[st[stv].left + pos.len_up] != s[l]) {
          return Position(-1, -1);
        }
        if (r - l < get_length(stv) - pos.len_up) {
          return Position(stv, pos.len_up + r - l);
        }
        l += get_length(stv) - pos.len_up;
        pos.len_up = get_length(stv);
      } 
    }
  }

  int32_t split(Position pos) {
    if (pos.len_up == get_length(pos.vertex)) {
      return pos.vertex;
    }
    if (pos.len_up == 0) {
      return st[pos.vertex].parent;
    }
    Node v = st[pos.vertex];
    ++cur;
    st.push_back(Node(v.left, v.left + pos.len_up, v.parent));
    st[v.parent].next[s[v.left]] = cur - 1;
    st[cur - 1].next[s[v.left + pos.len_up]] = pos.vertex;
    st[pos.vertex].parent = cur - 1;
    st[pos.vertex].left += pos.len_up;
    return cur - 1;
  }

  int32_t get_link(int32_t v) {
    if (st[v].link != -1) {
      return st[v].link;
    }
    if (st[v].parent == -1) {
      return 0;
    }
    int32_t to = get_link(st[v].parent);
    int32_t returned = split(go(Position(to, get_length(to)), st[v].left + (st[v].parent == 0 ? 1 : 0), st[v].right));
    st[v].link = returned;
    return returned;
  }

  Position extend_ukkonen(Position ptr, int32_t i) {
    while (true) {
      //almost nullptr, but it is not nullptr in some cases
      Position nulptr = go(ptr, i, i + 1);

      //if we can go, we go
      if (nulptr.vertex != -1) {
        return nulptr;
      }

      //we should split the edge
      int32_t mid = split(ptr);

      //append leaf
      int32_t leaf = cur;
      ++cur;
      st.push_back(Node(i, n, mid));

      //make a pointer to the leaf
      st[mid].next[s[i]] = leaf;

      //update ptr
      ptr.vertex = get_link(mid);
      ptr.len_up = get_length(ptr.vertex);

      //if we want to split the 0-vert edge, return
      if (mid == 0) {
        return ptr;
      }
    }
  }

  void dfs(int32_t v) {

    sum_str[v] += sum_str[st[v].parent] + get_length(v);

    //because of the unread symbol
    if (static_cast<size_t>(st[v].right) == n) {
      --sum_str[v];
    }

    //if list => num_of_lists = 1
    if (st[v].next.size() == 0) {
      num_of_lists[v] = 1;
    }
    for (const auto& it : st[v].next) {
      dfs(it.second);
      //add num_of_lists of a child
      num_of_lists[v] += num_of_lists[it.second];
    }
  }
};

int main() {
  std::ios_base::sync_with_stdio(0);
  freopen("input.txt", "r", stdin);
  freopen("output.txt", "w", stdout);
  std::string str;
  std::cin >> str;
  str.push_back('$');
  //build tree
  suff_tree tree(str);

  //solution of find in a a refrain
  tree.solve();
  return 0;
}
