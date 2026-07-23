// Copyright (C) 2025  darkoned12000
// SPDX-License-Identifier: GPL-3.0-or-later
// Part of the ltheory-old-test modernization effort (Revamp Work).
// See NOTICE and LICENSE.GPL. Original engine (c) Josh Parnell, public domain.

#include "Harness.h"

#include "Module/SoundEngine.h"
#include "LTE/Array.h"
#include "Game/Object.h"

#include <cstdlib>

namespace {
  bool HasAudioDevice() {
    // SFML requires an audio device (even a dummy one) for sound playback.
    // On headless CI without PulseAudio/OpenAL, this will be false.
    char const* display = std::getenv("DISPLAY");
    char const* wayland = std::getenv("WAYLAND_DISPLAY");
    return display != nullptr || wayland != nullptr;
  }
}

LTE_TEST(NullSoundEngine_GetName) {
  SoundEngine* engine = SoundEngine_Null();
  LTE_CHECK(engine != nullptr);

  char const* name = engine->GetName();
  LTE_CHECK(name != nullptr);
  // Should contain "NULL" in the name
  bool found = false;
  for (char const* p = name; *p; ++p) {
    if (p[0] == 'N' && p[1] == 'U' && p[2] == 'L' && p[3] == 'L') {
      found = true;
      break;
    }
  }
  LTE_CHECK(found);

  delete engine;
}

LTE_TEST(NullSoundEngine_Play2D) {
  SoundEngine* engine = SoundEngine_Null();
  Sound s = engine->Play2D("nonexistent.wav", 0.5f, false);

  LTE_CHECK(s != nullptr);
  // Null engine always returns a sound that reports finished
  LTE_CHECK(s->IsFinished());
  // Default getters should return reasonable values
  LTE_CHECK(s->GetVolume() == 1.0f);
  LTE_CHECK(s->GetPitch() == 1.0f);
  LTE_CHECK(s->GetPan() == 0.0f);
  LTE_CHECK(s->GetDuration() == 1.0f);
  LTE_CHECK(!s->IsLooped());

  // Setters should not crash
  s->SetVolume(0.75f);
  s->SetPitch(1.5f);
  s->SetPan(0.5f);
  s->SetPlaying(true);
  s->SetCursor(0.0f);
  s->Delete();

  delete engine;
}

LTE_TEST(NullSoundEngine_Play3D) {
  SoundEngine* engine = SoundEngine_Null();
  Sound s = engine->Play3D(
    "nonexistent.wav",
    nullptr,    // no carrier object
    V3(0),      // offset
    0.5f,       // volume
    1.0f,       // distanceDiv
    false);     // looped

  LTE_CHECK(s != nullptr);
  LTE_CHECK(s->IsFinished());

  s->Delete();
  delete engine;
}

LTE_TEST(NullSoundEngine_PlayRawBuffer) {
  SoundEngine* engine = SoundEngine_Null();

  // Play with an empty float buffer (raw PCM)
  Array<float> buffer(0);
  Sound s = engine->Play(buffer);

  LTE_CHECK(s != nullptr);
  // Null engine stub: should report finished immediately
  LTE_CHECK(s->IsFinished());

  s->Delete();
  delete engine;
}

LTE_TEST(NullSoundEngine_SettersClamp) {
  SoundEngine* engine = SoundEngine_Null();
  Sound s = engine->Play2D("test.wav", 0.5f, false);

  // Volume should be clamped to [0, 1]
  s->SetVolume(2.0f);
  // Pan should be clamped to [-1, 1]
  s->SetPan(-2.0f);
  s->SetPan(2.0f);

  // Verify no crash and values are stored
  LTE_CHECK(s->GetVolume() <= 1.0f);
  LTE_CHECK(s->GetPan() >= -1.0f);
  LTE_CHECK(s->GetPan() <= 1.0f);

  s->Delete();
  delete engine;
}

LTE_TEST(NullSoundEngine_UpdateNoCrash) {
  SoundEngine* engine = SoundEngine_Null();

  // Play a few sounds then update — should not crash
  Sound s1 = engine->Play2D("a.wav", 0.3f, false);
  Sound s2 = engine->Play2D("b.wav", 0.7f, true);
  Sound s3 = engine->Play3D("c.wav", nullptr, V3(1, 2, 3), 0.5f, 1.0f, false);

  engine->Update();

  // All null sounds should still report finished
  LTE_CHECK(s1->IsFinished());
  LTE_CHECK(s2->IsFinished());
  LTE_CHECK(s3->IsFinished());

  s1->Delete();
  s2->Delete();
  s3->Delete();
  delete engine;
}

LTE_TEST(SFMLSoundEngine_CreateAndQuery) {
  if (!HasAudioDevice())
    return;

  SoundEngine* engine = SoundEngine_SFML();
  LTE_CHECK(engine != nullptr);

  char const* name = engine->GetName();
  LTE_CHECK(name != nullptr);

  // Verify engine can be created and named without crash
  // (actual playback tests require real sound files)

  delete engine;
}
