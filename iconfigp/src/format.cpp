// Copyright (c) 2023 wolmibo
// SPDX-License-Identifier: MIT

#include "iconfigp/format.hpp"

#include "iconfigp/exception.hpp"
#include "iconfigp/section.hpp"
#include "iconfigp/serialize.hpp"

#include <optional>
#include <stdexcept>

#include <cstddef>

using namespace iconfigp;





text_line iconfigp::text_line_from_offset(std::string_view text, size_t offset) {
  if (offset >= text.size()) {
    throw std::out_of_range{"offset is outside of content"};
  }

  size_t row{0};

  for (auto pos = text.find('\n'); pos != std::string_view::npos; pos = text.find('\n')) {
    if (offset < pos) {
      return {text.substr(0, pos), row, offset};
    }

    if (offset == pos) {
      return {text.substr(0, pos), row, offset - 1};
    }

    text.remove_prefix(pos + 1);
    offset -= pos + 1;
    ++row;
  }

  return {text, row, offset};
}





std::optional<size_t> iconfigp::line_offset(
    std::string_view text,
    size_t           line
) {
  size_t offset{0};

  for (; line > 0; line--) {
    if (auto pos = text.find('\n'); pos != std::string_view::npos) {
      pos++;

      offset += pos;
      text.remove_prefix(pos);
    } else {
      return {};
    }
  }

  return offset;
}





[[nodiscard]] std::string iconfigp::dim(std::string content, bool colored) {
  if (!colored) {
    return content;
  }
  return "\x1b[2m" + content + "\x1b[0m";
}



[[nodiscard]] std::string iconfigp::colorize(std::string content, message_color color) {
  if (color == message_color::none) {
    return content;
  }
  return iconfigp::format("\x1b[1;{}m{}\x1b[0m", static_cast<int>(color), content);
}



[[nodiscard]] std::string iconfigp::emphasize(std::string content, bool colored) {
  if (!colored) {
    return content;
  }
  return "\x1b[1;37m" + content + "\x1b[0m";
}





namespace {
  [[nodiscard]] std::string emphasize_range(
      std::string_view content,
      size_t           offset,
      size_t           count,
      bool             colored
  ) {
    if (!colored || count == 0 || offset >= content.size()) {
      return std::string{content};
    }

    std::string out;
    size_t end = offset + count;
    for (size_t i = 0; i < content.size();) {
      if (i == offset) { out += "\x1b[1;37m"; }
      out.push_back(content[i]);
      if (++i == end)  { out += "\x1b[0m"; }
    }
    return out;
  }



  constexpr size_t dot_size {3};
  constexpr size_t min_size {dot_size * 2 + 1};



  [[nodiscard]] std::pair<std::string, size_t> center_offset(
      std::string_view line,
      size_t           offset,
      size_t           width
  ) {
    static constexpr size_t rposition{5};

    if (width < min_size) {
      return {"", 0};
    }

    if (line.size() <= width) {
      return {std::string{line}, offset};
    }

    if (offset < width) {
      return {iconfigp::format("{}...", line.substr(0, width - dot_size)), offset};
    }

    if (line.size() - offset < width) {
      return {iconfigp::format("...{}", line.substr(line.size() - width + dot_size)),
        width + offset - line.size()};
    }

    size_t pos = width / rposition;

    return {iconfigp::format("...{}...",
        line.substr(offset + dot_size - pos, width - 2 * dot_size)), pos};
  }





  [[nodiscard]] std::string fallback_range(size_t offset, size_t count) {
    return iconfigp::format("  Position: {}{}\n", offset,
        (count > 0) ? iconfigp::format("-{}", offset + count) : "");
  }





  [[nodiscard]] message_color select_color(bool colored, message_color color) {
    return colored ? color : message_color::none;
  }
}





std::string iconfigp::highlight_text_segment(
    std::string_view content,
    size_t           offset,
    size_t           length,
    message_color    color,
    bool             line_number,
    size_t           max_width
) {
  if (content.empty() || offset + length > content.size()) {
    return fallback_range(offset, length);
  }

  auto line = text_line_from_offset(content, std::min(offset, content.size() - 1));

  auto prefix      = line_number ? format("  {} | ", line.row + 1) : "  ";
  auto prefix_size = prefix.size();

  max_width = (prefix_size >= max_width + min_size) ?
                 min_size : max_width - prefix_size;

  length = std::min(length, line.content.size());

  auto [print, col] = center_offset(line.content, line.column, max_width);

  if (length + col > max_width) {
    length = 0;
  }

  return iconfigp::format("{}{}\n{}{}\n",
      dim(std::move(prefix), is_color(color)),
      emphasize_range(print, col, length, is_color(color)),
      std::string(prefix_size + col, ' '),
      colorize(std::string(std::max<size_t>(length, 1), '^'), color)
  );
}







