#include "../SDFs.h"

#include "LTE/Bound.h"
#include "LTE/Math.h"

namespace {
  AutoClassDerived(SDFAdd, SDFT,
    SDF, a,
    SDF, b)
    DERIVED_TYPE_EX(SDFAdd)

    SDFAdd() = default;

    float Evaluate(V3 const& p) const override {
      return a->Evaluate(p) + b->Evaluate(p);
    }

    Bound3 GetBound() const override {
      /* NOTE : There is no good way to
                get a bound on arbitrary density addition. */
      return a->GetBound();
    }

    String GetCode(String const& p) const override {
      return "(" + a->GetCode(p) + " + " + b->GetCode(p) + ")";
    }
  };

  DERIVED_IMPLEMENT(SDFAdd)
}

DefineFunction(SDF_Add) {
  return new SDFAdd(args.a, args.b);
} FunctionAlias(SDF_Add, +);
