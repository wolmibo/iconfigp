// Copyright (c) 2023 wolmibo
// SPDX-License-Identifier: MIT

#ifndef ICONFIGP_FORMAT_HPP_INCLUDED
#define ICONFIGP_FORMAT_HPP_INCLUDED

#include <version>
#include <chrono> // FIXME: to get __cpp_lib_format without compilation error pre GCC13

#if defined(__cpp_lib_format)
#include <format>
#else
#include <fmt/core.h>
#endif



namespace iconfigp {

namespace impl {
#if defined(__cpp_lib_format)
  namespace format = std;
#else
  namespace format = fmt;
#endif
}

template<typename... Args>
using format_string = impl::format::format_string<Args...>;

template<typename... Args>
std::string format(format_string<Args...> fmt, Args&&... args) {
  return impl::format::format(std::move(fmt), std::forward<Args>(args)...);
}





enum class message_color : int {
  none    = 0,
  warning = 33,
  error   = 31
};

[[nodiscard]] constexpr bool is_color(message_color c) {
  return c != message_color::none;
}



[[nodiscard]] std::string dim      (std::string, bool = false);
[[nodiscard]] std::string emphasize(std::string, bool = false);
[[nodiscard]] std::string colorize (std::string, message_color = message_color::none);



static constexpr size_t max_line_width = 90;

[[nodiscard]] std::string highlight_text_segment(
    std::string_view /*context*/,
    size_t           /*offset*/,
    size_t           /*length*/,
    message_color    /*color*/       = message_color::none,
    bool             /*line_number*/ = true,
    size_t           /*max_width*/   = max_line_width
);





struct text_line {
  std::string_view content;
  size_t           row;
  size_t           column;
};

[[nodiscard]] text_line text_line_from_offset(std::string_view, size_t);

[[nodiscard]] std::optional<size_t> line_offset(std::string_view, size_t);





class exception;

[[nodiscard]] std::string format_exception(
    const exception&,
    std::string_view /*source*/     = "",
    bool             /*colored*/    = false,
    size_t           /*line_width*/ = max_line_width
);





class section;

[[nodiscard]] std::optional<std::string> format_unused_message(
    const section&,
    std::string_view /*source*/    = "",
    bool             /*colored*/   = false,
    size_t           /*max_width*/ = max_line_width
);


}

#endif // ICONFIGP_FORMAT_HPP_INCLUDED
