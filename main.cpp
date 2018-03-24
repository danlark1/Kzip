// Copyright 2017, Danila Kutenin
#include "Huffman.h"
#include "tester.h"

#include <getopt.h>

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>

#include <string.h>

#include <unistd.h>

struct globalArgs_t {
    size_t dict_size;
    bool parse;
    char* input;
    bool test_time;
} globalArgs;

static const char* optString = "d:p:f:t:h?";

static const struct option longOpts[] = {
    {"dict_size", required_argument, 0, 'd'}, {"parsing", required_argument, 0, 'p'}, {"help", no_argument, NULL, 'h'},
    {"file", required_argument, NULL, 'f'},   {"time", required_argument, NULL, 't'}, {NULL, no_argument, NULL, 0}};

void display_usage(void) {
    puts("Kzip is here");
    puts("./kzip [OPTIONS] [FILE]");
    puts("--d --- dict size, (12400 default). MAX_LIMIT = 200000");
    puts("--p --- parsing argument (0 for \\n, 1 for LE uint32_t, 0 default)");
    puts("--f --- file to test (no default) REQUIRED PARAMETER.");
    puts("--t --- to test time. Need more memory than usual. (0 or 1), 0 default");
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    Tester tester;
    Codecs::HuffmanCodec huffman;
    tester.SetCodec(&huffman);
    // path to file

    int opt = 0;
    int longIndex;
    int count_files = 0;
    globalArgs.dict_size = 12400;  // standart dict_size
    globalArgs.parse = 0;
    globalArgs.test_time = 0;
    globalArgs.input = NULL;

    while ((opt = getopt_long(argc, argv, optString, longOpts, &longIndex)) != -1) {
        switch (opt) {
        case 'h': {
            display_usage();
            break;
        };
        case 'd': {
            globalArgs.dict_size = std::atoi(optarg);
            break;
        };
        case 'p': {
            globalArgs.parse = std::atoi(optarg);
            break;
        };
        case 'f': {
            globalArgs.input = optarg;
            ++count_files;
            break;
        };
        case 't': {
            globalArgs.test_time = std::atoi(optarg);
            break;
        }
        case '?':
        default: {
            fprintf(stderr, "found unknown option, try --help or -h for help\n");
            exit(EXIT_FAILURE);
            break;
        };
        }
    }

    if (count_files == 0 || count_files >= 2) {
        fprintf(stderr, "no file to decode or too many files\n");
        exit(EXIT_FAILURE);
    }

    if (globalArgs.parse == 1) {
        tester.ReadFileUint(globalArgs.input);
    } else if (globalArgs.parse == 0) {
        tester.ReadFile(globalArgs.input);
    } else {
        fprintf(stderr, "invalid argument for parsing\n");
        exit(EXIT_FAILURE);
    }

    if (globalArgs.dict_size >= 200000) {
        globalArgs.dict_size = 200000;
    }
    printf("Maximum dictionary size is %zu\n", globalArgs.dict_size);
    tester.LearnCodec(globalArgs.dict_size);

    if (globalArgs.test_time == 0) {
        tester.TestEncodeDecode();
        huffman.Reset();
    } else {
        tester.SaveConfig();
        tester.TestEncode();
        huffman.Reset();
        tester.TestDecode();
        tester.CheckCorrectness();
        tester.SavedMemory();
        huffman.Reset();
        tester.Reset();
    }
    return 0;
}
