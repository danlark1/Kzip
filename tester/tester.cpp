// Copyright 2017-2018, Danila Kutenin

#include "tester.h"

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

template <typename Iter>
void SelectSample(std::vector<std::string_view>& sample, Iter begin, Iter end, size_t sample_size) {
    std::sample(begin, end, std::back_inserter(sample), sample_size, std::mt19937(std::random_device()()));
}

void Tester::LearnCodec(const size_t dict_size) {
    std::vector<std::string_view> sample;
    SelectSample(sample, this->data.begin(), this->data.end(), this->codec->SampleSize(this->data.size()));
    double start = clock();
    if (dict_size == 0) {
        this->codec->Learn(sample, 12400);
    } else {
        this->codec->Learn(sample, dict_size);
    }
    double finish = clock();
    printf("learning is successful in %f\n", (finish - start) / CLOCKS_PER_SEC);
}

void Tester::ReadFile(const std::string& data_in_file) {
    std::ifstream input(data_in_file, std::ios_base::binary);
    if (!input.is_open()) {
        printf("Cannot open the file %s\n", data_in_file.c_str());
        exit(EXIT_FAILURE);
    }
    std::string cur_string;
    while (input.good()) {
        getline(input, cur_string);
        this->data.push_back(cur_string);
    }
    input.close();
    this->data.shrink_to_fit();
    printf("%zu strings were read\n", this->data.size());
}

void Tester::ReadFileUint(const std::string& data_in_file) {
    std::ifstream input(data_in_file, std::ios_base::binary);
    if (!input.is_open()) {
        printf("Cannot open the file %s\n", data_in_file.c_str());
        exit(EXIT_FAILURE);
    }
    while (input.good()) {
        std::string cur_string;
        std::string bin;
        for (size_t i = 0; i < 4; ++i) {
            bin += input.get();
        }
        std::istringstream raw(bin);
        uint32_t sz;
        raw.read(reinterpret_cast<char*>(&sz), sizeof sz);
        for (uint32_t i = 0; i < sz; ++i) {
            if (!input.good()) {
                break;
            }
            cur_string += input.get();
        }
        this->data.push_back(cur_string);
    }
    input.close();
    this->data.shrink_to_fit();
    printf("%zu strings were read\n", this->data.size());
}

void Tester::WriteEncodedFile(const std::string& where_to) {
    std::ofstream output(where_to, std::ios_base::binary);
    for (size_t i = 0; i + 1 < encoded_data.size(); ++i) {
        output << encoded_data[i] << '\n';
    }
    if (encoded_data.size() >= 1) {
        output << encoded_data[encoded_data.size() - 1];
    }
    output.close();
}

void Tester::WriteDecodedFile(const std::string& where_to) {
    std::ofstream output(where_to, std::ios_base::binary);
    for (size_t i = 0; i + 1 < decoded_data.size(); ++i) {
        output << decoded_data[i] << '\n';
    }
    if (decoded_data.size() >= 1) {
        output << decoded_data[decoded_data.size() - 1];
    }
    output.close();
}

void Tester::ReadDecodedFile(const std::string& from) {
    std::ifstream input(from, std::ios_base::binary);
    std::ifstream conf("Dictionary");
    encoded_data.clear();
    uint64_t n;
    conf >> n;
    for (size_t i = 0; i < n; ++i) {
        uint32_t input_config;
        std::string out = "", cur_string = "";
        conf >> input_config;
        int64_t j = 0;
        while (j - 1 != input_config) {
            getline(input, cur_string);
            out += cur_string;
            out += '\n';
            j += (cur_string.size() + 1);
        }
        out.pop_back();
        encoded_data.push_back(out);
    }
    input.close();
    conf.close();
}

void Tester::SaveConfig() {
    this->codec->Save("Dictionary");
}

void Tester::SetCodec(NCodecs::ICodecFace* set_codec) {
    this->codec = set_codec;
}

void Tester::TestEncode() {
    double start = 1.0 * clock();
    size_t i = 0;
    for (auto& cur_string : this->data) {
        std::string out;
        ++i;
        this->codec->Encode(out, cur_string);
        this->encoded_data.push_back(out);
    }
    double finish = 1.0 * clock();
    printf("encode ended in %f\n", (finish - start) / CLOCKS_PER_SEC);
}

