#include "simdutf.h"

#include <array>
#include <iostream>

#include <tests/helpers/random_int.h>
#include <tests/helpers/test.h>
#include <tests/helpers/transcode_test_base.h>

namespace {
std::array<size_t, 7> input_size{7, 16, 12, 64, 67, 128, 256};

using simdutf::tests::helpers::transcode_utf8_to_utf32_test_base;

constexpr size_t trials = 10000;
} // namespace

TEST(convert_pure_ASCII) {
  for (size_t trial = 0; trial < trials; trial++) {
    if ((trial % 100) == 0) {
      std::cout << ".";
      std::cout.flush();
    }
    size_t counter = 0;
    auto generator = [&counter]() -> uint32_t { return counter++ & 0x7f; };

    auto procedure = [&implementation](const char *utf8, size_t size,
                                       char32_t *utf32) -> size_t {
      return implementation.convert_utf8_to_utf32(utf8, size, utf32);
    };
    auto size_procedure = [&implementation](const char *utf8,
                                            size_t size) -> size_t {
      return implementation.utf32_length_from_utf8(utf8, size);
    };

    for (size_t size : input_size) {
      transcode_utf8_to_utf32_test_base test(generator, size);
      ASSERT_TRUE(test(procedure));
      ASSERT_TRUE(test.check_size(size_procedure));
    }
  }
}

TEST(convert_1_or_2_UTF8_bytes) {
  for (size_t trial = 0; trial < trials; trial++) {
    uint32_t seed{1234 + uint32_t(trial)};
    if ((trial % 100) == 0) {
      std::cout << ".";
      std::cout.flush();
    }
    simdutf::tests::helpers::RandomInt random(
        0x0000, 0x07ff, seed); // range for 1 or 2 UTF-8 bytes

    auto procedure = [&implementation](const char *utf8, size_t size,
                                       char32_t *utf32) -> size_t {
      return implementation.convert_utf8_to_utf32(utf8, size, utf32);
    };
    auto size_procedure = [&implementation](const char *utf8,
                                            size_t size) -> size_t {
      return implementation.utf32_length_from_utf8(utf8, size);
    };
    for (size_t size : input_size) {
      transcode_utf8_to_utf32_test_base test(random, size);
      ASSERT_TRUE(test(procedure));
      ASSERT_TRUE(test.check_size(size_procedure));
    }
  }
}

TEST(convert_1_or_2_or_3_UTF8_bytes) {
  for (size_t trial = 0; trial < trials; trial++) {
    uint32_t seed{1234 + uint32_t(trial)};
    if ((trial % 100) == 0) {
      std::cout << ".";
      std::cout.flush();
    }
    // range for 1, 2 or 3 UTF-8 bytes
    simdutf::tests::helpers::RandomIntRanges random(
        {{0x0000, 0xd7ff}, {0xe000, 0xffff}}, seed);

    auto procedure = [&implementation](const char *utf8, size_t size,
                                       char32_t *utf32) -> size_t {
      return implementation.convert_utf8_to_utf32(utf8, size, utf32);
    };
    auto size_procedure = [&implementation](const char *utf8,
                                            size_t size) -> size_t {
      return implementation.utf32_length_from_utf8(utf8, size);
    };
    for (size_t size : input_size) {
      transcode_utf8_to_utf32_test_base test(random, size);
      ASSERT_TRUE(test(procedure));
      ASSERT_TRUE(test.check_size(size_procedure));
    }
  }
}

TEST(convert_3_or_4_UTF8_bytes) {
  for (size_t trial = 0; trial < trials; trial++) {
    uint32_t seed{1234 + uint32_t(trial)};
    if ((trial % 100) == 0) {
      std::cout << ".";
      std::cout.flush();
    }
    simdutf::tests::helpers::RandomIntRanges random(
        {{0x0800, 0xd800 - 1}, {0xe000, 0x10ffff}},
        seed); // range for 3 or 4 UTF-8 bytes

    auto procedure = [&implementation](const char *utf8, size_t size,
                                       char32_t *utf32) -> size_t {
      return implementation.convert_utf8_to_utf32(utf8, size, utf32);
    };
    auto size_procedure = [&implementation](const char *utf8,
                                            size_t size) -> size_t {
      return implementation.utf32_length_from_utf8(utf8, size);
    };
    for (size_t size : input_size) {
      transcode_utf8_to_utf32_test_base test(random, size);
      ASSERT_TRUE(test(procedure));
      ASSERT_TRUE(test.check_size(size_procedure));
    }
  }
}

