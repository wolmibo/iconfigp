#include <iconfigp/serialize.hpp>
#include <iconfigp/parser.hpp>

#include <stdexcept>

using namespace std::literals;



static constexpr std::string_view example = R"(
poll-rate-ms= 100
fade-out-ms= 250
fade-in-ms= 1000
key-with-dash= Yes

[panels]
- anchor= lbr; size= 0x22; margin= 0

[wallpaper]
path= background.png # a commentary
scale= zoom
","=fane

[wallpaper]
color= "#000000"
- filter= box-blur; radius= 64; iterations= 2
- filter= lens-blur; radius= 128; iterations= 0

[e-DP1.panels]
-anchor=trl;size=0x0
margin=0:0:0:0

[]
dither= 0
)";


template<typename...Args>
void test(std::string_view correct, Args&&...args) {
  if (auto result = iconfigp::serialize(std::forward<Args>(args)...);
      result != correct) {

    throw std::runtime_error{"%" + std::string{correct}
      + "% was incorrectly serialized as %" + result + "%"};
  }
}

int main() { // NOLINT(*exception-escape)
  test("foo",           "foo"sv);
  test("foo bar",       "foo bar"sv);
  test(R"("foo\nbar")", "foo\nbar"sv);
  test("\" foo\"",      " foo"sv);
  test("\"foo \"",      "foo "sv);
  test("\" foo \"",     " foo "sv);

  test(R"("fo\"o")",    R"(fo"o)"sv);



  auto root1 = iconfigp::parser::parse(example);
  auto res1  = iconfigp::serialize(root1);

  auto root2 = iconfigp::parser::parse(res1);
  auto res2  = iconfigp::serialize(root2);

  if (res1 != res2) {
    throw std::runtime_error{"serialize -> parse -> serialize is not no-op"};
  }
}
