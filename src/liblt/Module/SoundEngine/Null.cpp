#include "Module/SoundEngine.h"

namespace {
  struct SoundNullImpl : public SoundT {
    void Delete() override {}

    bool IsFinished() const override {
      return true;
    }

    bool IsLooped() const override {
      return false;
    }

    float GetDuration() const override {
      return 1;
    }

    float GetPan() const override {
      return 0;
    }

    float GetPitch() const override {
      return 1;
    }

    float GetVolume() const override {
      return 1;
    }

    void SetCursor(float position) override {}
    void SetPan(float pan) override {}
    void SetPitch(float pitch) override {}
    void SetPlaying(bool playing) override {}
    void SetVolume(float volume) override {}
  };

  struct SoundEngineNullImpl : public SoundEngine {
    char const* GetName() const override {
      return "SoundEngine (NULL)";
    }

    Sound Play(Array<float> const& buffer) override {
      return new SoundNullImpl;
    }

    Sound Play2D(String const& filename, float volume, bool looped) override {
      return new SoundNullImpl;
    }

    Sound Play3D(
      String const& s,
      Object const& o,
      V3 const& v,
      float f,
      float distanceDiv,
      bool looped) override
    {
      return new SoundNullImpl;
    }

    void Update() override {}
  };
}

SoundEngine* SoundEngine_Null() {
  return new SoundEngineNullImpl;
}
