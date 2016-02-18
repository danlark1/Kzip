#pragma once

#include <experimental/algorithm>
#include <experimental/string_view>

#include <exception>
#include <string>
#include <vector>
#include <sstream>

namespace Codecs {
    using std::experimental::string_view;
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
        virtual void encode(string& encoded, const string_view& raw) const = 0;
        virtual void decode(string& raw, const string_view& encoded) = 0;
        virtual void decode_current_machine(string& raw, const string_view& encoded) = 0; //in the future we will not need it
        virtual string save() const = 0;
        virtual void load(const string&) = 0;

        virtual size_t sample_size(size_t records_total) const = 0;
        virtual void learn(const StringViewVector& sample) = 0;

        virtual void reset() = 0;

        virtual ~CodecIFace() {}
    };
}
