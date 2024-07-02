// Copyright (c) 2023 wolmibo
// SPDX-License-Identifier: MIT

#ifndef ICONFIGP_VALUE_PARSER_HPP_INCLUDED
#define ICONFIGP_VALUE_PARSER_HPP_INCLUDED

#include "iconfigp/exception.hpp"
#include "iconfigp/format.hpp"
#include "iconfigp/key-value.hpp"
#include "iconfigp/opt-ref.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <cstdint>
#include <limits>
#include <optional>
#include <sstream>
#include <string_view>



namespace iconfigp {


template<typename T>
struct value_parser {};

}



//NOLINTNEXTLINE(*-macro-usage)
#define ICONFIGP_DEFINE_VALUE_PARSER_NAMED(type, nm, fmt, function)      \
  template<> struct iconfigp::value_parser<type> {                       \
    static constexpr std::string_view name     { nm };                   \
    static constexpr std::string      format() { return fmt; }           \
    static std::optional<type> parse(std::string_view source) {          \
      return function(source);                                           \
    }                                                                    \
  };

//NOLINTNEXTLINE(*-macro-usage)
#define ICONFIGP_DEFINE_VALUE_PARSER(type, format, function) \
  ICONFIGP_DEFINE_VALUE_PARSER_NAMED(type, #type, format, function)


ICONFIGP_DEFINE_VALUE_PARSER_NAMED(std::string_view, "string",
    "[string] // case sensitive",
    [](std::string_view source) { return source; })



namespace iconfigp {


template<typename T>
struct floating_point {};

template<>
struct floating_point<float> {
  static constexpr std::string_view name{"f32"};
  static constexpr float(*sto)(const std::string&, size_t*) = &std::stof;
};

template<>
struct floating_point<double> {
  static constexpr std::string_view name{"f64"};
  static constexpr double(*sto)(const std::string&, size_t*) = &std::stod;
};

template<>
struct floating_point<long double> {
  static constexpr std::string_view name{"f128"};
  static constexpr long double(*sto)(const std::string&, size_t*) = &std::stold;
};



template<typename T> requires (!floating_point<T>::name.empty())
struct value_parser<T> {
  static constexpr std::string_view name{floating_point<T>::name};

  static std::string format() {
    return "[floating point number]";
  }



  static std::optional<T> parse(std::string_view input) {
#if defined(_LIBCPP_VERSION)
    try {
      size_t processed{0};
      T output = floating_point<T>::sto(std::string{input}, &processed);
      if (processed == input.size()) {
        return output;
      }
    } catch (...) {
      return {};
    }
#else
    T output{};
    const auto *end = input.data() + input.size();
    if (std::from_chars(input.data(), end, output).ptr == end) {
      return output;
    }
#endif

    return {};
  }
};





template<typename T>
struct integer {};

template<>
struct integer<int8_t>   { static constexpr std::string_view name{"i8"};  };
template<>
struct integer<uint8_t>  { static constexpr std::string_view name{"u8"};  };
template<>
struct integer<int16_t>  { static constexpr std::string_view name{"i16"}; };
template<>
struct integer<uint16_t> { static constexpr std::string_view name{"u16"}; };
template<>
struct integer<int32_t>  { static constexpr std::string_view name{"i32"}; };
template<>
struct integer<uint32_t> { static constexpr std::string_view name{"u32"}; };
template<>
struct integer<int64_t>  { static constexpr std::string_view name{"i64"}; };
template<>
struct integer<uint64_t> { static constexpr std::string_view name{"u64"}; };



template<typename T> requires (!integer<T>::name.empty())
struct value_parser<T> {
  static constexpr std::string_view name{integer<T>::name};

  static std::string format() {
    return iconfigp::format("integer from {} to {}",
      std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
  }



  static std::optional<T> parse(std::string_view input) {
    T output{};
    const auto *end = input.data() + input.size();
    if (auto res = std::from_chars(input.data(), end, output);
        res.ptr == end && res.ec != std::errc::result_out_of_range) {

      return output;
    }

    return {};
  }
};





template<typename T>
struct case_insensitive_parse_lut {};



namespace details {
  template<typename T, typename V>
    requires (!std::is_same_v<T, std::string_view>)
  [[nodiscard]] constexpr std::pair<std::string_view, T> to_lut_pair(V&& value) {
    if constexpr (std::is_same_v<V, T>) {
      return std::pair<std::string_view, T>({}, std::forward<T>(value));
    } else {
      //NOLINTNEXTLINE(*-array-to-pointer-decay)
      return std::pair<std::string_view, T>(value, {});
    }
  }
}



template<typename T, typename ...Args>
[[nodiscard]] constexpr auto create_lut(std::string_view k1, T&& v1, Args&& ...args) {
  std::array<std::pair<std::string_view, T>, sizeof...(args)> buffer {
    details::to_lut_pair<T, Args>(std::forward<Args>(args))...
  };

  std::array<std::pair<std::string_view, T>, sizeof...(args) / 2 + 1> output;
  auto it = output.begin();

  *it++ = std::pair<std::string_view, T>(k1, std::forward<T>(v1));

  //NOLINTBEGIN(*-pointer-arithmetic)
  for (auto jt = buffer.begin(); it < output.end() && jt < buffer.end(); it++, jt += 2) {
    *it = std::pair<std::string_view, T>(jt->first, (jt + 1)->second);
  }
  //NOLINTEND(*-pointer-arithmetic)

  return output;
}
}


//NOLINTNEXTLINE(*-macro-usage)
#define ICONFIGP_DEFINE_CI_LUT_NAMED(type, nm, ...)              \
  template<> struct iconfigp::case_insensitive_parse_lut<type> { \
    static constexpr std::string_view name { nm };               \
    static constexpr auto lut = create_lut<type>(__VA_ARGS__);   \
  };

//NOLINTNEXTLINE(*-macro-usage)
#define ICONFIGP_DEFINE_CI_LUT(type, ...) \
  ICONFIGP_DEFINE_CI_LUT_NAMED(type, #type, __VA_ARGS__)

//NOLINTNEXTLINE(*-macro-usage)
#define ICONFIGP_DEFINE_ENUM_LUT_NAMED(type, nm, ...)            \
  template<> struct iconfigp::case_insensitive_parse_lut<type> { \
    using enum type;                                             \
    static constexpr std::string_view name { nm };               \
    static constexpr auto lut = create_lut<type>(__VA_ARGS__);   \
  };

//NOLINTNEXTLINE(*-macro-usage)
#define ICONFIGP_DEFINE_ENUM_LUT(type, ...) \
  ICONFIGP_DEFINE_ENUM_LUT_NAMED(type, #type, __VA_ARGS__)



ICONFIGP_DEFINE_CI_LUT(bool,
    "true",  true,  "t", true,  "yes", true,  "y", true,  "1", true,
    "false", false, "f", false, "no",  false, "n", false, "0", false)



namespace iconfigp {

template<typename T> requires (!case_insensitive_parse_lut<T>::name.empty())
struct value_parser<T> {
  static constexpr std::string_view name{case_insensitive_parse_lut<T>::name};

  static std::string format() {
    std::stringstream output;
    output << '(';

    bool first{true};
    for (const auto& [label, _]: case_insensitive_parse_lut<T>::lut) {
      if (!first) {
        output << '|';
      }
      output << label;

      first = false;
    }

    output << ") // case insensitive";
    return output.str();
  }



  static std::optional<T> parse(std::string_view input) {
    auto it = std::ranges::find_if(case_insensitive_parse_lut<T>::lut,
        [input](const auto& p) {
          return std::ranges::equal(input, std::get<0>(p),
              [](char l, char r) { return std::tolower(l) == r; });
    });

    if (it != std::ranges::end(case_insensitive_parse_lut<T>::lut)) {
      return std::get<1>(*it);
    }
    return {};
  }
};





template<typename T>
concept value_parser_defined = requires () { value_parser<T>::name; };



template<value_parser_defined T>
[[nodiscard]] T parse(const key_value& value) {
  try {
    if (auto result = value_parser<T>::parse(value.value())) {
      return *result;
    }

  } catch (const value_parse_exception::range_exception& rex) {
    throw value_parse_exception{value, std::string{value_parser<T>::name},
      std::string{value_parser<T>::format()}, rex};

  } catch (...) {
    throw value_parse_exception{value, std::string{value_parser<T>::name},
      std::string{value_parser<T>::format()}};
  }

  throw value_parse_exception{value, std::string{value_parser<T>::name},
    std::string{value_parser<T>::format()}};
}



template<value_parser_defined T>
[[nodiscard]] std::optional<T> parse(opt_ref<const key_value> value) {
  if (value) {
    return parse<T>(*value);
  }
  return {};
}

}


#endif // ICONFIGP_VALUE_PARSER_HPP_INCLUDED
