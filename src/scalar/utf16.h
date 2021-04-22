#ifndef SIMDUTF_UTF16_H
#define SIMDUTF_UTF16_H

namespace simdutf {
namespace scalar {
namespace {
namespace utf16 {

inline size_t count_code_points(const char16_t* buf, size_t len) {
    // We are not BOM aware.
    const uint16_t * p = reinterpret_cast<const uint16_t *>(buf);
    size_t counter{0};
    for(size_t i = 0; i < len; i++) {
        counter += ((p[i] & 0xFC00) != 0xDC00);
    }
    return counter;
}

} // utf16 namespace
} // unnamed namespace
} // namespace scalar
} // namespace simdutf

#endif