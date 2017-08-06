/*
  This is a simple tester, it shows the example how to use codec, pls, be sure
  that you understand everything here.
*/


#include "tester.h"
#include <cstdio>
#include <ctime>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <iostream>

template <typename Iter>
void select_sample(StringViewVector& sample, Iter begin, Iter end,
  size_t sample_size) {
  std::experimental::sample(begin, end, std::back_inserter(sample),
    sample_size, std::mt19937(std::random_device()()));
}

void Tester::learn_codec(const size_t dict_size) {
  StringViewVector sample;
  select_sample(sample, this->data.begin(), this->data.end(),
    this->codec->sample_size(this->data.size()));
  double start = clock();
  if (dict_size == 0) {
    this->codec->learn(sample, 12400);
  } else {
    this->codec->learn(sample, dict_size);
  }
  double finish = clock();
  printf("learning is successful in %f\n", (finish - start) / CLOCKS_PER_SEC);
}

void Tester::readfile(const std::string& data_in_file) {
  std::ifstream input(data_in_file, std::ios_base::binary);
  std::string cur_string;
  while (input.good()) {
    getline(input, cur_string);
    this->data.push_back(cur_string);
  }
  input.close();
  // sort(this->data.begin(), this->data.end());
  this->data.shrink_to_fit();
  printf("%zu strings were read\n", this->data.size());
}

void Tester::readfile_uint(const std::string& data_in_file) {
  std::ifstream input(data_in_file, std::ios_base::binary);
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
  // sort(this->data.begin(), this->data.end());
  this->data.shrink_to_fit();
  printf("%zu strings were read\n", this->data.size());
}

void Tester::write_encoded_file(const std::string& where_to) {
  std::ofstream output(where_to, std::ios_base::binary);
  for (size_t i = 0; i + 1 < encoded_data.size(); ++i) {
    output << encoded_data[i] << '\n';
  }
  if (encoded_data.size() >= 1) {
    output << encoded_data[encoded_data.size() - 1];
  }
  output.close();
}

void Tester::write_decoded_file(const std::string& where_to) {
  std::ofstream output(where_to, std::ios_base::binary);
  for (size_t i = 0; i + 1 < decoded_data.size(); ++i) {
    output << decoded_data[i] << '\n';
  }
  if (decoded_data.size() >= 1) {
    output << decoded_data[decoded_data.size() - 1];
  }
  output.close();
}


void Tester::read_decoded_file(const std::string& from) {
  std::ifstream input(from, std::ios_base::binary);
  std::ifstream conf("config1");
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

void Tester::save_config() {
  std::string out = this->codec->save();
}


void Tester::set_codec(Codecs::CodecIFace* codec) {
  this->codec = codec;
}

void Tester::test_encode() {
  double start = 1.0 * clock();
  size_t i = 0;
  for (auto& cur_string : this->data) {
    std::string out;
    ++i;
    this->codec->encode(out, cur_string);
    this->encoded_data.push_back(out);
  }
  double finish = 1.0 * clock();
  printf("encode ended in %f\n", (finish - start) / CLOCKS_PER_SEC);
}

void Tester::test_encode_decode() {
  size_t i = 0;
  std::string out = this->codec->save();
  this->codec->reset();
  this->codec->load("config");
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
    this->codec->encode(out, cur_string);
    finish = clock();
    time_en += finish - start;

    ++i;

    std::string another;

    start = clock();
    this->codec->decode(another, out);
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
  std::ifstream file("config", std::ios::binary | std::ios::ate);
  printf("Memory saved (percent): %f%%\n", 100 - 100.0 * (mem1 + file.tellg()) / mem2);
  printf("%zu errors were occured\n", error_count);
}

void Tester::test_decode() {
  this->codec->load("config");
  double start = 1.0 * clock();
  for (auto& cur_string : this->encoded_data) {
    std::string out;
    this->codec->decode(out, cur_string);
    this->decoded_data.push_back(out);
  }
  double finish = 1.0 * clock();
  printf("decode ended in %f\n", (finish - start) / CLOCKS_PER_SEC);
}

void Tester::load() {
  this->codec->load("config");
}

void Tester::save_info() {
  std::ofstream out("config1");
  out << encoded_data.size() << '\n';
  for (auto& cur_string : encoded_data) {
    out << cur_string.size() << '\n';
  }
  out.close();
}

void Tester::check_correctness() {

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
        errors << *data_it << "\n" << *decoded_it << std::endl;
      }
    }
    ++i;
    ++data_it;
    ++decoded_it;
  }
  double perc_errors = 100.0 * error_count / (int64_t)this->data.size();
  printf("%f percent errors were occured\n", perc_errors);
}

void Tester::saved_memory() {
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
  std::ifstream file("config", std::ios::binary | std::ios::ate);
  printf("Memory saved (percent): %f%%\n", 100 - 100.0 * (mem1 + file.tellg()) / mem2);
}

void Tester::reset() {
  this->data.clear();
  this->data.shrink_to_fit();
  this->decoded_data.clear();
  this->decoded_data.shrink_to_fit();
  this->encoded_data.clear();
  this->encoded_data.shrink_to_fit();
}
