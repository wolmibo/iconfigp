// Copyright (c) 2023 wolmibo
// SPDX-License-Identifier: MIT

#ifndef ICONFIGP_PATH_HPP_INCLUDED
#define ICONFIGP_PATH_HPP_INCLUDED

#include "iconfigp/value-parser.hpp"

#include <filesystem>

#include <unistd.h>



namespace iconfigp {

template<> struct value_parser<std::filesystem::path> {
  static constexpr std::string_view name{"file path"};

  static std::string_view format() {
    return "(<string> | ~<string>)";
  }

  static std::optional<std::filesystem::path> parse(std::string_view);
};



void clear_preferred_root_path();
void preferred_root_path(const std::filesystem::path&);

[[nodiscard]] std::optional<std::filesystem::path> preferred_root_path();

}

#endif // ICONFIGP_PATH_HPP_INCLUDED
