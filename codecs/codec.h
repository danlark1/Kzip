// Copyright 2017, Danila Kutenin
#pragma once
#include <exception>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace Codecs {
    using std::string_view;
    using std::string;
    using std::vector;
    using StringViewVector = vector< string_view >;
    using StringVector = vector< string >;

    class CodecException : public std::exception, public std::ostringstream {
        std::string descr;
    public:
        CodecException() = default;
        CodecException(CodecException&& c) noexcept {
            (*this) = std::move(c);
        }

        CodecException& operator=(CodecException&& c) noexcept {
            descr = std::move(c.descr);
            (std::exception&)(*this) = std::move(c);
            return (*this);
        }

        explicit CodecException(const std::string& d)
            : descr(d)
        {}

        const char* what() const noexcept override {
            return descr.c_str();
        }
    };

#if defined(cthrow)
    #error "already defined"
#else
    #define cthrow(what) do { std::ostringstream s; s << "at " << __FILE__ << ":" << __LINE__ << " " << what; \
        throw CodecException(s.str()); \
    } while (false)
#endif

    class CodecIFace {
    public:
        virtual void Encode(string& encoded, const string_view raw) const = 0;
        virtual void Decode(string& raw, const string_view encoded) const = 0;
        virtual void Save(const string_view file_name) const = 0;
        virtual void Load(const string_view) = 0;

        virtual size_t SampleSize(size_t records_total) const = 0;
        virtual void Learn(StringViewVector& sample, const size_t dict_size) = 0;

        virtual void Reset() = 0;

        virtual ~CodecIFace() {}
    };
}
