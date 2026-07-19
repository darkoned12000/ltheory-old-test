#include "Thread.h"

#include "AutoPtr.h"
#include "Job.h"
#include "Lock.h"

#include <atomic>

#include "SFML/System.hpp"

namespace {
  Lock GetThreadLock() {
    /* NOTE : Locks cannot be static destructed. */
    static Lock* lock = new Lock(Lock_Create());
    return *lock;
  }

  struct ThreadImpl : public ThreadT {
    Job job;
    AutoPtr<sf::Thread> thread;
    std::atomic<bool> finished;
    bool waited;

    ThreadImpl(Job const& job) :
      job(job),
      finished(false),
      waited(false)
    {
      ScopedLock lock(GetThreadLock());
      job->OnBegin();
      thread = new sf::Thread(&ThreadImpl::Run, this);
      thread->launch();
    }

    ~ThreadImpl() override {
      if (!finished.load(std::memory_order_acquire))
        Terminate();
      job->OnEnd();
    }

    Job GetJob() const override {
      return job;
    }

    bool IsFinished() const override {
      return finished.load(std::memory_order_acquire);
    }

    void Run() {
      job->OnRun(UINT_MAX);
      /* Publish the result (written by OnRun via the job's returnValue)
         before marking the thread finished, so a consumer that observes
         IsFinished() == true is guaranteed to also observe the result. */
      finished.store(true, std::memory_order_release);
    }

    void Terminate() override {
      ScopedLock lock(GetThreadLock());
      thread->terminate();
    }

    void Wait() override {
      if (!waited) {
        thread->wait();
        waited = true;
      }
    }
  };
}

Thread Thread_Create(Job const& job) {
  return new ThreadImpl(job);
}

DefineFunction(Thread_SleepMS) {
  sf::sleep(sf::milliseconds(args.ms));
}

DefineFunction(Thread_SleepUS) {
  sf::sleep(sf::microseconds(args.us));
}
