// Copyright (C) 2025  darkoned12000
// SPDX-License-Identifier: GPL-3.0-or-later
// Part of the ltheory-old-test modernization effort (Revamp Work).
// See NOTICE and LICENSE.GPL. Original engine (c) Josh Parnell, public domain.

#include "Harness.h"
#include "LTE/String.h"

using namespace LTE;

LTE_TEST(String_ConstructAndCompare) {
  String a = "hello";
  String b("world");
  String c = a;

  LTE_CHECK_EQ(a.size(), size_t(5));
  LTE_CHECK_EQ(c, a);
  LTE_CHECK(a != b);
  LTE_CHECK(a < b);            // "hello" < "world" lexicographically
  LTE_CHECK(!(b < a));
}

LTE_TEST(String_Concatenation) {
  String a = "foo";
  String b = "bar";
  String c = a + b;
  LTE_CHECK_EQ(c, String("foobar"));
  LTE_CHECK_EQ(c.size(), size_t(6));

  a += b;
  LTE_CHECK_EQ(a, String("foobar"));

  // operator+(char const*, String)
  String d = String("pre") + "fix";
  LTE_CHECK_EQ(d, String("prefix"));
}

LTE_TEST(String_SubstringAndFind) {
  String s = "Limit Theory";
  LTE_CHECK_EQ(s.substr(0, 5), String("Limit"));
  LTE_CHECK_EQ(s.substr(6), String("Theory"));
  LTE_CHECK_EQ(s.find("Theory"), size_t(6));
  LTE_CHECK_EQ(s.find("xyz"), String::npos);
  LTE_CHECK(s.find("Theory") != String::npos);
}

LTE_TEST(String_CStringConversion) {
  String s = "abc";
  char const* c = (char const*)s;     // operator char const*
  LTE_CHECK(c && std::strcmp(c, "abc") == 0);
}
