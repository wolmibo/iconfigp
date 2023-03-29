// Copyright (c) 2023 wolmibo
// SPDX-License-Identifier: MIT

#ifndef ICONFIGP_COLOR_HPP_INCLUDED
#define ICONFIGP_COLOR_HPP_INCLUDED

#include "iconfigp/value-parser.hpp"

#include <array>



namespace iconfigp {

using rgba_f32 = std::array<float, 4>;



template<> struct value_parser<rgba_f32> {
  static constexpr std::string_view name{"color (rgba_f32)"};

  static std::string_view format() {
    return "([#]rrggbb[aa] | [#]rgb[a]) // case insensitive";
  }

  static std::optional<rgba_f32> parse(std::string_view);
};

}

#endif // ICONFIGP_COLOR_HPP_INCLUDED
