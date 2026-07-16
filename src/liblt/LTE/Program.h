#ifndef LTE_Program_h__
#define LTE_Program_h__

#include "Window.h"

struct Program {
  Window window;
  bool deleted;

  LT_API Program();
  LT_API virtual ~Program();

  LT_API void Delete();
  LT_API void Execute();

  virtual void OnInitialize() = 0;
  virtual void OnUpdate() = 0;
  virtual void OnDelete() {}
};

/* Returns the program currently being executed by Program::Execute, or null. */
LT_API Program* Program_GetCurrent();

LT_API bool Program_InStaticSection();

#endif
