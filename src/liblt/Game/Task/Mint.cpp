#include "../Tasks.h"

#include "Game/Events.h"
#include "Game/Icons.h"
#include "Game/Object.h"
#include "Game/Item/Blueprint.h"

#include "LTE/Math.h"
#include "LTE/Pool.h"
#include "LTE/StackFrame.h"

namespace {
  AutoClassDerived(TaskMint, TaskT,
    Task_Mint_Args, args)
    DERIVED_TYPE_EX(TaskMint)
    POOLED_TYPE

    TaskMint() = default;

    float GetDuration() const override {
      return args.blueprint->GetValue();
    }

    Icon GetIcon() const override {
      return Icon_Disc();
    }

    String GetName() const override {
      return "Mint Assembly Chip";
    }

    String GetNoun() const override {
      return "Assembly Chip Lab";
    }

    Capability GetRateFactor() const override {
      return Capability_Research(1);
    }

    void OnUpdate(Object const& self, float dt, Data& data) override { AUTO_FRAME;
      if (RandExp() < dt) {
        Blueprint* bp = (Blueprint*)args.blueprint.t;
        self->GetRoot()->AddItem(bp->assemblyChip, 1);
      }
    }
  };
}

DefineFunction(Task_Mint) {
  return new TaskMint(args);
}
