#include "../SDFs.h"

#include "LTE/Bound.h"
#include "LTE/Math.h"

namespace {
  AutoClassDerived(SDFRadial, SDFT,
    SDF, source,
    float, rMin,
    float, rMax)
    DERIVED_TYPE_EX(SDFRadial)

    SDFRadial() = default;

    float Evaluate(V3 const& p) const override {
      return Length(p) - Mix(rMin, rMax, source->Evaluate(p));
    }

    Bound3 GetBound() const override {
      return Bound3(V3(-rMax), V3(rMax));
    }

    String GetCode(String const& p) const override {
      return Stringize()
        | "(length(" | p | ") - mix(" | rMin | ", " | rMax | ", "
        | source->GetCode(p) | "))";
    }
  };

  DERIVED_IMPLEMENT(SDFRadial)
}

DefineFunction(SDF_Radial) {
  return new SDFRadial(args.source, args.rMin, args.rMax);
}
