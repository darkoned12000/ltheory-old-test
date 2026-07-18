#ifndef LTE_Environment_h__
#define LTE_Environment_h__

#include "AutoClass.h"
#include "Data.h"
#include "Expression.h"
#include "Map.h"
#include "ScriptType.h"
#include "Stack.h"
#include "Vector.h"

namespace LTE {
  struct Environment {
    uint base;
    Vector<void*> registers;

    /* Set by a `return` expression; signals enclosing blocks to stop
       evaluating further expressions and propagate the return value up
       to the calling function. Checked by ExpressionBlock::Evaluate. */
    bool returnSignal;

    /* Canonical return-value slot for the innermost enclosing function
       call. Set by ScriptFunctionT::Call (and ExpressionT::Evaluate) so a
       `return` anywhere in the body writes directly here, regardless of how
       deeply nested it is. Null for void / unknown return slots. */
    void* returnValue;

    Environment() : base(0), returnSignal(false), returnValue(nullptr) {}

    inline void* Allocate(Type const& type) {
      return type->Allocate();
    }

    inline void Free(Type const& type, void* ptr) {
      type->Deallocate(ptr);
    }
  };

  AutoClass(Variable,
    String, name,
    uint, registerIndex,
    int, offset,
    Type, type,
    bool, constant,
    bool, reference)

    Variable() = default;
  };

  struct CompileEnvironment {
    Pointer<ScriptT> script;
    Vector<ScriptType> context;
    Map<String, Stack<Variable> > variables;
    int registers;
    bool hasErrors;
    bool detail;

    CompileEnvironment() :
      registers(0),
      hasErrors(false),
      detail(false)
      {}

    uint Allocate(
      String const& name,
      Type const& type,
      bool constant,
      bool reference)
    {
      uint registerIndex = registers++;
      variables[name].push(Variable(name, registerIndex, -1, type, constant, reference));
      return registerIndex;
    }

    bool Contains(String const& name) {
      return variables[name].size() > 0;
    }

    void Free(String const& name) {
      variables[name].pop();
      registers--;
    }

    Variable& Get(String const& name) {
      return variables[name].back();
    }
  };
}

#endif
