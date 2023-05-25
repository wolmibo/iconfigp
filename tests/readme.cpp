#include <iconfigp/parser.hpp>
#include <iconfigp/value-parser.hpp>

#include <cassert>

static constexpr std::string_view config = R"(
# This is a comment

global-option = true

[section]
key = value
)";



int main() { // NOLINT(*exception-escape)
  auto root = iconfigp::parser::parse(config);

  // NOLINTNEXTLINE(*optional-access)
  assert(iconfigp::parse<bool>(root.unique_key("global-option")).value() == true);

  if (auto sec = root.subsection("section")) {
    assert(sec->unique_key("key"));
    assert(sec->unique_key("key")->value() == "value");
  } else {
    throw std::runtime_error{"config has no [section]!"};
  }
}
