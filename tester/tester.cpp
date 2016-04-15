#include <cstdio>
#include <ctime>
#include <fstream>
#include "tester.h"
#include <iostream>

template <typename Iter>
void select_sample(StringViewVector& sample, Iter begin, Iter end,
  size_t sample_size) {
  std::experimental::sample(begin, end, std::back_inserter(sample),
    sample_size, std::mt19937(std::random_device()()));
}

void Tester::learn_codec() {
  StringViewVector sample;
  select_sample(sample, this->data.begin(), this->data.end(), this->codec->sample_size(this->data.size()));
  double start = clock();
  this->codec->learn(sample);
  double finish = clock();
  printf("learning is successful in %f\n", (finish - start) / CLOCKS_PER_SEC);
}

void Tester::readfile(const std::string& data_in_file) {
  std::ifstream input(data_in_file, std::ios_base::binary);
  while(input.good()) {
    std::string cur_string;
    getline(input, cur_string);
    cur_string.shrink_to_fit();
    this->data.push_back(cur_string);
  }
  input.close();
  printf("%zu strings were read\n", this->data.size());
}

void Tester::write_encoded_file(const std::string& where_to) {
  std::ofstream output(where_to, std::ios_base::binary);
  for (auto& cur_string : this->encoded_data) {
    output << cur_string << '\n';
  }
  output.close();
}

void Tester::write_decoded_file(const std::string& where_to) {
  std::ofstream output(where_to, std::ios_base::binary);
  for (auto& cur_string : this->decoded_data) {
    output << cur_string << '\n';
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


void Tester::set_codec(Codecs::CodecIFace& codec) {
  (this->codec) = &codec;
}

void Tester::test_encode() {
  double start = 1.0 * clock();
  for (auto& cur_string : this->data) {
    std::string out;
    this->codec->encode(out, cur_string);
    this->encoded_data.push_back(out);
  }
  double finish = 1.0 * clock();
  printf("encode ended in %f\n", (finish - start) / CLOCKS_PER_SEC);
}

void Tester::test_decode() {
  this->codec->load("config");

  double start = 1.0 * clock();
  for (auto& cur_string : this->encoded_data) {
    std::string out;
    //this->codec->decode_current_machine(out, cur_string);
    // if (cur_string.size() == 1) {
    //   this->decoded_data.push_back(out);
    //   continue;
    // }
    this->codec->decode(out, cur_string);
    this->decoded_data.push_back(out);
  } 
  double finish = 1.0 * clock();
  printf("decode ended in %f\n", (finish - start) / CLOCKS_PER_SEC);
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
    return;
  }
  auto data_it = this->data.begin();
  auto decoded_it = this->decoded_data.begin();
  int64_t i = 0;
  while (data_it != this->data.end() && decoded_it != this->decoded_data.end()) {
    if (*data_it != *decoded_it) {
      ++error_count;
      std::cout << *data_it << " " << *decoded_it << std::endl;
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
  if (this->data.size() != this->decoded_data.size()) {
    return;
  }
  auto data_it = this->data.begin();
  auto encoded_it = this->encoded_data.begin();
  while (data_it != this->data.end() && encoded_it != this->encoded_data.end()) {
    saved += static_cast<int64_t>((data_it->size() - encoded_it->size()));
    ++data_it;
    ++encoded_it;
  }
  printf("Memory saved (MBs): %f\n", 1.0 * saved / 1024 / 1024);
}

void Tester::reset() {
  this->data.clear();
  this->data.shrink_to_fit();
  this->decoded_data.clear();
  this->decoded_data.shrink_to_fit();
  this->encoded_data.clear();
  this->encoded_data.shrink_to_fit();
}
