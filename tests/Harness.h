// Copyright (C) 2025  darkoned12000
// SPDX-License-Identifier: GPL-3.0-or-later
// Part of the ltheory-old-test modernization effort (Revamp Work).
// See NOTICE and LICENSE.GPL. Original engine (c) Josh Parnell, public domain.

#ifndef LTE_TESTS_Harness_h__
#define LTE_TESTS_Harness_h__

// Minimal self-contained test harness for the LTE core. No external framework:
// each test file registers itself via LTE_TEST and the main() runner executes
// them, tallying pass/fail. Kept dependency-free so the tests link only against
// the engine shared library (bin/liblt.so) and need no display / audio device.

#include <cstdio>
#include <cstring>
#include <functional>
#include <string>
#include <vector>

namespace LTE {
namespace Tests {

struct Registry {
  struct Entry {
    char const* file;
    char const* name;
    std::function<void()> fn;
  };
  static std::vector<Entry>& entries() {
    static std::vector<Entry> v;
    return v;
  }
  static int& failures() {
    static int f = 0;
    return f;
  }
  static int& checks() {
    static int c = 0;
    return c;
  }
};

inline void Register(char const* file, char const* name, std::function<void()> fn) {
  Registry::entries().push_back({file, name, std::move(fn)});
}

struct AutoRegister {
  AutoRegister(char const* file, char const* name, std::function<void()> fn) {
    Register(file, name, std::move(fn));
  }
};

// Assertion helpers. On failure they record the failure and continue so that a
// single test reports all its broken checks rather than stopping at the first.
inline void Check(bool cond, char const* expr, char const* file, int line) {
  Registry::checks()++;
  if (!cond) {
    Registry::failures()++;
    std::fprintf(stderr, "  FAIL %s:%d: %s\n", file, line, expr);
  }
}

}  // namespace Tests
}  // namespace LTE

#define LTE_TEST(name)                                                          \
  static void LTE_Test_##name();                                                \
  static ::LTE::Tests::AutoRegister LTE_TestReg_##name(                         \
    __FILE__, #name, &LTE_Test_##name);                                        \
  static void LTE_Test_##name()

#define LTE_CHECK(cond) ::LTE::Tests::Check((cond), #cond, __FILE__, __LINE__)
#define LTE_CHECK_EQ(a, b)                                                      \
  ::LTE::Tests::Check((a) == (b), #a " == " #b, __FILE__, __LINE__)

#endif
