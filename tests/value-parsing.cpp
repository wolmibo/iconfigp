#include <iconfigp/array.hpp>
#include <iconfigp/color.hpp>
#include <iconfigp/exception.hpp>
#include <iconfigp/key-value.hpp>
#include <iconfigp/path.hpp>
#include <iconfigp/value-parser.hpp>

#include <sstream>
#include <stdexcept>
#include <iostream>

std::ostream& operator<<(std::ostream& out, const iconfigp::rgba_f32& me) {
  return out << '(' << me[0] << ", " << me[1] << ", "
    << me[2] << ", " << me[3] << ')';
}

std::ostream& operator<<(std::ostream& out, std::span<const float> me) {
  for (const auto& item: me) {
    out << item << ' ';
  }
  return out;
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

template<size_t Size>
void parse_array(std::string_view input, const std::vector<float>& correct) {
  try {
    auto result = iconfigp::parse_as_array<float, Size>(input);

    if (!std::ranges::equal(correct, result)) {
      std::cout << input << " was incorrectly parsed as "
        << std::span{result}
        << " instead of " << std::span{correct} << std::endl;
    }
  } catch (iconfigp::value_parse_exception::range_exception& ex) {
    if (!correct.empty()) {
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


ICONFIGP_DEFINE_ENUM_LUT(my_enum,
    "foo", foo,
    "bar", bar
)



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

  parse_array<4>("1.0",                 {1.F, 1.F, 1.F, 1.F});
  parse_array<4>("1.0,0.0",             {});
  parse_array<4>("0.0,0.0,1.0,0.0",     {0.F, 0.F, 1.F, 0.F});
  parse_array<4>("0.0,0.0,1.0,0.0,1.0", {});
  parse_array<4>("0.0,0.0,1.0",         {});
  parse_array<4>(",0.0,0.0,1.0",        {});


  // NOLINTEND(*-magic-numbers)
}
