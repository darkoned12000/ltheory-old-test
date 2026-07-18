#include "Lock.h"

#include "SFML/System/Mutex.hpp"

namespace {
  struct LockImpl : public LockT {
    sf::Mutex mutex;

    void Acquire() override {
      mutex.lock();
    }

    void Release() override {
      mutex.unlock();
    }
  };
}

Lock Lock_Create() {
  return new LockImpl;
}
