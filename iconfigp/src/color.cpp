#include "iconfigp/color.hpp"
#include "iconfigp/space.hpp"

#include <cstdint>
#include <limits>



namespace {
  [[nodiscard]] iconfigp::rgba_f32 convert(const std::array<uint8_t, 4>& input) {
    auto range = static_cast<float>(std::numeric_limits<uint8_t>::max());
    return {
      static_cast<float>(input[0]) / range,
      static_cast<float>(input[1]) / range,
      static_cast<float>(input[2]) / range,
      static_cast<float>(input[3]) / range,
    };
  }



  [[nodiscard]] std::optional<uint8_t> parse_hex(char c) {
    if ('0' <= c && c <= '9') { return c - '0'; }
    if ('a' <= c && c <= 'f') { return c - 'a' + 10; } //NOLINT(*magic-numbers)
    if ('A' <= c && c <= 'F') { return c - 'A' + 10; } //NOLINT(*magic-numbers)
    return {};
  }



  [[nodiscard]] std::optional<uint8_t> parse_hex(std::string_view input) {
    if (input.size() == 1 || input.size() == 2) {
      auto l = parse_hex(input.front());
      auto r = parse_hex(input.back());

      if (!l || !r) {
        return {};
      }

      return (*l << 4) | *r;
    }

    return {};
  }



  [[nodiscard]] std::optional<std::array<uint8_t, 4>> parse_hex_color(
      std::string_view input
  ) {
    std::array<uint8_t, 4> output{};

    size_t skip{1};

    if (input.size() == std::string_view{"rrggbb"}.size() ||
        input.size() == std::string_view{"rrggbbaa"}.size()) {
      skip = 2;
    } else if (input.size() != 3 && input.size() != 4) {
      return {};
    }

    if (input.size() % 4 == 0) {
      if (auto value = parse_hex(input.substr(input.size() - skip, skip))) {
        output[3] = *value;
      } else {
        return {};
      }
    } else {
      output[3] = std::numeric_limits<uint8_t>::max();
    }

    for (size_t i = 0; i < 3; ++i) {
      if (auto value = parse_hex(input.substr(i * skip, skip))) {
        output[i] = *value; //NOLINT(*constant-array-index)
      } else {
        return {};
      }
    }

    return output;
  }
}





std::optional<iconfigp::rgba_f32>
iconfigp::value_parser<iconfigp::rgba_f32>::parse(std::string_view input) {
  input = trim(input);

  if (input.empty()) {
    return {};
  }

  if (input.starts_with('#')) {
    input.remove_prefix(1);
  }

  if (auto col = parse_hex_color(input)) {
    return convert(*col);
  }


  return {};
}
