#include "../Expressions.h"

#include "LTE/AutoClass.h"
#include "LTE/Environment.h"
#include "LTE/Pool.h"

namespace {
  AutoClassDerivedEmpty(ExpressionNoop, ExpressionT)
    DERIVED_TYPE_EX(ExpressionNoop)
    POOLED_TYPE

    ExpressionNoop() = default;

    String Emit(Vector<String>& context) const override {
      return "";
    }

    void Evaluate(void* returnValue, Environment& env) const override {}

    Type GetType() const override {
      return Type_Get<void>();
    }

    bool IsConstant(CompileEnvironment& env) const override {
      return true;
    }
  };
}

namespace LTE {
  Expression Expression_Noop() {
    return new ExpressionNoop;
  }
}
