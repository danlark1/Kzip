// Copyright 2017-2018, Danila Kutenin
#pragma once
#include <exception>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace NCodecs {

    class ICodecFace {
    public:
        virtual void Encode(std::string& encoded, const std::string_view raw) const = 0;
        virtual void Decode(std::string& raw, const std::string_view encoded) const = 0;
        virtual void Save(const std::string_view file_name) const = 0;
        virtual void Load(const std::string_view) = 0;

        virtual size_t SampleSize(size_t records_total) const = 0;
        virtual void Learn(std::vector<std::string_view>& sample, const size_t dict_size) = 0;

        virtual void Reset() = 0;

        virtual ~ICodecFace() = default;
    };

} // namespace NCodecs
