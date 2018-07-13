// Copyright 2017-2018, Danila Kutenin

#include "Huffman.h"
#include "Node.h"
#include "Suffix_tree.h"
#include "Trie.h"
#include "Util.h"

#include <algorithm>
#include <cinttypes>
#include <fstream>
#include <ios>
#include <iostream>
#include <map>
#include <queue>
#include <random>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace NCodecs {

    void HuffmanCodec::Encode(std::string& encoded, const std::string_view raw) const {
        // empty strings should remain empty
        if (__builtin_expect(!raw.size(), false)) {
            return;
        }
        encoded.reserve(raw.size() << 1);
        unsigned char buf = 0;
        int8_t count = CHAR_SIZE - LOG_CHAR_SIZE;
        size_t code_index = 0;
        size_t raw_index = 0;
        size_t uz = 0;
        size_t copy_raw = 0;
        size_t last_uz = 0;

        while (raw_index < raw.size()) {
            code_index = 0;
            uz = 0;
            copy_raw = raw_index;

            uz = trie.Next(uz, raw[raw_index]);
            last_uz = uz;

            while (raw_index + 1 < raw.size() && trie.IsNext(uz, raw[raw_index + 1])) {
                uz = trie.Next(uz, raw[raw_index + 1]);
                ++raw_index;
                if (trie.nodes[uz].is_terminal) {
                    copy_raw = raw_index;
                    last_uz = uz;
                }
            }
            raw_index = copy_raw;
            size_t size_of_path = trie.nodes[last_uz].code.size();
            while (code_index < size_of_path) {
                while (code_index < size_of_path && count) {
                    buf <<= 1;
                    buf += trie.nodes[last_uz].code[code_index];
                    ++code_index;
                    --count;
                }
                if (!count) {
                    count = CHAR_SIZE;
                    encoded.push_back(buf);
                    buf = 0;
                }
            }
            ++raw_index;
        }
        // this happens with probability 7/8
        if (__builtin_expect(count != CHAR_SIZE, true)) {
            encoded.push_back(buf << count);
        }
        // first "three" bits for last char
        encoded[0] |= (static_cast<unsigned char>(CHAR_SIZE - count)) << (CHAR_SIZE - LOG_CHAR_SIZE);
    }

    void HuffmanCodec::Decode(std::string& raw, const std::string_view encoded) const {
        if (__builtin_expect(!encoded.size(), false)) {
            return;
        }
        raw.reserve(encoded.size() << 1);
        int8_t count = LOG_CHAR_SIZE;
        Node* cur = root_for_decode;
        unsigned char byte = encoded[0];
        int64_t encoded_size = static_cast<int64_t>(encoded.size()) - 1;

        if (encoded_size >= 1) {
            while (count != CHAR_SIZE) {
                if (byte & (1 << (CHAR_SIZE - count - 1))) {
                    cur = cur->right;
                } else {
                    cur = cur->left;
                }
                if (!cur->left) {
                    raw += cur->GetData();
                    cur = root_for_decode;
                }
                ++count;
            }
            count = 0;
            byte = encoded[1];
        }

        // till almost the last element
        for (int64_t i = 1; i < encoded_size; ++i) {
            raw += cur->to_go[byte].first;
            cur = cur->to_go[byte].second;
            byte = encoded[i + 1];
        }

        // last element
        while (true) {
            if (byte & (1 << (CHAR_SIZE - count - 1))) {
                cur = cur->right;
            } else {
                cur = cur->left;
            }
            if (!cur->left) {
                raw += cur->GetData();
                cur = root_for_decode;
            }
            ++count;
            if (count == CHAR_SIZE || (static_cast<unsigned char>(encoded[0]) >> (CHAR_SIZE - LOG_CHAR_SIZE)) == count) {
                break;
            }
        }
    }

    void HuffmanCodec::Save(const std::string_view file_name) const {
        std::ofstream output(file_name.data(), std::ios_base::binary);
        for (const auto& str : ans) {
            output << std::hex << str.first.size() << " " << str.first << " " << std::hex << str.second << "\n";
        }
        output.close();
    }

    void HuffmanCodec::BuildTable(Node* root_for_table, std::vector<int8_t>& code) {
        if (root_for_table->left) {
            code.push_back(false);
            BuildTable(root_for_table->left, code);
        }

        if (root_for_table->right) {
            code.push_back(true);
            BuildTable(root_for_table->right, code);
        }
        if (root_for_table->left == root_for_table->right) {
            auto s = root_for_table->GetData();
            trie.Insert(s, code);
        }
        code.pop_back();
    }

    void HuffmanCodec::Load(const std::string_view config) {
        ans.clear();
        ans.shrink_to_fit();
        trie.nodes.clear();
        trie = Trie();
        for (uint32_t c = 0; c < CHAR_SIZE_POWER; ++c) {
            std::string s(1, c);
            ans.push_back({s, 0});
        }

        std::ifstream input(config.data(), std::ios_base::binary);
        while (input.good()) {
            size_t len_string;
            std::string current;
            input >> std::hex >> len_string;
            input.get();
            if (!input.good())
                break;
            for (size_t i = 0; i < len_string; ++i) {
                current += input.get();
            }
            size_t frequency;
            input >> std::hex >> frequency;
            if (len_string == 1) {
                ans[static_cast<unsigned char>(current[0])].second = frequency;
            } else {
                ans.push_back({current, frequency});
            }
        }
        input.close();
        BuildTree();
    }

    void HuffmanCodec::BuildTree() {
        std::priority_queue<std::pair<Node*, int64_t>, std::vector<std::pair<Node*, int64_t>>, Comp> table_cur;
        int64_t i = 0;
        for (const auto& c : ans) {
            Node* p = new Node(c.first, c.second);
            table_cur.push({p, i});
            ++i;
        }
        Node* parent;
        while (table_cur.size() != 1) {
            Node* left_son = table_cur.top().first;
            table_cur.pop();
            Node* right_son = table_cur.top().first;
            table_cur.pop();
            parent = new Node(left_son, right_son);
            table_cur.push({parent, i});
            ++i;
        }
        root_for_decode = table_cur.top().first;
        BuildJumps(root_for_decode);
        std::vector<int8_t> code;
        BuildTable(root_for_decode, code);
    }

    size_t HuffmanCodec::SampleSize(size_t records_total) const {
        return std::min(static_cast<size_t>(MAX_CONCAT_SIZE), records_total);
    }

    void HuffmanCodec::BuildJumps(Node* node) {
        Node* cur;
        for (size_t byte = 0; byte < (CHAR_SIZE_POWER); ++byte) {
            cur = node;
            if (!cur->right || !cur->left) {
                continue;
            }
            for (size_t j = 0; j < CHAR_SIZE; ++j) {
                if (byte & (1 << (CHAR_SIZE - j - 1))) {
                    cur = cur->right;
                } else {
                    cur = cur->left;
                }
                if (!cur->left) { // is equal to cur->left == nullptr == cur->right
                    node->to_go[byte].first += cur->GetData();
                    cur = root_for_decode;
                }
            }
            node->to_go[byte].second = cur;
        }
        if (node->left) {
            BuildJumps(node->left);
            BuildJumps(node->right);
        }
    }

    void HuffmanCodec::Reset() {
        ans.clear();
        ans.shrink_to_fit();
        if (root_for_decode) {
            delete root_for_decode;
        }
    }

} // namespace NCodecs
