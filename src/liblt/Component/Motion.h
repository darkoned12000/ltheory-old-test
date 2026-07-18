#ifndef Component_Motion_h__
#define Component_Motion_h__

#include "Common.h"
#include "Orientation.h"
#include "Game/Item.h"
#include "LTE/AutoClass.h"
#include "LTE/V3.h"

const float kLinearDrag = 0.8f;
const float kAngularDrag = 2;

/* TODO : Reduce memory usage. */

AutoClass(ComponentMotion,
  V3, force,
  V3, torque,
  V3, velocity,
  V3, velocityA,
  Mass, mass,
  Mass, inertia,
  float, speed)

  ComponentMotion() :
    force(0),
    torque(0),
    velocity(0),
    velocityA(0),
    mass(1),
    inertia(1),
    speed(0)
    {}

  LT_API void Run(ObjectT* self, UpdateState& state);
};

AutoComponent(Motion)
  void OnUpdate(UpdateState& s) override {
    Motion.Run(this, s);
    BaseT::OnUpdate(s);
  }

  void SetSupertype(Item const& type) override {
    LTE_ASSERT(type->GetMass() > 0);
    Motion.mass = type->GetMass();
    Motion.inertia = type->GetInertia();

    BaseT::SetSupertype(type);
  }

  void ApplyForce(V3 const& force) override {
    Motion.force += force;
  }

  void ApplyTorque(V3 const& torque) override {
    Motion.torque += torque;
  }

  V3 GetAngularVelocity() const override {
    return Motion.velocityA;
  }

  Mass GetMass() const override {
    return Motion.mass;
  }

  float GetSpeed() const override {
    return Motion.speed;
  }

  V3 GetVelocity() const override {
    return Motion.velocity;
  }

  V3 GetVelocityA() const override {
    return Motion.velocityA;
  }

  float GetTopSpeed() const override {
    return this->GetMaxThrust() / (kLinearDrag * Motion.mass);
  }

  bool IsMovable() const override {
    return true;
  }
};

#endif
