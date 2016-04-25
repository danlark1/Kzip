#include "Suffix_tree.h"
#include <unordered_map>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <utility>
#include <iostream>

suff_tree::suff_tree(const std::string& str) {
  s = str;
  n = str.size();

  //  vertices cant be more than 2 * n.
  for (size_t i = 0; i < 2 * n; ++i) {
    num_of_lists.push_back(0);
    sum_str.push_back(0);
  }
  cur = 1;
  Position pos(0, 0);
  st.push_back(Node_s(0, 0, -1));
  for (size_t i = 0; i < n; ++i) {
    pos = extend_ukkonen(pos, i);
  }
}

suff_tree::~suff_tree() {
  sum_str.clear();
  sum_str.shrink_to_fit();
  num_of_lists.clear();
  num_of_lists.shrink_to_fit();
  st.clear();
  st.shrink_to_fit();
  s.clear();
  s.shrink_to_fit();
}

std::vector<std::pair<std::string, int64_t> > suff_tree::find_substr() {
  dfs(0);
  std::vector<std::pair<int32_t, int32_t> > p;
  for (size_t i = 1; i < st.size(); ++i) {
    p.push_back({num_of_lists[i], i});
  }
  stable_sort(p.begin(), p.end());
  int32_t k = 0, i = p.size() - 1;
  int j = 0;
  bool flag = false;
  std::vector<std::pair<std::string, int64_t> > ans;
  while (j <= 5000 && i >= 0 && k <= 100000) {
    flag = false;
    for (int32_t pop = st[p[i].second].right - sum_str[p[i].second]; pop < st[p[i].second].right; ++pop) {
      if (s[pop] == '\0') {
        flag = true;
        break;
      }
    }
    if (flag) {
      --i;
      continue;
    }
    ++j;
    k += sum_str[p[i].second];
    ans.push_back({s.substr(st[p[i].second].right - sum_str[p[i].second],
      sum_str[p[i].second]), num_of_lists[p[i].second]});
    --i;
  }
  return ans;
}
// length of an edge
int32_t suff_tree::get_length(const int32_t v) const {
  return (st[v].right - st[v].left);
}

// we need to go as much as we can
Position suff_tree::go(Position pos, int32_t l, int32_t r) {
  while (true) {
    // if left == right => return
    if (l == r) {
      return pos;
    }

    // i made it as it was in the instruction at neerc ifmo
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

int32_t suff_tree::split(const Position pos) {
  if (pos.len_up == get_length(pos.vertex)) {
    return pos.vertex;
  }
  if (pos.len_up == 0) {
    return st[pos.vertex].parent;
  }
  Node_s v = st[pos.vertex];
  ++cur;
  st.push_back(Node_s(v.left, v.left + pos.len_up, v.parent));
  st[v.parent].next[s[v.left]] = cur - 1;
  st[cur - 1].next[s[v.left + pos.len_up]] = pos.vertex;
  st[pos.vertex].parent = cur - 1;
  st[pos.vertex].left += pos.len_up;
  return cur - 1;
}

int32_t suff_tree::get_link(const int32_t v) {
  if (st[v].link != -1) {
    return st[v].link;
  }
  if (st[v].parent == -1) {
    return 0;
  }
  int32_t to = get_link(st[v].parent);
  int32_t returned = split(go(Position(to, get_length(to)), st[v].left +
    (st[v].parent == 0 ? 1 : 0), st[v].right));
  st[v].link = returned;
  return returned;
}

Position suff_tree::extend_ukkonen(Position ptr, const int32_t i) {
  while (true) {
    // almost nullptr, but it is not nullptr in some cases
    Position nulptr = go(ptr, i, i + 1);

    // if we can go, we go
    if (nulptr.vertex != -1) {
      return nulptr;
    }

    // we should split the edge
    int32_t mid = split(ptr);

    // append leaf
    int32_t leaf = cur;
    ++cur;
    st.push_back(Node_s(i, n, mid));

    // make a pointer to the leaf
    st[mid].next[s[i]] = leaf;

    // update ptr
    ptr.vertex = get_link(mid);
    ptr.len_up = get_length(ptr.vertex);

    // if we want to split the 0-vert edge, return
    if (mid == 0) {
      return ptr;
    }
  }
}

void suff_tree::dfs(int32_t v) {
  sum_str[v] += sum_str[st[v].parent] + get_length(v);

  // because of the unread symbol
  if (static_cast<size_t>(st[v].right) == n) {
    --sum_str[v];
  }

  // if list => num_of_lists = 1
  if (st[v].next.size() == 0) {
    num_of_lists[v] = 1;
  }

  for (const auto& it : st[v].next) {
    dfs(it.second);
    // add num_of_lists of a child
    num_of_lists[v] += num_of_lists[it.second];
  }
}
