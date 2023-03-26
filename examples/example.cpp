#include <iconfigp/serialize.hpp>
#include <iconfigp/exception.hpp>
#include <iconfigp/section.hpp>
#include <iconfigp/parser.hpp>

#include <iostream>

#include <cassert>

static constexpr size_t line_width{90};

static constexpr std::string_view example = R"(
poll-rate-ms= 100
fade-out-ms= 250
fade-in-ms= 1000
key-with-dash= Yes

[panels]
- anchor= lbr; size= 0x22; margin= 0

[wallpaper]
# a comment
path= background.png
scale= zoom
";"=fane

[wallpaper]
color= #000000
- filter= box-blur; radius= 64; iterations= 2
- filter= lens-blur; radius= 128; iterations= 0

[e-DP1.panels]
-anchor=trl;size=0x0
margin=0:0:0:0

[]
dither= 0
)";

int main() {
  try {
    auto root = iconfigp::parser::parse(example);
    //std::cout << root.to_string() << std::endl;
    //
    assert(root.unique_key("fade-out-ms").value().value() == "250");

    if (auto msg = iconfigp::generate_unused_message(root, example, true)) {
      std::cout << *msg << std::flush;
    }

    std::cout << std::string(line_width, '-') << std::endl;
    std::cout << iconfigp::serialize(root) << std::flush;
    std::cout << std::string(line_width, '-') << std::endl;


    // query global outputs

    //auto poll_rate = parse<int>(root.get_unique("poll-rate-ms")).value_or(100);
    //root.count_keys("poll-rate-ms");





  } catch (iconfigp::syntax_exception& ex) {
    std::cout << iconfigp::format_exception(ex, example, true) << std::flush;
  }
}
