// Copyright (C) 2025  darkoned12000
// SPDX-License-Identifier: GPL-3.0-or-later
// Part of the ltheory-old-test modernization effort (Revamp Work).
// See NOTICE and LICENSE.GPL. Original engine (c) Josh Parnell, public domain.
// Substantial modification: replaced static-init TypeAlias(Position, Position)
// with lazy dll-side Position_RegisterConstructor() binding "Position" -> V3D.

#include "Game/Common.h"
#include "Game/Object.h"
#include "Game/Player.h"

#include "LTE/Function.h"
#include "LTE/Renderable.h"
#include "LTE/Script.h"
#include "LTE/Transform.h"

#include "UI/Widget.h"

TypeAlias(Reference<ObjectT>, Object);
TypeAlias(Reference<PlayerT>, Player);
TypeAlias(HashT, HashT);

/* Position === V3D === V3T<double> (see Game/Common.h). We bind the script name
 * "Position" to the (fully operator-populated) Vec3d type.
 *
 * IMPORTANT (AGENTS.md §8d #1): binding Position via the static-init TypeAlias
 * macro (which calls Type_Get<Position>()) triggers the static-initialization-
 * order fiasco that corrupts the global registry (every numeric literal like
 * '0.5' starts failing with "variable name '0.5' not found"). The fix is lazy /
 * late registration. We use a dll-side __attribute__((constructor)) so this runs
 * at dll load, AFTER all dll static-init (so V3D is fully populated) and BEFORE
 * any script is compiled -- avoiding both the SIOF and re-entrancy with the
 * compiler. We reach the type via Type_Get<V3D>() (safe; V3.cpp resolves it
 * cleanly) and never call Type_Get<Position>(). */
static void Position_RegisterScriptAPI() {
  Type_AddAlias(Type_Get<V3D>(), "Position");
}

__attribute__((constructor))
static void Position_RegisterConstructor() {
  Position_RegisterScriptAPI();
}

DefineConversion(object_to_player, Object, Player) {
  dest = (Player)src;
}

DefineConversion(player_to_object, Player, Object) {
  dest = (Object)src;
}

VoidFreeFunction(Object_AddChild,
  "Add 'child' to 'object'",
  Object, object,
  Object, child)
{
  object->AddChild(child);
} FunctionAlias(Object_AddChild, AddChild);

VoidFreeFunction(Object_Attach,
  "Attach 'child' to 'object' with 'localTransform'",
  Object, object,
  Object, child,
  Transform, localTransform)
{
  object->Attach(child, localTransform);
} FunctionAlias(Object_Attach, Attach);

VoidFreeFunction(Object_Broadcast,
  "Broadcast 'message' to 'object'",
  Object, object,
  Data, message)
{
  object->Broadcast(Mutable(message));
} FunctionAlias(Object_Broadcast, Broadcast);

VoidFreeFunction(Object_Delete,
  "Delete 'object' from the game world",
  Object, object)
{
  object->Delete();
} FunctionAlias(Object_Delete, Delete);

FreeFunction(bool, Object_Equal,
  "Return a == b",
  Object, a,
  Object, b)
{
  return a == b;
} FunctionAlias(Object_Equal, ==);

FreeFunction(Object, Object_GetContainer,
  "Return the container within which 'object' exists",
  Object, object)
{
  return object->GetContainer().t;
} FunctionAlias(Object_GetContainer, GetContainer);

FreeFunction(DistanceT, Object_GetDistance,
  "Return the distance from 'a' to 'b'",
  Object, a,
  Object, b)
{
  return Length(a->GetPos() - b->GetPos());
} FunctionAlias(Object_GetDistance, GetDistance);

FreeFunction(HashT, Object_GetHash,
  "Return a hash for 'object'",
  Object, object)
{
  return object->GetHash();
} FunctionAlias(Object_GetHash, GetHash);

FreeFunction(Icon, Object_GetIcon,
  "Return the icon for 'object'",
  Object, object)
{
  return object->GetIcon();
} FunctionAlias(Object_GetIcon, GetIcon);

FreeFunction(ObjectID, Object_GetID,
  "Return the globally-unique ID of 'object'",
  Object, object)
{
  return object->GetID();
} FunctionAlias(Object_GetID, GetID);

