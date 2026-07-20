// Copyright (C) 2025  darkoned12000
// SPDX-License-Identifier: GPL-3.0-or-later
// Part of the ltheory-old-test modernization effort (Revamp Work).
// See NOTICE and LICENSE.GPL. Original engine (c) Josh Parnell, public domain.
// Substantial modification: added ParticleSystem_Add_Position declaration (Position/Vec3d).

#ifndef LTE_ParticleSystem_h__
#define LTE_ParticleSystem_h__

#include "BaseType.h"
#include "DeclareFunction.h"
#include "Reference.h"
#include "V3.h"

struct ParticleSystemT : public RefCounted {
  BASE_TYPE(ParticleSystemT)

  virtual void Draw(DrawState* state) const = 0;
  virtual void Run(float dt) = 0;
};

DeclareFunction(ParticleSystem_Add, void,
  ParticleSystem, particleSystem,
  ShaderInstance, particle,
  V3D, position,
  V3, velocity,
  float, scale,
  float, life,
  V3, attribute)

/* Overload accepting Position (V3D) for velocity/attribute so scripts that work
 * in double-precision Position space (e.g. WarpRail) don't need a V3D -> V3F
 * conversion (which corrupts the engine's global conversion table; see
 * AGENTS.md §8d #1). */
DeclareFunction(ParticleSystem_Add_Position, void,
  ParticleSystem, particleSystem,
  ShaderInstance, particle,
  V3D, position,
  V3D, velocity,
  float, scale,
  float, life,
  V3D, attribute)

DeclareFunctionNoParams(ParticleSystem_Create, ParticleSystem)

DeclareFunctionNoParams(ParticleSystem_Get, ParticleSystem)

DeclareFunctionNoParams(ParticleSystem_Pop, void)

DeclareFunction(ParticleSystem_Push, void,
  ParticleSystem, ps)

#endif
