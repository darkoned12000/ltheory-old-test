// Copyright (C) 2025  darkoned12000
// SPDX-License-Identifier: GPL-3.0-or-later
// Part of the ltheory-old-test modernization effort (Revamp Work).
// See NOTICE and LICENSE.GPL. Original engine (c) Josh Parnell, public domain.
//
// Regression guard for the "Reference<unknown type>" static-initialization
// corruption (see AGENTS.md §7 / §8d #2, #13). The abstract RefCounted bases
// RNGT / FontT / InterfaceT are only ever used through Reference<X>; if they are
// not reflected, Type_Get<X>() falls through to the generic "unknown type"
// fallback, which caches a Type with null function pointers. That made
// Reference<RNGT> resolve to "Reference<unknown type>" and any conversion to it
// dereference a null assign pointer (Access Violation). These tests assert the
// types resolve to their real, named types instead.

#include "Harness.h"
#include "LTE/Type.h"
#include "LTE/Reference.h"
#include "LTE/RNG.h"
#include "LTE/Font.h"
#include "LTE/Vector.h"
#include "UI/Interface.h"

using namespace LTE;

static bool IsUnknownType(Type const& t) {
  return t->name == "unknown type";
}

LTE_TEST(Type_ReflectedAbstractBasesAreNotUnknown) {
  // Each abstract base used only through Reference<> must resolve to a real
  // named type, never the generic "unknown type" fallback.
  LTE_CHECK(!IsUnknownType(Type_Get<RNGT>()));
  LTE_CHECK(!IsUnknownType(Type_Get<FontT>()));
  LTE_CHECK(!IsUnknownType(Type_Get<InterfaceT>()));

  LTE_CHECK_EQ(Type_Get<RNGT>()->name, String("RNGT"));
  LTE_CHECK_EQ(Type_Get<FontT>()->name, String("FontT"));
  LTE_CHECK_EQ(Type_Get<InterfaceT>()->name, String("InterfaceT"));
}

LTE_TEST(Type_ReferenceToAbstractBasesAreNotUnknown) {
  // The key regression: Reference<RNGT> used to resolve to
  // "Reference<unknown type>" (with null function pointers).
  Type rngRef = Type_Get<Reference<RNGT> >();
  Type fontRef = Type_Get<Reference<FontT> >();
  Type ifaceRef = Type_Get<Reference<InterfaceT> >();

  LTE_CHECK(!IsUnknownType(rngRef));
  LTE_CHECK(!IsUnknownType(fontRef));
  LTE_CHECK(!IsUnknownType(ifaceRef));

  LTE_CHECK_EQ(rngRef->name, String("Reference<RNGT>"));
  LTE_CHECK_EQ(fontRef->name, String("Reference<FontT>"));
  LTE_CHECK_EQ(ifaceRef->name, String("Reference<InterfaceT>"));
}

LTE_TEST(Type_PrimitiveResolution) {
  // Sanity checks that ordinary primitives still resolve to real, named types
  // (never the "unknown type" fallback). We assert non-emptiness rather than a
  // specific name so the test is robust to the engine's primitive naming.
  Type i = Type_Get<int>();
  Type r = Type_Get<float>();
  Type s = Type_Get<String>();
  LTE_CHECK(!IsUnknownType(i));
  LTE_CHECK(!IsUnknownType(r));
  LTE_CHECK(!IsUnknownType(s));
  LTE_CHECK(i->name.size() > 0);
  LTE_CHECK(!IsUnknownType(Type_Get<Vector<int> >()));
}

LTE_TEST(Type_SharedCacheIsConsistent) {
  // AGENTS §8d #13: the type cache was historically a per-TU function-local
  // static, so the exe and dll could hold *different* Type objects for the same
  // C++ type T, breaking `Type_Get<T>() == Type_Get<T>()` identity across the
  // boundary (launch.cpp:62-66). The cache is now a single dll-owned map keyed
  // by std::type_index. These tests assert: (a) repeated Type_Get<T>() returns
  // the SAME Type identity, and (b) the name-map lookup (Type_Find) yields the
  // exact same object Type_Get<T>() returned -- proving there is one canonical
  // instance per type, not a duplicated "unknown type" per translation unit.
  Type i1 = Type_Get<int>();
  Type i2 = Type_Get<int>();
  LTE_CHECK(i1 == i2);                       // identity preserved across calls
  LTE_CHECK(i1 == Type_Find(i1->name));     // name-map == cache object

  Type s1 = Type_Get<String>();
  Type s2 = Type_Get<String>();
  LTE_CHECK(s1 == s2);
  LTE_CHECK(s1 == Type_Find(s1->name));

  // A reflected composite type must also resolve to exactly one object.
  Type v1 = Type_Get<Vector<int> >();
  Type v2 = Type_Get<Vector<int> >();
  LTE_CHECK(v1 == v2);
  LTE_CHECK(v1 == Type_Find(v1->name));
}
