// Copyright 2017-2018, Danila Kutenin
#pragma once
#include "codec.h"

#include <string>
#include <string_view>
#include <vector>

class Tester {
public:
    void ReadFile(const std::string&);
    void ReadFileUint(const std::string&);
    void ReadDecodedFile(const std::string&);
    void WriteEncodedFile(const std::string&);
    void WriteDecodedFile(const std::string&);
    void LearnCodec(const size_t dict_size);
    void SetCodec(NCodecs::ICodecFace*);
    void CheckCorrectness();
    void SaveConfig();
    void SaveInfo();
    void SavedMemory();
    void TestEncode();
    void TestDecode();
    void Reset();
    void Load();
    void TestEncodeDecode();

private:
    NCodecs::ICodecFace* codec;
    std::vector<std::string> data;
    std::vector<std::string> decoded_data;
    std::vector<std::string> encoded_data;
};
