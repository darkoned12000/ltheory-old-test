#include "../Tasks.h"

#include "Game/Conditions.h"
#include "Game/Items.h"
#include "Game/Player.h"

#include "LTE/Pool.h"
#include "LTE/SDF.h"
#include "LTE/StackFrame.h"

const Distance kDockDistance = 100.0;

namespace {
  AutoClassDerived(TaskDock, TaskT, Task_Dock_Args, args)
    DERIVED_TYPE_EX(TaskDock)
    POOLED_TYPE

    TaskDock() = default;

    String GetName() const override {
      return "Dock";
    }

    Capability GetRateFactor() const override {
      return Capability_Motion(1);
    }

    Object GetTarget() const override {
      return args.target;
    }

    bool IsFinished(Object const& self, Data const&) const override {
      return self->GetContainer() == args.target;
    }

    void OnUpdate(Object const& self, float dt, Data&) override { AUTO_FRAME;
      if (Condition_Nearby(self, args.target, kDockDistance))
        args.target->Dock(self);
      else
        self->PushTask(Task_Goto(args.target, 0.9 * kDockDistance));
    }
  };
}

DefineFunction(Task_Dock) {
  return new TaskDock(args);
}
