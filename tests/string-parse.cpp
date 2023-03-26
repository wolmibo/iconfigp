#include <iconfigp/exception.hpp>
#include <iconfigp/located-string.hpp>
#include <iconfigp/reader.hpp>

#include <iostream>
#include <stdexcept>
#include <string>


[[nodiscard]] std::string to_string(const iconfigp::located_string& ls) {
  return std::to_string(ls.offset()) + ":" + std::to_string(ls.size())
    + ":%" + std::string{ls.content()} + "%";
}



void print_exception(const iconfigp::syntax_exception& ex, std::string_view input) {
  std::cout << iconfigp::format_exception(ex, input, false) << std::flush;
}



void test(const std::string& input, const iconfigp::located_string& correct) {
  iconfigp::reader read{input};

  try {
    auto parsed = read.read_until_one_of(",;#\n");

    if (parsed != correct) {
      throw std::runtime_error{
        "test failed: %" + input + "% was parsed as " + to_string(parsed)
          + " instead of " + to_string(correct)
      };
    }

  } catch (iconfigp::syntax_exception& ex) {
    print_exception(ex, input);
    throw std::runtime_error{"unable to parse"};
  }
}



void test_exception(const std::string& input, const iconfigp::syntax_exception& ex1) {
  try {
    iconfigp::reader read{input};
    auto parsed = read.read_until_one_of(",;#\n");
  } catch (iconfigp::syntax_exception& ex2) {
    if (ex1.type() == ex2.type() && ex1.offset() == ex2.offset()) {
      return;
    }
    print_exception(ex2, input);
  }
  throw std::runtime_error{"failed to get correct exception"};
}



int main() { // NOLINT(*exception-escape)
  // NOLINTBEGIN(*-magic-numbers)
  test("foo",                 iconfigp::located_string{"foo",     0, 3});
  test("fo\\no",              iconfigp::located_string{"fo\no",   0, 5});
  test("fo\\\"o",             iconfigp::located_string{"fo\"o",   0, 5});
  test("  foo bar ",          iconfigp::located_string{"foo bar", 2, 7});
  test("  foo  #",            iconfigp::located_string{"foo",     2, 3});
  test("foo#",                iconfigp::located_string{"foo",     0, 3});
  test("foo\\#",              iconfigp::located_string{"foo#",    0, 5});
  test("foo\\##",             iconfigp::located_string{"foo#",    0, 5});
  test("foo\\#  #",           iconfigp::located_string{"foo#",    0, 5});

  test("\"  foo bar \"",      iconfigp::located_string{"  foo bar ",  0, 12});
  test("  \"  fo'o' bar\"  ", iconfigp::located_string{"  fo'o' bar", 2, 13});
  test(R"("fo\"o")",          iconfigp::located_string{"fo\"o",       0, 7});
  test("\"foo\"#",            iconfigp::located_string{"foo",         0, 5});
  test("\"foo\" #",           iconfigp::located_string{"foo",         0, 5});
  test(R"("foo\#"#)",         iconfigp::located_string{"foo#",        0, 7});
  test("\"foo#\"#",           iconfigp::located_string{"foo#",        0, 6});

  test("'  foo bar '",        iconfigp::located_string{"  foo bar ",    0, 12});
  test("  '  fo\"o\" bar'  ", iconfigp::located_string{"  fo\"o\" bar", 2, 13});
  test("'fo\\\"o'",           iconfigp::located_string{"fo\\\"o",       0, 7});
  test("'foo'#",              iconfigp::located_string{"foo",           0, 5});
  test("'foo' #",             iconfigp::located_string{"foo",           0, 5});
  test("'foo\\#'#",           iconfigp::located_string{"foo\\#",        0, 7});
  test("'foo#'#",             iconfigp::located_string{"foo#",          0, 6});

  test_exception("  'foo ",      iconfigp::syntax_exception{iconfigp::syntax_error_type::missing_quotation_mark, 2});
  test_exception("  \"foo ",     iconfigp::syntax_exception{iconfigp::syntax_error_type::missing_quotation_mark, 2});

  test_exception("  'foo \n bar'",   iconfigp::syntax_exception{iconfigp::syntax_error_type::missing_quotation_mark_eol, 2});
  test_exception("  \"foo \n bar\"", iconfigp::syntax_exception{iconfigp::syntax_error_type::missing_quotation_mark_eol, 2});

  test_exception("  'foo' f ",   iconfigp::syntax_exception{iconfigp::syntax_error_type::unexpected_character, 8});
  test_exception("  \"foo\" f ", iconfigp::syntax_exception{iconfigp::syntax_error_type::unexpected_character, 8});

  test_exception("escape\\",     iconfigp::syntax_exception{iconfigp::syntax_error_type::invalid_escape_sequence, 6});

  test("\\#000000\n-",   iconfigp::located_string{"#000000", 0, 8});
  test("\"#000000\"\n-", iconfigp::located_string{"#000000", 0, 9});
  test("'#000000'\n-",   iconfigp::located_string{"#000000", 0, 9});

  // NOLINTEND(*-magic-numbers)
}
