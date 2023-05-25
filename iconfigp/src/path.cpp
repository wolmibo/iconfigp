#include "iconfigp/path.hpp"

#include <unistd.h>



//NOLINTBEGIN(*-global-variables)
namespace { namespace global_state {
  std::optional<std::filesystem::path> root_path{};
}}
//NOLINTEND(*-global-variables)



void iconfigp::clear_preferred_root_path() {
  global_state::root_path = {};

}



void iconfigp::preferred_root_path(const std::filesystem::path& path) {
  global_state::root_path = std::filesystem::absolute(path);
}



std::optional<std::filesystem::path> iconfigp::preferred_root_path() {
  return global_state::root_path;
}





std::optional<std::filesystem::path> iconfigp::value_parser<std::filesystem::path>::parse(
    std::string_view input
) {
  if (input.starts_with('~')) {
    //NOLINTNEXTLINE(*-mt-unsafe)
    if (auto* home = getenv("HOME")) {
      input.remove_prefix(1);
      return std::filesystem::path{home} / input;
    }
  }

  std::filesystem::path path{input};

  if (path.is_absolute()) {
    return path;
  }

  if (auto root = preferred_root_path()) {
    if (auto abs = *root / input; std::filesystem::exists(abs)) {
      return abs;
    }
  }

  return std::filesystem::absolute(path);
}
