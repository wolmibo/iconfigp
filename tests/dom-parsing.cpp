#include "iconfigp/exception.hpp"
#include <iconfigp/parser.hpp>

#include <iostream>

#include <cassert>


static constexpr std::string_view example = R"(
poll-rate-ms= 100
fade-out-ms= 250
fade-in-ms=1000
key-with-dash=Yes

[panels]
- anchor= lbr; size=0x22; margin=0

[wallpaper] # a commentary
path= background.png
scale= zoom
";"=fane

[wallpaper]
enable-if=app_id == "foot"
color= "#000000"
- filter= box-blur; radius= 64; iterations= 2
- filter= lens-blur; radius= 128; iterations= 0

[e-DP1.panels]
-anchor=trl;size=0x0
margin=0:0:0:0

[]
dither= 0
)";


int main() { // NOLINT(readability-function-cognitive-complexity)
  try {
    auto root = iconfigp::parser::parse(example);

    assert(root.unique_key("poll-rate-ms") .value().value() == "100");
    assert(root.unique_key("fade-out-ms")  .value().value() == "250");
    assert(root.unique_key("fade-in-ms")   .value().value() == "1000");
    assert(root.unique_key("key-with-dash").value().value() == "Yes");
    assert(root.unique_key("dither")       .value().value() == "0");

    assert(!root.unique_key("foo"));

    assert(root.count_keys("fade-out-ms") == 1);
    assert(root.count_keys("foo")         == 0);

    assert(root.groups().size() == 2);

    assert(root.groups()[0].count_keys("key-with-dash") == 1);
    assert(root.groups()[0].entries().size() == 4);
    assert(root.groups()[0].entries()[0].key() == "poll-rate-ms");

    assert(root.subsections()[0].name() == "panels");
    auto panels = root.subsection("panels").value();
    assert(panels.unique_key("anchor").value().value() == "lbr");
    assert(panels.unique_key("size")  .value().value() == "0x22");
    assert(panels.unique_key("margin").value().value() == "0");

    auto wallpaper = root.subsection("wallpaper").value();
    assert(wallpaper.unique_key("enable-if").value().value() == "app_id == \"foot\"");
    assert(wallpaper.groups()[0].entries().back().key() == ";");
    assert(wallpaper.count_keys("filter") == 2);

    size_t filter_count{0};
    for (const auto& gp: wallpaper.groups()) {
      if (gp.count_keys("filter") > 0) {
        filter_count++;
        assert(std::stoi(std::string{gp.unique_key("radius").value().value()}) % 64 == 0);
      }
    }
    assert(filter_count == 2);

    assert(root.subsection("e-DP1").value().subsection("panels").value().unique_key("size").value().value() == "0x0");

  } catch (const iconfigp::exception& ex) {
    std::cout << iconfigp::format_exception(ex, example, true) << std::endl;
  }

  std::string_view content{"[section] key1 = value1; key2 = value2; - key1 = value2; key2= value1 [section2]"};
  try {
    auto root = iconfigp::parser::parse(content);
  } catch (const iconfigp::exception& ex) {
    std::cout << iconfigp::format_exception(ex, content, true) << std::endl;
  }



}
