#include <fstream>
#include <iostream>
#include "tester.h"

template <typename Iter>
void select_sample(StringViewVector& sample, Iter begin, Iter end,
  size_t sample_size) {
  std::experimental::sample(begin, end, std::back_inserter(sample),
    sample_size, std::mt19937(std::random_device()()));
}

void Tester::learn_codec() {
  StringViewVector sample;
  select_sample(sample, this->data.begin(), this->data.end(), this->codec->sample_size(this->data.size()));
  double start = 1.0 * clock();
  this->codec->learn(sample);
  double finish = 1.0 * clock();
  std::cout << "learning is successful in " << (finish - start) / CLOCKS_PER_SEC << std::endl;
}

void Tester::readfile(const std::string& data_in_file) {
  std::ifstream input(data_in_file, std::ios_base::binary);
  this->data.clear();
  while(input.good()) {
    std::string cur_string;
    getline(input, cur_string);
    this->data.push_back(cur_string);
  }
  input.close();
  std::cout << this->data.size() << " strings were read" << std::endl;
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
    std::string out = "", cur_string ="";
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
  this->codec->load("config");
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
  std::cout << "encode ended in " << (finish - start) / CLOCKS_PER_SEC << std::endl;
}

void Tester::test_decode() {
  double start = 1.0 * clock();
  for (auto& cur_string : this->encoded_data) {
    std::string out;
    //this->codec->decode_current_machine(out, cur_string);
    this->codec->decode(out, cur_string);
    this->decoded_data.push_back(out);
  } 
  double finish = 1.0 * clock();
  std::cout << "decode ended in " << (finish - start) / CLOCKS_PER_SEC << std::endl;
}

void Tester::save_info() {
  std::ofstream out("config1");
  out << encoded_data.size() << '\n';
  for (auto& cur_string : encoded_data) {
    out << cur_string.size() << '\n';
  } 
}

void Tester::check_correctness() {
  int error_count = 0;
  if (this->data.size() != this->decoded_data.size()) {
    std::cout << "Sizes don't match" << std::endl;
    return;
  }
  auto data_it = this->data.begin();
  auto decoded_it = this->decoded_data.begin();
  while (data_it != this->data.end() && decoded_it != this->decoded_data.end()) {
    if (*data_it != *decoded_it) {
      ++error_count;
    }
    ++data_it;
    ++decoded_it;
  }
  double perc_errors = 100.0 * error_count / (int)this->data.size();
  std::cout << perc_errors << "% errors were occured" << std::endl;
}

void Tester::saved_memory() {
  int64_t saved = 0;
  if (this->data.size() != this->decoded_data.size()) {
    std::cout << "Sizes don't match" << std::endl;
    return;
  }
  auto data_it = this->data.begin();
  auto encoded_it = this->encoded_data.begin();
  while (data_it != this->data.end() && encoded_it != this->encoded_data.end()) {
    saved += (data_it->size() - encoded_it->size());
    ++data_it;
    ++encoded_it;
  }
  std::cout << "Memory saved (bytes): " << saved << std::endl;
  std::cout << "Memory saved (MBs): " << 1.0 * saved / 1024 / 1024 << std::endl;
}

