#include <iconfigp/color.hpp>
#include <iconfigp/exception.hpp>
#include <iconfigp/key-value.hpp>
#include <iconfigp/path.hpp>
#include <iconfigp/value-parser.hpp>

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <array>

std::ostream& operator<<(std::ostream& out, const iconfigp::rgba_f32& me) {
  return out << '(' << me[0] << ", " << me[1] << ", "
    << me[2] << ", " << me[3] << ')';
}

template<typename T>
std::string to_string(std::optional<T> value) {
  std::stringstream out;
  if (value) {
    out << *value;
  } else {
    out << "{}";
  }
  return out.str();
}

template<typename T>
void parse_value(std::string_view input, std::optional<T> correct) {
  iconfigp::key_value kv{iconfigp::located_string{"foo"}, iconfigp::located_string{std::string{input}}};

  try {
    auto value = iconfigp::parse<T>(kv);
    if (value != correct) {
      std::cout << input << " was incorrectly parsed as "
        << value
        << " instead of " << to_string(correct) << std::endl;
      throw std::runtime_error{"incorrectly parsed"};
    }
  } catch (iconfigp::value_parse_exception& ex) {
    if (correct) {
      std::cout << input << '\n' << std::flush;
      throw;
    }
  }
}


enum class my_enum {
  foo,
  bar
};

std::ostream& operator<<(std::ostream& out, my_enum me) {
  return out << static_cast<int>(me);
}


template<>
struct iconfigp::case_insensitive_parse_lut<my_enum> {
  static constexpr std::string_view name {"my_enum"};
  static constexpr std::array<std::pair<std::string_view, my_enum>, 2> lut = {
    std::make_pair("foo", my_enum::foo),
    std::make_pair("bar", my_enum::bar)
  };
};



int main() { // NOLINT(*exception-escape)
  parse_value<std::string_view>("hello world", "hello world");

  parse_value<bool>("yes",  true);
  parse_value<bool>("yES",  true);
  parse_value<bool>("y",    true);
  parse_value<bool>("Y",    true);
  parse_value<bool>("1",    true);
  parse_value<bool>("tRUe", true);
  parse_value<bool>("t",    true);

  parse_value<bool>("rue", {});
  parse_value<bool>("",    {});

  parse_value<bool>("NO",    false);
  parse_value<bool>("no",    false);
  parse_value<bool>("n",     false);
  parse_value<bool>("F",     false);
  parse_value<bool>("0",     false);
  parse_value<bool>("falsE", false);
  parse_value<bool>("f",     false);

  // NOLINTBEGIN(*-magic-numbers)

  parse_value<float>("0.5", 0.5);
  parse_value<double>("0.5", 0.5);
  parse_value<long double>("0.5", 0.5);

  parse_value<float>("-0.5", -0.5);
  parse_value<float>("-0",   -0);
  parse_value<float>("0",     0);
  parse_value<float>("-5",   -5);
  parse_value<float>("5",     5);
  parse_value<float>("5.",    5);
  parse_value<float>("-5.",  -5.);
  parse_value<float>(".5",    0.5);
  parse_value<float>("-.5",  -.5);

  parse_value<float>("-.5e+0",  -.5);
  parse_value<float>("-.5e+0",  -.5);
  parse_value<float>("-.5e0",  -.5);

  parse_value<float>("foo", {});
  parse_value<float>("5f",  {});
  parse_value<float>("+5",  {});

  parse_value<int8_t>  ("0", 0);
  parse_value<uint8_t> ("0", 0);
  parse_value<int16_t> ("0", 0);
  parse_value<uint16_t>("0", 0);
  parse_value<int32_t> ("0", 0);
  parse_value<uint32_t>("0", 0);
  parse_value<int64_t> ("0", 0);
  parse_value<uint64_t>("0", 0);

  parse_value<int>         ("0", 0);
  parse_value<unsigned int>("0", 0);
  parse_value<size_t>      ("0", 0);

  parse_value<int>("100", 100);
  parse_value<int>("-100", -100);

  parse_value<uint8_t>("10.", {});
  parse_value<uint8_t>("256", {});
  parse_value<uint8_t>("-1",  {});



  parse_value<my_enum>("foo", my_enum::foo);
  parse_value<my_enum>("FOO", my_enum::foo);
  parse_value<my_enum>("bar", my_enum::bar);

  parse_value<my_enum>("fo", {});



  parse_value<iconfigp::rgba_f32>("",    {});
  parse_value<iconfigp::rgba_f32>("  ",  {});
  parse_value<iconfigp::rgba_f32>("foo", {});
  parse_value<iconfigp::rgba_f32>("#ff", {});
  parse_value<iconfigp::rgba_f32>("#abcdefgh", {});
  parse_value<iconfigp::rgba_f32>("#ffffff",   iconfigp::rgba_f32{1., 1., 1., 1.});
  parse_value<iconfigp::rgba_f32>("#ffFf",     iconfigp::rgba_f32{1., 1., 1., 1.});
  parse_value<iconfigp::rgba_f32>("#f0f",      iconfigp::rgba_f32{1., 0., 1., 1.});
  parse_value<iconfigp::rgba_f32>(" f0f",      iconfigp::rgba_f32{1., 0., 1., 1.});
  parse_value<iconfigp::rgba_f32>("#000000",   iconfigp::rgba_f32{0., 0., 0., 1.});
  parse_value<iconfigp::rgba_f32>("#00000000", iconfigp::rgba_f32{0., 0., 0., 0.});

  parse_value<std::filesystem::path>("/root", "/root");


  // NOLINTEND(*-magic-numbers)
}
