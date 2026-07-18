#include "../SDFs.h"

#include "LTE/Bound.h"
#include "LTE/Math.h"

namespace {
  AutoClassDerived(SDFTranslate, SDFT,
    SDF, source,
    V3, offset)
    DERIVED_TYPE_EX(SDFTranslate)

    SDFTranslate() = default;

    float Evaluate(V3 const& p) const override {
      return source->Evaluate(p - offset);
    }

    Bound3 GetBound() const override {
      Bound3 bound = source->GetBound();
      return Bound3(bound.lower + offset, bound.upper + offset);
    }

    String GetCode(String const& p) const override {
      return source->GetCode(Stringize() | "(" | p | " - " | offset | ")");
    }
  };

  DERIVED_IMPLEMENT(SDFTranslate)
}

DefineFunction(SDF_Translate) {
  return new SDFTranslate(args.source, args.offset);
} FunctionAlias(SDF_Translate, +);
