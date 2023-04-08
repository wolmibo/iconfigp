// Copyright (c) 2023 wolmibo
// SPDX-License-Identifier: MIT

#ifndef ICONFIGP_SPACE_HPP_INCLUDED
#define ICONFIGP_SPACE_HPP_INCLUDED

#include <algorithm>
#include <string>
#include <string_view>



namespace iconfigp {

[[nodiscard]] constexpr bool is_space(char c) {
  // compare https://en.cppreference.com/w/cpp/string/byte/isspace
  return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}



[[nodiscard]] constexpr std::string_view trim_front(std::string_view input) {
  return {std::ranges::find_if_not(input, is_space), input.end()};
}



[[nodiscard]] constexpr std::string_view trim_back(std::string_view input) {
  while (!input.empty() && is_space(input.back())) {
    input.remove_suffix(1);
  }
  return input;
}



[[nodiscard]] constexpr std::string_view trim(std::string_view input) {
  return trim_back(trim_front(input));
}

}

#endif // ICONFIGP_SPACE_HPP_INCLUDED
