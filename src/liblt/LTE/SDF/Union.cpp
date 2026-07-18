#include "../SDFs.h"

#include "LTE/Bound.h"
#include "LTE/Math.h"

namespace {
  AutoClassDerived(SDFUnion, SDFT,
    SDF, a,
    SDF, b,
    float, sharpness)
    DERIVED_TYPE_EX(SDFUnion)

    SDFUnion() = default;

    float Evaluate(V3 const& p) const override {
      float at = a->Evaluate(p);
      float bt = b->Evaluate(p);
      return Mix(at, bt, Sigmoid((at - bt) * sharpness));
    }

    Bound3 GetBound() const override {
      return a->GetBound().Union(b->GetBound());
    }

    String GetCode(const String& p) const override {
      return Stringize()
        | "smoothUnion(" | a->GetCode(p) | ", " | b->GetCode(p) | ", "
        | sharpness | ")";
    }
  };

  DERIVED_IMPLEMENT(SDFUnion)

  AutoClassDerived(SDFUnion0, SDFT,
    SDF, a,
    SDF, b)
    DERIVED_TYPE_EX(SDFUnion0)

    SDFUnion0() = default;

    float Evaluate(V3 const& p) const override {
      return Min(a->Evaluate(p), b->Evaluate(p));
    }

    Bound3 GetBound() const override {
      return a->GetBound().Union(b->GetBound());
    }

    String GetCode(const String& p) const override {
      return "min(" + a->GetCode(p) + ", " + b->GetCode(p) + ")";
    }
  };

  DERIVED_IMPLEMENT(SDFUnion0)
}

DefineFunction(SDF_Union) {
  if (args.sharpness <= 0)
    return new SDFUnion0(args.a, args.b);
  else
    return new SDFUnion(args.a, args.b, args.sharpness);
}

SDF SDFT::Union(SDF const& other, float sharpness) {
  return SDF_Union(this, other, sharpness);
}
