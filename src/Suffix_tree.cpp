// Copyright 2017, Danila Kutenin
#include "Suffix_tree.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <inttypes.h>

SuffTree::SuffTree(const std::string& str) {
    s = str;
    n = str.size();

    //  vertices cant be more than 2 * n.
    num_of_lists.resize(n << 1);
    sum_str.resize(n << 1);
    std::fill(num_of_lists.begin(), num_of_lists.end(), 0);
    std::fill(sum_str.begin(), sum_str.end(), 0);
    cur = 1;
    Position pos(0, 0);
    st.push_back(Node_s(0, 0, -1));
    for (size_t i = 0; i < n; ++i) {
        pos = ExtendUkkonen(pos, i);
    }
}

std::vector<std::pair<std::string, int64_t>> SuffTree::FindSubstrings(const size_t dict_size,
                                                                      const unsigned char min_char) {
    Dfs(0);
    std::vector<std::pair<int32_t, int32_t>> p;
    p.resize(st.size() - 1);
    std::generate(p.begin(), p.end(), [&] {
        static auto i_temp = 0;
        ++i_temp;
        return std::make_pair(num_of_lists[i_temp], i_temp);
    });
    stable_sort(p.begin(), p.end());
    int32_t k = 0;
    int32_t i = p.size() - 1;
    size_t j = 0;
    bool flag = false;
    std::vector<std::pair<std::string, int64_t>> ans;
    while (j <= dict_size && i >= 0 && k <= 10000000) {
        flag = false;
        for (int32_t pop = st[p[i].second].right - sum_str[p[i].second]; pop < st[p[i].second].right; ++pop) {
            if (s[pop] == min_char) {
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
        ans.push_back(
            {s.substr(st[p[i].second].right - sum_str[p[i].second], sum_str[p[i].second]), num_of_lists[p[i].second]});
        --i;
    }
    return ans;
}
// length of an edge
inline int32_t SuffTree::GetLength(const int32_t v) const {
    return (st[v].right - st[v].left);
}

// we need to go as much as we can
Position SuffTree::Go(Position pos, int32_t l, int32_t r) {
    while (true) {
        // if left == right => return
        if (l == r) {
            return pos;
        }

        // i made it as it was in the instruction at neerc ifmo
        int32_t stv = pos.vertex;
        if (pos.len_up == GetLength(stv)) {
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
            if (r - l < GetLength(stv) - pos.len_up) {
                return Position(stv, pos.len_up + r - l);
            }
            l += GetLength(stv) - pos.len_up;
            pos.len_up = GetLength(stv);
        }
    }
}

int32_t SuffTree::Split(const Position pos) {
    if (pos.len_up == GetLength(pos.vertex)) {
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

int32_t SuffTree::GetLink(const int32_t v) {
    if (st[v].link != -1) {
        return st[v].link;
    }
    if (st[v].parent == -1) {
        return 0;
    }
    int32_t to = GetLink(st[v].parent);
    int32_t returned = Split(Go(Position(to, GetLength(to)), st[v].left + (st[v].parent == 0), st[v].right));
    st[v].link = returned;
    return returned;
}

Position SuffTree::ExtendUkkonen(Position ptr, const int32_t i) {
    while (true) {
        // almost nullptr, but it is not nullptr in some cases
        Position nulptr = Go(ptr, i, i + 1);

        // if we can go, we go
        if (nulptr.vertex != -1) {
            return nulptr;
        }

        // we should Split the edge
        int32_t mid = Split(ptr);

        // append leaf
        int32_t leaf = cur;
        ++cur;
        st.push_back(Node_s(i, n, mid));

        // make a pointer to the leaf
        st[mid].next[s[i]] = leaf;

        // update ptr
        ptr.vertex = GetLink(mid);
        ptr.len_up = GetLength(ptr.vertex);

        // if we want to Split the 0-vert edge, return
        if (mid == 0) {
            return ptr;
        }
    }
}

void SuffTree::Dfs(int32_t start) {
    std::stack<int64_t> Stack;
    Stack.push(start);
    while (!Stack.empty()) {
        int64_t v = Stack.top();
        Stack.pop();
        if (static_cast<size_t>(v) >= (n << 1)) {
            for (auto& it : st[v - (n << 1)].next) {
                num_of_lists[v - (n << 1)] += num_of_lists[it.second];
            }
            continue;
        }
        if (st[v].parent != -1) {
            sum_str[v] += sum_str[st[v].parent] + GetLength(v);
        } else {
            sum_str[v] += GetLength(v);
        }
        // because of the unread symbol
        if (static_cast<size_t>(st[v].right) == n) {
            --sum_str[v];
        }

        // if list => num_of_lists = 0
        if (st[v].next.size() == 0) {
            num_of_lists[v] = 1;
        } else {
            Stack.push((n << 1) + v);
            for (auto& it : st[v].next) {
                Stack.push(it.second);
            }
        }
    }
}
