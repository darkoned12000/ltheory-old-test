#include "Mission.h"

#include "Game/Object.h"

namespace {
  AutoClassDerived(MissionConstraintRange, MissionConstraintT,
    ItemProperty, property,
    Data, lower,
    Data, upper)
    DERIVED_TYPE_EX(MissionConstraintRange)
    MissionConstraintRange() = default;

    double Evaluate(Item const& data) const override {
      return 1.0;
    }

    String GetDescription() const override {
      return "";
    }
  };

  AutoClassDerived(MissionConstraintEquality, MissionConstraintT,
    ItemProperty, property,
    Data, value)
    DERIVED_TYPE_EX(MissionConstraintRange)
    MissionConstraintEquality() = default;

    double Evaluate(Item const& data) const override {
      return 1.0;
    }

    String GetDescription() const override {
      return Stringize() | property->GetName() | " is " | (*(Object*)value.data)->GetName();
    }
  };
}

MissionConstraint MissionConstraint_Equality(
  ItemProperty const& property,
  Data const& value)
{
  return new MissionConstraintEquality(property, value);
}

DefineFunction(Mission_Create) {
  Mission self = new MissionT;
  self->owner = args.owner;
  return self;
} FunctionAlias(Mission_Create, Mission);
