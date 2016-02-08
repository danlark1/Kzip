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
  this->codec->learn(sample);
  std::cout << "learning is successful" << std::endl;
}

void Tester::readfile(const std::string& data_in_file) {
  std::ifstream input(data_in_file);
  while(input.good()) {
    std::string cur_string;
    getline(input, cur_string);
    this->data.push_back(cur_string);
  }
  input.close();
  std::cout << this->data.size() << " strings were read" << std::endl;
}

void Tester::set_codec(Codecs::CodecIFace& codec) {
  this->codec = &codec;
}

void Tester::test_encode() {
  for (auto& cur_string : this->data) {
    std::string out;
    this->codec->encode(out, cur_string);
    this->encoded_data.push_back(out);
  }
  std::cout << "encode ended" << std::endl;
}

void Tester::test_decode() {
  for (auto& cur_string : this->data) {
    std::string out;
    this->codec->decode(out, cur_string);
    this->decoded_data.push_back(out);
  } 
  std::cout << "decode ended" << std::endl;
}

void Tester::check_correctness() {
  int error_count = 0;
  if (this->encoded_data.size() != this->decoded_data.size()) {
    std::cout << "Sizes don't match" << std::endl;
    return;
  }
  auto encoded_it = this->encoded_data.begin();
  auto decoded_it = this->decoded_data.begin();
  while (encoded_it != this->encoded_data.end() && decoded_it != this->decoded_data.end()) {
    if (*encoded_it != *decoded_it) {
      ++error_count;
    }
    ++decoded_it;
    ++encoded_it;
  }
  double perc_errors = 100.0 * error_count / (int)this->encoded_data.size();
  std::cout << perc_errors << "% errors were occured" << std::endl;
}

