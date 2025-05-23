#include "simdutf.h"

#include <array>
#include <vector>

#include <tests/helpers/transcode_test_base.h>
#include <tests/helpers/random_int.h>
#include <tests/helpers/test.h>

namespace {
constexpr std::array<size_t, 7> input_size{7, 16, 12, 64, 67, 128, 256};
constexpr simdutf::endianness BE = simdutf::endianness::BIG;

using simdutf::tests::helpers::transcode_utf32_to_utf16_test_base;

constexpr int trials = 1000;
} // namespace

TEST(issue_convert_utf32_to_utf16be_with_errors_fb5c30a7d5815504) {
  const char32_t data[] = {0x001000ef, 0x335e0200};
  constexpr std::size_t data_len = 2;
  const auto validation1 =
      implementation.validate_utf32_with_errors(data, data_len);
  ASSERT_EQUAL(validation1.count, 1);
  ASSERT_EQUAL(validation1.error, simdutf::error_code::TOO_LARGE);

  const bool validation2 =
      implementation.validate_utf32((const char32_t *)data, data_len);
  ASSERT_EQUAL(validation1.error == simdutf::error_code::SUCCESS, validation2);

  const auto outlen =
      implementation.utf16_length_from_utf32((const char32_t *)data, data_len);
  ASSERT_EQUAL(outlen, 4);
  std::vector<char16_t> output(outlen);
  const auto r = implementation.convert_utf32_to_utf16be_with_errors(
      (const char32_t *)data, data_len, output.data());
  ASSERT_EQUAL(r.error, simdutf::error_code::TOO_LARGE);
  ASSERT_EQUAL(r.count, 1);
}

TEST_LOOP(trials, convert_into_2_UTF16_bytes) {
  // range for 2 UTF-16 bytes
  simdutf::tests::helpers::RandomIntRanges random(
      {{0x0000, 0xd7ff}, {0xe000, 0xffff}}, seed);

  auto procedure = [&implementation](const char32_t *utf32, size_t size,
                                     char16_t *utf16be) -> size_t {
    simdutf::result res = implementation.convert_utf32_to_utf16be_with_errors(
        utf32, size, utf16be);
    ASSERT_EQUAL(res.error, simdutf::error_code::SUCCESS);
    return res.count;
  };
  auto size_procedure = [&implementation](const char32_t *utf32,
                                          size_t size) -> size_t {
    return implementation.utf16_length_from_utf32(utf32, size);
  };
  for (size_t size : input_size) {
    transcode_utf32_to_utf16_test_base test(BE, random, size);
    ASSERT_TRUE(test(procedure));
    ASSERT_TRUE(test.check_size(size_procedure));
  }
}

TEST_LOOP(trials, convert_into_4_UTF16_bytes) {
  // range for 4 UTF-16 bytes
  simdutf::tests::helpers::RandomIntRanges random({{0x10000, 0x10ffff}}, seed);

  auto procedure = [&implementation](const char32_t *utf32, size_t size,
                                     char16_t *utf16be) -> size_t {
    const simdutf::result res =
        implementation.convert_utf32_to_utf16be_with_errors(utf32, size,
                                                            utf16be);
    ASSERT_EQUAL(res.error, simdutf::error_code::SUCCESS);
    return res.count;
  };
  auto size_procedure = [&implementation](const char32_t *utf32,
                                          size_t size) -> size_t {
    return implementation.utf16_length_from_utf32(utf32, size);
  };
  for (size_t size : input_size) {
    transcode_utf32_to_utf16_test_base test(BE, random, size);
    ASSERT_TRUE(test(procedure));
    ASSERT_TRUE(test.check_size(size_procedure));
  }
}

TEST_LOOP(trials, convert_into_2_or_4_UTF16_bytes) {
  // range for 2 or 4 UTF-16 bytes (all codepoints)
  simdutf::tests::helpers::RandomIntRanges random(
      {{0x0000, 0xd7ff}, {0xe000, 0xffff}, {0x10000, 0x10ffff}}, seed);

  auto procedure = [&implementation](const char32_t *utf32, size_t size,
                                     char16_t *utf16be) -> size_t {
    const simdutf::result res =
        implementation.convert_utf32_to_utf16be_with_errors(utf32, size,
                                                            utf16be);
    ASSERT_EQUAL(res.error, simdutf::error_code::SUCCESS);
    return res.count;
  };
  auto size_procedure = [&implementation](const char32_t *utf32,
                                          size_t size) -> size_t {
    return implementation.utf16_length_from_utf32(utf32, size);
  };
  for (size_t size : input_size) {
    transcode_utf32_to_utf16_test_base test(BE, random, size);
    ASSERT_TRUE(test(procedure));
    ASSERT_TRUE(test.check_size(size_procedure));
  }
}

TEST(convert_fails_if_there_is_surrogate) {
  const size_t size = 64;
  transcode_utf32_to_utf16_test_base test(BE, []() { return '*'; }, size + 32);

  for (char32_t surrogate = 0xd800; surrogate <= 0xdfff; surrogate++) {
    for (size_t i = 0; i < size; i++) {
      auto procedure = [&implementation, &i](const char32_t *utf32, size_t size,
                                             char16_t *utf16be) -> size_t {
        const simdutf::result res =
            implementation.convert_utf32_to_utf16be_with_errors(utf32, size,
                                                                utf16be);
        ASSERT_EQUAL(res.error, simdutf::error_code::SURROGATE);
        ASSERT_EQUAL(res.count, i);
        return 0;
      };
      const auto old = test.input_utf32[i];
      test.input_utf32[i] = surrogate;
      ASSERT_TRUE(test(procedure));
      test.input_utf32[i] = old;
    }
  }
}

TEST(convert_fails_if_input_too_large) {
  uint32_t seed{1234};
  simdutf::tests::helpers::RandomInt generator(0x110000, 0xffffffff, seed);

  const size_t size = 64;
  transcode_utf32_to_utf16_test_base test(BE, []() { return '*'; }, size + 32);

  for (size_t j = 0; j < 1000; j++) {
    uint32_t wrong_value = generator();
    for (size_t i = 0; i < size; i++) {
      auto procedure = [&implementation, &i](const char32_t *utf32, size_t size,
                                             char16_t *utf16be) -> size_t {
        const simdutf::result res =
            implementation.convert_utf32_to_utf16be_with_errors(utf32, size,
                                                                utf16be);
        ASSERT_EQUAL(res.error, simdutf::error_code::TOO_LARGE);
        ASSERT_EQUAL(res.count, i);
        return 0;
      };
      auto old = test.input_utf32[i];
      test.input_utf32[i] = wrong_value;
      ASSERT_TRUE(test(procedure));
      test.input_utf32[i] = old;
    }
  }
}

TEST_MAIN
