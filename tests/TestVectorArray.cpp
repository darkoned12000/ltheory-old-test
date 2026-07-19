// Copyright (C) 2025  darkoned12000
// SPDX-License-Identifier: GPL-3.0-or-later
// Part of the ltheory-old-test modernization effort (Revamp Work).
// See NOTICE and LICENSE.GPL. Original engine (c) Josh Parnell, public domain.

#include "Harness.h"
#include "LTE/Vector.h"
#include "LTE/Array.h"

using namespace LTE;

LTE_TEST(Vector_PushAndIndex) {
  Vector<int> v;
  LTE_CHECK(v.empty());
  LTE_CHECK_EQ(v.size(), size_t(0));

  v.push(10);
  v.push(20);
  v.push(30);
  LTE_CHECK_EQ(v.size(), size_t(3));
  LTE_CHECK_EQ(v[0], 10);
  LTE_CHECK_EQ(v[2], 30);
  LTE_CHECK_EQ(v.back(), 30);
  LTE_CHECK_EQ(v.front(), 10);

  v[1] = 99;
  LTE_CHECK_EQ(v[1], 99);
}

LTE_TEST(Vector_AppendAndOperator) {
  Vector<int> a;
  a.push(1);
  a.push(2);
  Vector<int> b;
  b.push(3);
  b.push(4);
  a.append(b);
  LTE_CHECK_EQ(a.size(), size_t(4));
  LTE_CHECK_EQ(a[3], 4);

  Vector<int> c;
  c = 7;                       // assign single element
  LTE_CHECK_EQ(c.size(), size_t(1));
  LTE_CHECK_EQ(c[0], 7);

  a << 5 << 6;                 // stream-insert
  LTE_CHECK_EQ(a.size(), size_t(6));
  LTE_CHECK_EQ(a[5], 6);
}

LTE_TEST(Vector_EraseAndInsert) {
  Vector<int> v;
  for (int i = 0; i < 5; ++i)
    v.push(i);
  v.erase(2);                  // remove the '2'
  LTE_CHECK_EQ(v.size(), size_t(4));
  LTE_CHECK_EQ(v[2], 3);

  v.insert(1, 42);
  LTE_CHECK_EQ(v.size(), size_t(5));
  LTE_CHECK_EQ(v[1], 42);
  LTE_CHECK_EQ(v[2], 1);
}

LTE_TEST(Vector_ResizeValueInitializes) {
  Vector<int> v;
  v.resize(4);
  LTE_CHECK_EQ(v.size(), size_t(4));
  LTE_CHECK_EQ(v[0], 0);

  v.resize(2, 9);
  LTE_CHECK_EQ(v.size(), size_t(2));
  LTE_CHECK_EQ(v[0], 0);

  Vector<int> w;
  w.resize(3, 5);
  LTE_CHECK_EQ(w.size(), size_t(3));
  LTE_CHECK_EQ(w[2], 5);
}

LTE_TEST(Array_BasicOperations) {
  Array<int> a(4);
  LTE_CHECK_EQ(a.size(), size_t(4));
  for (size_t i = 0; i < 4; ++i)
    a[i] = (int)(i * 10);
  LTE_CHECK_EQ(a[3], 30);

  // iterator
  int sum = 0;
  for (Array<int>::IteratorType it = a.begin(); it != a.end(); ++it)
    sum += *it;
  LTE_CHECK_EQ(sum, 60);
}
