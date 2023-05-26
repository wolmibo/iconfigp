# iconfigp File Format

The configuration format used by iconfigp is based on INI.


## Example File

```ini
# This is a comment
key = value

- item = 1; option = arg
- item = 2; option = arg

[section]
key = "value"

[section.subsection]
key = 'value'
```


## Config File Location

By default, the config file for a program MyProgram will be searched in the following
locations:
* `$MYPROGRAM_CONFIG`
* `$XDG_CONFIG_HOME/myprogram/config.ini`
* `$HOME/.config/myprogram/config.ini`


## File Syntax

The following syntactical elements are recognized:
* **Comments**: introduced by `#` at the beginning of a line until the end of that line
* Named **sections**: introduced by a section header `[name of section]` until the
  next section header or until cleared by returning to the global section via `[]`.
  Subsections are separated by a `.`: `[section.subsection]`
* Anonymous **groups**: introduced by `-` inside a section or by repeating the section
  header and cleared at the next `-` or next section
* **Key-value** pairs: introduced by a string, separated by `=` and ended at the end of
  line, a section header, at a group divider `-`, or with a semicolon `;`

All keys, values, and (sub)section names are strings subject to the follow rules:
* when started with a single quotation mark `'` everything after this character up to
  the next `'` is the content of the string
* when started with a double quotation mark `"` everything after this character up to
  the next (unescaped) `"` is the content of the string and escape sequences get
  resolved
* if no quotation marks are used, leading and trailing white space get trimmed and
  escape sequences get resolved
* the escape sequence `\n` gets resolved as new line, while any other sequence `\<char>`
  gets resolved as `<char>`

In particular, the following are equivalent:
```ini
number=10
number = 10
number = "10"
number = \1\0
number = "1\0"
number = '10'
```


## Built-In Value Types

### Boolean

The following values are recognized as booleans (case insensitive):
* `false`, `f`, `no`, `n`, `0`
* `true`, `t`, `yes`, `y`, `1`


### Numbers

Currently the three floating point types **f32**, **f64**, **f128** and
the eight integer types **i8**, **u8**, **i16**, **u16**, **i32**, **u32**,
**i64**, **u64** are supported. They are parsed using
[`std::from_chars`](https://en.cppreference.com/w/cpp/utility/from_chars).

Examples of allowed values:
* **i32**: `-42`, `0`, `42`
* **f32**: `-42`, `-42.0`, `0`, `42`, `42.0`

As usual the range of
* an unsigned integer type **u*****n*** is `0 ... 2^n - 1`
* a signed integer type **i*****n*** is `-2^(n - 1) ... 2^(n - 1) - 1`


### File Paths

File paths can be specified as:
* absolute paths
* `$HOME`-relative paths starting with `~/`
* relative paths:
   - relative to the configuration file
   - relative to the execution directory


### Colors

Colors are specified as RGBA hexadecimal codes with optional alpha channel and
optional leading `#`: `[#]rrggbb[aa]`.
For example: `#4488ff`, `2a3a4b`, `#4488ff80`, `2a3a4b80`.

Single digits per color channel are also allowed: `[#]rgb[a]`, e.g.,
`48f` is equivalent to `#4488ffff`.
