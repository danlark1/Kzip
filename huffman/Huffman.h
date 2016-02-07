#pragma once

#include "codec.h"

namespace Codecs {

  class HuffmanCodec : public CodecIFace {
  public:
    void encode(string_view& encoded, const string_view& raw) const override;
    void decode(string_view& raw, const string_view& encoded) const override;
    // string save() const override;
    // void load(const string_view&) override;
    // size_t sample_size(size_t records_total) const override;
    void learn(const StringViewVector& all_samples) override;
    // void reset() override;
  };

} 