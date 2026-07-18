#include "../CodeObjects.h"

#include "LTE/Pool.h"
#include "LTE/Script.h"

#include "UI/Widget.h"

namespace {
  AutoClassDerived(CodeObjectCustom, CodeObjectT,
    Data, instance,
    ScriptFunction, getName,
    ScriptFunction, getType,
    ScriptFunction, getWidget)
    DERIVED_TYPE_EX(CodeObjectCustom)
    POOLED_TYPE

    CodeObjectCustom() = default;

    String GetName() const override {
      return "Custom Code Object";
    }

    String GetType() const override {
      return "unknown";
    }

    Widget GetWidget() const override {
      return nullptr;
    }
  };
}

DefineFunction(CodeObject_Custom) {
  Reference<CodeObjectCustom> self = new CodeObjectCustom;
  return self;
}