TEST(convert_null_4_UTF8_bytes) {
  for (size_t trial = 0; trial < trials; trial++) {
    uint32_t seed{1234 + uint32_t(trial)};
    if ((trial % 100) == 0) {
      std::cout << ".";
      std::cout.flush();
    }
    simdutf::tests::helpers::RandomIntRanges random(
        {{0x0000, 0x00000}, {0x10000, 0x10ffff}},
        seed); // range for 3 or 4 UTF-8 bytes

    auto procedure = [&implementation](const char *utf8, size_t size,
                                       char32_t *utf32) -> size_t {
      return implementation.convert_utf8_to_utf32(utf8, size, utf32);
    };

    for (size_t size : input_size) {
      transcode_utf8_to_utf32_test_base test(random, size);
      ASSERT_TRUE(test(procedure));
    }
  }
}

TEST(convert_invalid_special_cases) {
  std::string source =
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x02\x2e\x00\x00\x00\x02\x00\x00"
      "\x00\x0c\x00\x02\x02\x2e\x2e\x00\x00\x0b\x00\x00\x00\x14\x00\x0a\x02\x6c"
      "\x6f\x73\x74\x2b\x66\x6f\x75\x6e\x64\x00\x00\x0c\x00\x00\x00\x14\x00\x0b"
      "\x02\x61\x5f\x64\x69\x72\x65\x63\x74\x6f\x72\x79\x00\x0e\x00\x00\x00\x18"
      "\x00\x0d\x01\x70\x61\x73\x73\x77\x6f\x72\x64\x73\x2e\x74\x78\x74\x00\x00"
      "\x00\x10\x00\x00\x00\x9c\x03\x06\x07\x61\x5f\x6c\x69\x6e\x6b\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff\xff\xff\xff\x02\x00\x00\x00\x0c"
      "\x00\x01\x02\x2e\x00\x00\x00\x02\x00\x00\x00\x0c\x00\x02\x02\x2e\x2e\x00"
      "\x00\x0b\x00\x00\x00\x14\x00\x0a\x02\x6c\x6f\x73\x74\x2b\x66\x6f\x75\x6e"
      "\x64\x00\x00\x0c\x00\x00\x00\x14\x00\x0b\x02\x61\x5f\x64\x69\x72\x65\x63"
      "\x74\x6f\x72\x79\x00\x0e\x00\x00\x00\x18\x00\x0d\x01\x70\x61\x73\x73\x77"
      "\x6f\x72\x64\x73\x2e\x74\x78\x74\x00\x00\x00\x10\x00\x00\x00\x9c\x03\x06"
      "\x07\x61\x5f\x6c\x69\x6e\x6b\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff"
      "\xff\xff\xff\xff\xff\x02\x00\x00\x00\x0c\x00\x01\x02\x2e\x00\x00\x00\x02"
      "\x00\x00\x00\x0c\x00\x02\x02\x2e\x2e\x00\x00\x0b\x00\x00\x00\x14\x00\x0a"
      "\x02\x6c\x6f\x73\x74\x2b\x66\x6f\x75\x6e\x64\x00\x00\x0c\x00\x00\x00\x14"
      "\x00\x0b\x02\x61\x5f\x64\x69\x72\x65\x63\x74\x6f\x72\x79\x00\x0e\x00\x00"
      "\x00\x18\x00\x0d\x01\x70\x61\x73\x73\x77\x6f\x72\x64\x73\x2e\x74\x78\x74"
      "\x00\x00\x00\x10\x00\x00\x00\x9c\x03\x06\x07\x61\x5f\x6c\x69\x6e\x6b\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff\xff\xff\xff\x02\x00\x00"
      "\x00\x0c\x00\x01\x02\x2e\x00\x00\x00\x02\x00\x00\x00\x0c\x00\x02\x02\x2e"
      "\x2e\x00\x00\x0b\x00\x00\x00\x14\x00\x0a\x02\x6c\x6f\x73\x74\x2b\x66\x6f"
      "\x75\x6e\x64\x00\x00\x0c\x00\x00\x00\x14\x00\x0b\x02\x61\x5f\x64\x69\x72"
      "\x65\x63\x74\x6f\x72\x79\x00\x0e\x00\x00\x00\x18\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x10\x72\x64\x73\x2e\x74\x78\x74\x00\x00\x00\x10\x00\x00\x00\x9c"
      "\x03\x06\x07\x61\x5f\x6c\x69\x6e\x6b\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\xff\xff\xff\xff\xff\xff\xff\x02\x00\x00\x00\x0c\x00\x01\x02\x2e\x00\x00"
      "\x00\x02\x00\x00\x00\x0c\x00\x02\x02\x2e\x2e\x00\x00\x0b\x00\x00\x00\x14"
      "\x00\x0a\x02\x6c\x6f\x73\x74\x2b\x66\x6f\x75\x6e\x64\x00\x00\x0c\x00\x00"
      "\x00\x14\x00\x0b\x02\x61\x5f\x64\x69\x72\x65\x63\x74\x6f\x72\x79\x00\x0e"
      "\x00\x00\x00\x18\x00\x0d\x01\x70\x61\x73\x73\x77\x70\x72\x64\x73\x2e\x74"
      "\x78\x74\x00\x00\x00\x10\x00\x00\x00\x9c\x03\x06\x07\x61\x5f\x6c\x69\x6e"
      "\x6b\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff\xff\xff\xff\x02"
      "\x00\x00\x00\x0c\x00\x01\x02\x2e\x00\x00\x00\x02\x00\x00\x00\x0c\x00\x02"
      "\x02\x2e\x2e\x00\x00\x0b\x00\x00\x00\x14\x00\x0a\x02\x6c\x6f\x73\x74\x2b"
      "\x66\x6f\x75\x6e\x64\x00\x00\x0c\x00\x00\x00\x14\x00\x0b\x02\x61\x5f\x64"
      "\x69\x72\x65\x63\x74\x6f\x72\x79\x00\x0e\x00\x00\x00\x18\x00\x0d\x01\x70"
      "\x61\x73\x73\x77\x6f\x72\x64\x73\x2e\x74\x78\x74\x00\x00\x00\x10\x00\x00"
      "\x00\x9c\x03\x06\x07\x61\x5f\x6c\x69\x6e\x6b\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\xff\xff\xff\xff\xff\xff\xff\x02\x00\x00\x00\x0c\x00\x01\x02\x2e"
      "\x00\x00\x00\x02\x00\x00\x00\x0c\x00\x02\x02\x2e\x2e\x00\x00\x0b\x00\x00"
      "\x00\x14\x00\x0a\x02\x6c\x6f\x73\x74\x2b\x66\x6f\x75\x6e\x64\x00\x00\x0c"
      "\x00\x00\x00\x14\x00\x0b\x02\x61\x5f\x64\x69\x72\x65\x63\x74\x6f\x72\x79"
      "\x00\x0e\x00\x00\x00\x18\x00\x0d\x01\x70\x61\x73\x73\x77\x6f\x72\x64\x73"
      "\x2e\x74\x78\x74\x00\x00\x00\x10\x00\x00\x00\x9c\x03\x06\x07\x61";

  // invalid input!!!
  size_t expected_utf32words =
      implementation.utf32_length_from_utf8(source.data(), source.size());
  std::unique_ptr<char32_t[]> utf32_output{new char32_t[expected_utf32words]};
  size_t utf32words = implementation.convert_utf8_to_utf32(
      source.c_str(), source.size(), utf32_output.get());
  ASSERT_TRUE(utf32words == 0);
}

int main(int argc, char *argv[]) { return simdutf::test::main(argc, argv); }
