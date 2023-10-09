// Copyright (c) 2023 wolmibo
// SPDX-License-Identifier: MIT

#ifndef ICONFIGP_ARRAY_HPP_INCLUDED
#define ICONFIGP_ARRAY_HPP_INCLUDED

#include "iconfigp/exception.hpp"
#include "iconfigp/value-parser.hpp"

#include <algorithm>
#include <array>
#include <string_view>



namespace iconfigp {

template<typename T, size_t Size>
[[nodiscard]] std::array<T, Size> parse_as_array(
    std::string_view input,
    std::string_view delim = ":,"
) {
  std::array<T, Size> buffer{};
  auto it = buffer.begin();

  size_t position = 0;

  for (; position < input.size() && it != buffer.end(); ++it) {
    auto str = input.substr(position, input.find_first_of(delim, position) - position);

    if (str.empty()) {
      throw value_parse_exception::range_exception{"expected value", position};
    }

    if (auto value = value_parser<T>::parse(str)) {
      *it = *value;
    } else {
      throw value_parse_exception::range_exception{"unable to parse item",
        position, str.size()};
    }

    position += str.size() + 1;
  }

  if (Size > 1 && it == buffer.begin() + 1) {
    std::ranges::fill(buffer, buffer[0]);
  } else if (it != buffer.end()) {
    throw value_parse_exception::range_exception{"expected more items", input.size()};
  } else if (position < input.size()) {
    throw value_parse_exception::range_exception{"found excess items", position,
      input.size() - position};
  }

  return buffer;
}

}

#endif // ICONFIGP_ARRAY_HPP_INCLUDED
