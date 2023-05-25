// Copyright (c) 2023 wolmibo
// SPDX-License-Identifier: MIT

#ifndef ICONFIGP_FIND_CONFIG_HPP_INCLUDED
#define ICONFIGP_FIND_CONFIG_HPP_INCLUDED

#include <filesystem>
#include <optional>



namespace iconfigp {

[[nodiscard]] std::optional<std::filesystem::path> find_config_file(std::string_view);

}

#endif // ICONFIGP_FIND_CONFIG_HPP_INCLUDED
