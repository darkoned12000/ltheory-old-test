#ifndef Component_Log_h__
#define Component_Log_h__

#include "Common.h"
#include "LTE/AutoClass.h"
#include "LTE/String.h"
#include "LTE/Vector.h"

AutoClass(LogEntry,
  Time, time,
  String, message,
  float, importance)

  LogEntry() = default;

  LogEntry(String const& message, float importance) :
    time(Universe_Age()),
    message(message),
    importance(importance)
    {}
};

AutoClass(ComponentLog,
  Vector<LogEntry>, elements)

  ComponentLog() = default;

  void Add(String const& message, float importance) {
    elements.push(LogEntry(message, importance));
  }
};

AutoComponent(Log)
  void AddLogMessage(String const& message, float importance = 0) override {
    Log.Add(message, importance);
  }
};

#endif