void Tester::TestEncodeDecode() {
    size_t i = 0;
    this->codec->Save("Dictionary");
    this->codec->Reset();
    this->codec->Load("Dictionary");
    size_t mem1 = 0;
    size_t mem2 = 0;

    double time_en = 0;
    double time_dec = 0;
    double start;
    double finish;
    size_t error_count = 0;

    std::ofstream errors("errors", std::ios_base::binary);
    for (auto& cur_string : this->data) {
        // start encoding
        std::string out;
        start = clock();
        this->codec->Encode(out, cur_string);
        finish = clock();
        time_en += finish - start;

        ++i;

        std::string another;

        start = clock();
        this->codec->Decode(another, out);
        finish = clock();
        time_dec += finish - start;

        mem1 += out.size();
        mem2 += cur_string.size();

        if (another != cur_string) {
            errors << another << std::endl;
            errors << cur_string << std::endl;
            errors << i << std::endl;
            ++error_count;
        }
    }
    errors.close();
    printf("encode ended in %f\n", (time_en) / CLOCKS_PER_SEC);
    printf("decode ended in %f\n", (time_dec) / CLOCKS_PER_SEC);
    printf("Memory saved (MBs): %f\n", 1.0 * ((long long)mem2 - (long long)mem1) / 1024 / 1024);
    // max dict size is 1mb on average
    std::ifstream file("Dictionary", std::ios::binary | std::ios::ate);
    printf("Memory saved (percent): %f%%\n", 100 - 100.0 * (mem1 + file.tellg()) / mem2);
    printf("%zu errors were occured\n", error_count);
}

void Tester::TestDecode() {
    this->codec->Load("Dictionary");
    double start = 1.0 * clock();
    for (auto& cur_string : this->encoded_data) {
        std::string out;
        this->codec->Decode(out, cur_string);
        this->decoded_data.push_back(out);
    }
    double finish = 1.0 * clock();
    printf("decode ended in %f\n", (finish - start) / CLOCKS_PER_SEC);
}

void Tester::Load() {
    this->codec->Load("Dictionary");
}

void Tester::SaveInfo() {
    std::ofstream out("Dictionary1");
    out << encoded_data.size() << '\n';
    for (auto& cur_string : encoded_data) {
        out << cur_string.size() << '\n';
    }
    out.close();
}

void Tester::CheckCorrectness() {
    int64_t error_count = 0;
    if (this->data.size() != this->decoded_data.size()) {
        printf("incorrect size of decoded stream\n");
        return;
    }
    auto data_it = this->data.begin();
    auto decoded_it = this->decoded_data.begin();
    int64_t i = 0;
    std::ofstream errors("c", std::ios_base::binary);
    while (data_it != this->data.end() && decoded_it != this->decoded_data.end()) {
        if (*data_it != *decoded_it) {
            ++error_count;
            if (error_count < 10) {
                errors << *data_it << "\n"
                       << *decoded_it << std::endl;
            }
        }
        ++i;
        ++data_it;
        ++decoded_it;
    }
    double perc_errors = 100.0 * error_count / (int64_t)this->data.size();
    printf("%f percent errors were occured\n", perc_errors);
}

void Tester::SavedMemory() {
    int64_t saved = 0;
    int64_t mem1 = 0;
    int64_t mem2 = 0;
    if (this->data.size() != this->decoded_data.size()) {
        return;
    }
    auto data_it = this->data.begin();
    auto encoded_it = this->encoded_data.begin();
    while (data_it != this->data.end() && encoded_it != this->encoded_data.end()) {
        saved += static_cast<int64_t>((data_it->size() - encoded_it->size()));
        mem1 += encoded_it->size();
        mem2 += data_it->size();
        ++data_it;
        ++encoded_it;
    }
    printf("Memory saved (MBs): %f\n", 1.0 * saved / 1024 / 1024);
    std::ifstream file("Dictionary", std::ios::binary | std::ios::ate);
    printf("Memory saved (percent): %f%%\n", 100 - 100.0 * (mem1 + file.tellg()) / mem2);
}

void Tester::Reset() {
    this->data.clear();
    this->data.shrink_to_fit();
    this->decoded_data.clear();
    this->decoded_data.shrink_to_fit();
    this->encoded_data.clear();
    this->encoded_data.shrink_to_fit();
}
