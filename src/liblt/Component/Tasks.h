#ifndef Component_Tasks_h__
#define Component_Tasks_h__

#include "Common.h"
#include "Game/Object.h"
#include "Game/Project.h"
#include "Game/Task.h"
#include "LTE/AutoClass.h"
#include "LTE/Vector.h"

AutoClass(ComponentTasks,
  Vector<TaskInstance>, elements,
  Project, project)

  ComponentTasks() = default;

  LT_API void Clear(ObjectT* self);
  LT_API void Run(ObjectT* self, UpdateState& state);
};

AutoComponent(Tasks)
  void OnUpdate(UpdateState& s) override {
    Tasks.Run(this, s);
    BaseT::OnUpdate(s);
  }

  void ClearTasks() override {
    Tasks.elements.clear();
  }

  TaskInstance const* GetCurrentTask() const override {
    return Tasks.elements.size() ? &Tasks.elements.back() : nullptr;
  }

  void OnDeath() override {
    Tasks.Clear(this);
    BaseT::OnDeath();
  }

  void OnMessage(Data& message) override {
    if (Tasks.elements.size())
      Tasks.elements.back().OnMessage(this, message);
    BaseT::OnMessage(message);
  }

  void PushTask(Task const& t) override {
    Tasks.elements.push(TaskInstance(t));
  }
};

#endif
