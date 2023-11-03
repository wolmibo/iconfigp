#include <iconfigp/format.hpp>

#include <cassert>

using namespace iconfigp;



int main() {
  assert(line_offset("one\ntwo\nthree", 0) == 0);
  assert(line_offset("one\ntwo\nthree", 1) == 4);
  assert(line_offset("one\ntwo\nthree", 2) == 8);

  assert(line_offset("\n\n\nfour\n", 3) == 3);
}