FreeFunction(float, Object_GetRadius,
  "Return the world-space radius of 'object'",
  Object, object)
{
  return object->GetRadius();
} FunctionAlias(Object_GetRadius, GetRadius);

FreeFunction(Renderable, Object_GetRenderable,
  "Return the renderable for 'object'",
  Object, object)
{
  return object->GetRenderable();
} FunctionAlias(Object_GetRenderable, GetRenderable);

FreeFunction(Object, Object_GetRoot,
  "Return the root object of 'object'",
  Object, object)
{
  return object->GetRoot().t;
} FunctionAlias(Object_GetRoot, GetRoot);

FreeFunction(uint, Object_GetSeed,
  "Return the seed value of 'object'",
  Object, object)
{
  return object->GetSeed();
} FunctionAlias(Object_GetSeed, GetSeed);

FreeFunction(Item, Object_GetSupertype,
  "Return the item supertype of 'object'",
  Object, object)
{
  return object->GetSupertype();
} FunctionAlias(Object_GetSupertype, GetSupertype);

FreeFunction(Object, Object_GetSystem,
  "Return the system within which 'object' exists",
  Object, object)
{
  return (ObjectT*)object->GetSystem().t;
} FunctionAlias(Object_GetSystem, GetSystem);

FreeFunction(Traits, Object_GetTraits,
  "Return the personality traits of 'object'",
  Object, object)
{
  return object->GetTraits();
} FunctionAlias(Object_GetTraits, GetTraits);

FreeFunction(String, Object_GetType,
  "Return the type of 'object'",
  Object, object)
{
  return object->GetTypeString();
} FunctionAlias(Object_GetType, GetType);

FreeFunction(Widget, Object_GetWidget,
  "Return the object-specific widget for 'object' from 'player's point-of-view",
  Object, object,
  Player, player)
{
  return object->GetWidget(player);
} FunctionAlias(Object_GetWidget, GetWidget);

FreeFunction(bool, Object_NotEqual,
  "Return a != b",
  Object, a,
  Object, b)
{
  return a != b;
} FunctionAlias(Object_NotEqual, !=);

VoidFreeFunction(Object_RemoveChild,
  "Remove 'child' from 'object'",
  Object, object,
  Object, child)
{
  object->RemoveChild(child);
} FunctionAlias(Object_RemoveChild, RemoveChild);

VoidFreeFunction(Object_Update,
  "Run one iteration of 'object's update logic, with time step 'dt'",
  Object, object,
  float, dt)
{
  UpdateState state(dt, true);
  object->Update(state);
} FunctionAlias(Object_Update, Update);

VoidFreeFunction(Object_Send,
  "Send 'message' to 'object'",
  Object, object,
  Data, message)
{
  object->Send(Mutable(message));
} FunctionAlias(Object_Send, Send);

namespace Parent {
  AutoClass(ObjectChildIterator,
    Object, object,
    Object, child)
    ObjectChildIterator() = default;
  };

  FreeFunction(ObjectChildIterator, Object_GetChildren,
    "Return an iterator to the children of 'object'",
    Object, object)
  {
    return ObjectChildIterator(object, object->children);
  } FunctionAlias(Object_GetChildren, GetChildren);

  VoidFreeFunction(ObjectChildIterator_Advance,
    "Advance 'iterator'",
    ObjectChildIterator, iterator)
  {
    Mutable(iterator).child = iterator.child->nextSibling;
  } FunctionAlias(ObjectChildIterator_Advance, Advance);

  FreeFunction(Object, ObjectChildIterator_Get,
    "Return the contents of 'iterator'",
    ObjectChildIterator, iterator)
  {
    return iterator.child;
  } FunctionAlias(ObjectChildIterator_Get, Get);

  FreeFunction(bool, ObjectChildIterator_HasMore,
    "Return whether 'iterator' has more elements",
    ObjectChildIterator, iterator)
  {
    return iterator.child != nullptr;
  } FunctionAlias(ObjectChildIterator_HasMore, HasMore);
}

#define Z(x, y, z)                                                             \
  FreeFunction(bool, Object_Is##y,                                             \
  "Return whether 'object' is a " z,                                           \
  Object, object)                                                              \
  {                                                                            \
    return object->GetType() == ObjectType_##y;                                \
  } FunctionAlias(Object_Is##y, Is##y);
OBJECT_X
#undef Z