namespace {
  [[nodiscard]] std::string format_range(
    const value_parse_exception::range_exception& ex,
    std::string_view                              source,
    bool                                          colored,
    size_t                                        max_width,
    bool                                          show_line_number = true
  ) {
    return iconfigp::format("{}:\n{}",
        ex.message(),
        highlight_text_segment(source, ex.offset(), ex.size(),
          select_color(colored, message_color::error), show_line_number, max_width)
    );
  }





  [[nodiscard]] std::string format_missing_key(
    const missing_key_exception& ex,
    std::string_view             source,
    bool                         colored,
    size_t                       max_width
  ) {
    return iconfigp::format("The required key {} is missing in this group:\n{}",
        emphasize(iconfigp::serialize(ex.key()), colored),
        highlight_text_segment(source, ex.offset(), 0,
          select_color(colored, message_color::error), true, max_width)
    );
  }





  [[nodiscard]] std::string format_value_parse(
    const value_parse_exception& ex,
    std::string_view             source,
    bool                         colored,
    size_t                       max_width
  ) {
    return iconfigp::format("{} cannot be parsed as {}:\n{}{}"
        "A value of type {} has the following form:\n{}\n",
        ex.value().value().empty() ? "An empty value" :
          "The value " + emphasize(iconfigp::serialize(ex.value().value()), colored),

        emphasize(iconfigp::serialize(ex.target()), colored),

        highlight_text_segment(source, ex.value().value_offset(), ex.value().value_size(),
          select_color(colored, message_color::error), true, max_width),

        ex.range_ex()
          ? (format_range(*ex.range_ex(), ex.value().value(), colored, max_width, false))
          : "",

        iconfigp::serialize(ex.target()),
        ex.format()
      );
  }





  [[nodiscard]] std::string format_multiple_definitions(
    const multiple_definitions_exception& ex,
    std::string_view                      source,
    bool                                  colored,
    size_t                                max_width
  ) {
    return iconfigp::format("The key {} is only allowed once per {}:\n{}"
        "Previous definition:\n{}{}",

        emphasize(iconfigp::serialize(ex.definition1().key()), colored),

        ex.per_section() ? "section" : "group",

        highlight_text_segment(source, ex.definition2().key_offset(),
          ex.definition2().key_size(),
          select_color(colored, message_color::error), true, max_width),

        highlight_text_segment(source, ex.definition1().key_offset(),
          ex.definition1().key_size(),
          select_color(colored, message_color::warning), true, max_width),

        ex.per_section() ? "" : "Preceed the key with - to start a new group.\n"
    );
  }





  [[nodiscard]] std::string_view error_type_to_string(iconfigp::syntax_error_type type) {
    using enum iconfigp::syntax_error_type;
    switch (type) {
      case missing_quotation_mark:
      case missing_quotation_mark_eol:
        return "Missing a closing quotation mark";
      case invalid_escape_sequence:
        return "Invalid escape sequence";
      case unexpected_character:
        return "Unexpected character";

      case empty_key:
        return "Empty string";
      case unexpected_semicolon:
        return "Unexpected semicolon";
      case missing_section_end:
        return "Missing a matching ]";
      case missing_value:
        return "Missing a value";
    }
    return "Encountered a problem which apparently has no meaningful error message";
  }



  [[nodiscard]] size_t error_type_character_count(iconfigp::syntax_error_type type) {
    using enum iconfigp::syntax_error_type;
    switch (type) {
      case iconfigp::syntax_error_type::missing_quotation_mark:
      case iconfigp::syntax_error_type::missing_quotation_mark_eol:
      case iconfigp::syntax_error_type::invalid_escape_sequence:
      case iconfigp::syntax_error_type::unexpected_character:
      case iconfigp::syntax_error_type::unexpected_semicolon:
      case iconfigp::syntax_error_type::missing_section_end:
        return 1;
      case iconfigp::syntax_error_type::missing_value:
      case iconfigp::syntax_error_type::empty_key:
      default:
        return 0;
    }
  }



