#include "../Tasks.h"

#include "Game/Object.h"

#include "LTE/Math.h"
#include "LTE/Pool.h"
#include "LTE/StackFrame.h"

namespace {
  AutoClassDerived(TaskWait, TaskT, Task_Wait_Args, args)
    DERIVED_TYPE_EX(TaskWait)
    POOLED_TYPE

    TaskWait() = default;

    float GetDuration() const override {
      return args.duration;
    }

    String GetName() const override {
      return "Wait";
    }

    bool IsFinished(Object const& self, Data const& data) const override {
      return data.Convert<float>() >= args.duration;
    }

    void OnBegin(Object const& self, Data& data) override {
      data = 0.0f;
    }

    void OnUpdate(Object const& self, float dt, Data& data) override { AUTO_FRAME;
      data.Convert<float>() += dt;
    }
  };
}

DefineFunction(Task_Wait) {
  return new TaskWait(args);
}
