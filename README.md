# iconfigp

C++ parser library for an inlinable, INI-inspired configuration syntax with verbose
error messages.

## Example

```cpp
#include <iconfigp/parser.hpp>
#include <iconfigp/value-parser.hpp>

#include <cassert>

static constexpr std::string_view config = R"(
# This is a comment

global-option = true

[section]
key = value
)";



int main() {
  auto root = iconfigp::parser::parse(config);

  assert(iconfigp::parse<bool>(root.unique_key("global-option")).value() == true);

  if (auto sec = root.subsection("section")) {
    assert(sec->unique_key("key"));
    assert(sec->unique_key("key")->value() == "value");
  } else {
    throw std::runtime_error{"config has not [section]!"};
  }
}
```
