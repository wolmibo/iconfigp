#include "iconfigp/find-config.hpp"

#include <unistd.h>



namespace {
  [[nodiscard]] char to_upper(char c) {
    if ('a' <= c && c <= 'z') {
      return static_cast<char>((c - 'a') + 'A');
    }
    return c;
  }



  [[nodiscard]] char to_lower(char c) {
    if ('A' <= c && c <= 'Z') {
      return static_cast<char>((c - 'A') + 'a');
    }
    return c;
  }



  template<typename Trafo>
  [[nodiscard]] std::string join(
      std::string_view name,
      std::string_view suffix,
      Trafo            trafo
  ) {
    std::string output;
    output.reserve(name.size() + suffix.size());

    for (char c: name) {
      output.push_back(trafo(c));
    }

    output += suffix;

    return output;
  }



  [[nodiscard]] std::optional<std::filesystem::path> envpath(const char* var) {
    //NOLINTNEXTLINE(*mt-unsafe)
    if (const char* value = getenv(var); value != nullptr && *value != 0) {
      return std::filesystem::absolute(value);
    }
    return {};
  }
}



std::optional<std::filesystem::path> iconfigp::find_config_file(std::string_view name) {
  if (auto path = envpath(join(name, "_CONFIG", to_upper).c_str())) {
    return path;
  }

  if (auto xdg_config_home = envpath("XDG_CONFIG_HOME")) {
    auto candidate = *xdg_config_home / join(name, "/config.ini", to_lower);
    if (std::filesystem::exists(candidate) && !std::filesystem::is_directory(candidate)) {
      return candidate;
    }
  }

  if (auto home = envpath("HOME")) {
    auto candidate = *home / ".config" / join(name, "/config.ini", to_lower);
    if (std::filesystem::exists(candidate) && !std::filesystem::is_directory(candidate)) {
      return candidate;
    }
  }

  return {};
}
