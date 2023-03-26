#include <iconfigp/exception.hpp>
#include <iconfigp/parser.hpp>
#include <iconfigp/value-parser.hpp>

#include <iostream>
#include <tuple>

static constexpr size_t line_width{90};

template<typename...Args>
void print(const iconfigp::exception& ex, Args&&... args) {
  std::cout << std::string(line_width, '-') << '\n';
  std::cout << iconfigp::format_exception(ex, std::forward<Args>(args)...);
  std::cout << std::string(line_width, '-') << '\n';
}

void test_syntax_message(std::string_view document) {
  try {
    auto root = iconfigp::parser::parse(document);
  } catch (const iconfigp::exception& ex) {
    print(ex, document, true);
    return;
  }
  throw std::runtime_error{"caused no exception: " + std::string{document}};
}

int main() { // NOLINT(*exception-escape)
  test_syntax_message("[section \n without end");
  test_syntax_message("\"quotation \n without end");
  test_syntax_message("'quotation without end");
  test_syntax_message("escape sequence\\");
  test_syntax_message("'stray character' y");
  test_syntax_message("'stray character':,:");
  test_syntax_message(";");
  test_syntax_message("missing value");

  std::string_view doc = "key=value1;key=value2;foo=bar;empty=";
  auto root = iconfigp::parser::parse(doc);
  try {
    std::ignore = root.unique_key("key");
  } catch (const iconfigp::exception& ex) {
    print(ex, doc, true);
  }

  try {
    std::ignore = root.require_unique_key("bar");
  } catch (const iconfigp::exception& ex) {
    print(ex, doc, true);
  }

  try {
    std::ignore = iconfigp::parse<int>(root.unique_key("foo").value());
  } catch (const iconfigp::exception& ex) {
    print(ex, doc, true);
  }

  try {
    std::ignore = iconfigp::parse<bool>(root.unique_key("foo").value());
  } catch (const iconfigp::exception& ex) {
    print(ex, doc, true);
  }

  try {
    std::ignore = iconfigp::parse<bool>(root.unique_key("empty").value());
  } catch (const iconfigp::exception& ex) {
    print(ex, doc, true);
  }

  test_syntax_message("this is a very very very very very very very very very"
      "very long key but is has a missing value ;foo");

  test_syntax_message("this is also a very very very very very very very very very"
      "very long key but is has a missing value ;foo "
      "with way to much stuff behind the position still going"
      "still going still going still going still going still going still going");
}