  [[nodiscard]] std::optional<std::string_view> error_type_hint(syntax_error_type type) {
    using enum iconfigp::syntax_error_type;

    switch (type) {
      case missing_quotation_mark_eol:
        return "You can write multi-line strings by using the escape sequence \\n.\n";
      case invalid_escape_sequence:
        return "To escape '\\' use \\\\.\n";
      case unexpected_semicolon:
        return "Semicolons only belong right behind a value. Use # for comments.\n";
      case empty_key:
        return "Keys must not be empty.\n";
      case missing_value:
        return "Values are introduced using '='.\n";
      default:
        return {};
    }
  }



  [[nodiscard]] std::string_view task_type_to_string(iconfigp::task_type type) {
    using enum iconfigp::task_type;
    switch (type) {
      case key:      return "a key";
      case value:    return "a value";
      case section:  return "a section header";
      case toplevel: return "a toplevel element";
    }
    return "something";
  }



  [[nodiscard]] std::string format_syntax(
    const iconfigp::syntax_exception& ex,
    std::string_view                  source,
    bool                              colored,
    size_t                            max_width
  ) {
    return iconfigp::format("{} while parsing {}:\n{}{}",
        error_type_to_string(ex.type()),

        task_type_to_string(ex.task()),

        highlight_text_segment(source, ex.offset(), error_type_character_count(ex.type()),
          select_color(colored, message_color::error), true, max_width),

        error_type_hint(ex.type()).value_or("")
    );
  }
}





std::string iconfigp::format_exception(
  const exception& ex,
  std::string_view source,
  bool             colored,
  size_t           max_width
) {
  if (const auto* missing = dynamic_cast<const missing_key_exception*>(&ex)) {
    return format_missing_key(*missing, source, colored, max_width);
  }

  if (const auto* parse = dynamic_cast<const value_parse_exception*>(&ex)) {
    return format_value_parse(*parse, source, colored, max_width);
  }

  if (const auto* multi = dynamic_cast<const multiple_definitions_exception*>(&ex)) {
    return format_multiple_definitions(*multi, source, colored, max_width);
  }

  if (const auto* syntax = dynamic_cast<const syntax_exception*>(&ex)) {
    return format_syntax(*syntax, source, colored, max_width);
  }

  if (const auto* range =
      dynamic_cast<const value_parse_exception::range_exception*>(&ex)) {
    return format_range(*range, source, colored, max_width);
  }

  return ex.what();
}





namespace {
  [[nodiscard]] std::string_view select_number(
      size_t           count,
      std::string_view singular,
      std::string_view plural
  ) {
    return count == 1 ? singular : plural;
  }
}





std::optional<std::string> iconfigp::format_unused_message(
    const section&   sec,
    std::string_view source,
    bool             colored,
    size_t           max_width
) {
  std::string output;

  auto unused_sections = sec.unused_sections();
  if (!unused_sections.empty()) {
    output += select_number(unused_sections.size(),
                "The following section (and its key(s)) has not been used:\n",
                "The following sections (and their key(s)) have not been used:\n");

    for (const auto* un_sec: unused_sections) {
      if (source.empty()) {
        output += iconfigp::format("  {}\n", un_sec->name());
      } else {
        output += highlight_text_segment(source, un_sec->offset(), 0,
            select_color(colored, message_color::warning), true, max_width);
      }
    }
  }



  auto unused_keys = sec.unused_keys();
  if (!unused_keys.empty()) {
    output += select_number(unused_keys.size(),
                "The following key has not been used:\n",
                "The following keys have not been used:\n");

    for (const auto* un_key: unused_keys) {
      if (source.empty()) {
        output += iconfigp::format("  {}\n", un_key->key());
      } else {
        output += highlight_text_segment(source, un_key->key_offset(), un_key->key_size(),
            select_color(colored, message_color::warning), true, max_width);
      }
    }
  }



  if (output.empty()) {
    return {};
  }

  output += "Make sure you have spelled ";
  output += select_number(unused_sections.size() + unused_keys.size(), "it", "them");
  output += " correctly.\n"
            "Alternatively, you can use # to comment out unneeded keys.\n";

  return output;
}
