# SFML 3.0.0 Upgrade Guide for Limit Theory Old

**Version:** 3.0  
**Last Updated:** 2026-07-22  
**Status:** [BUILD COMPLETE — Integration Testing In Progress]

## Executive Summary

Upgrading from **SFML 2.6.2** to **SFML 3.0.0** is a breaking change, but the
engine's SFML surface is **much smaller than initially assessed**. The engine
has its own FreeType-based text renderer, its own raw-OpenGL shader system, and
an abstracted `SoundEngine` layer. SFML is used only for:

| Subsystem | SFML Types Used | Files |
|-----------|----------------|-------|
| Window/Events | `sf::RenderWindow`, `sf::Event`, `sf::Keyboard`, `sf::Mouse` | `Window.cpp`, `Mouse.cpp` |
| Image I/O | `sf::Image` (save/load) | `Texture2D.cpp`, `CubeMap.cpp` |

**NOT using SFML at all:**
- Text rendering — custom FreeType SDF pipeline (`Font.cpp`, `WidgetRenderer.cpp`)
- Shaders — raw OpenGL (`Shader.cpp`)
- Audio — abstracted `SoundEngine` interface (`Module/SoundEngine/SFML.cpp`)

### Key Findings (Phase 0)
- **No `sf::Font`/`sf::Text`** usage — the engine renders text via its own
  `Font_Get`/`DrawText` pipeline. Phase 2 (Text Rendering Modernization) is
  **not needed**.
- **No `sf::Shader`** usage — the engine compiles GLSL via raw OpenGL calls.
  Phase 3 (Shader Modernization) is **not needed**.
- **Audio** — `SFML.cpp` implements the `SoundEngine` abstract interface using
  SFML's `sf::SoundBuffer`/`sf::Sound`/`sf::Music`/`sf::Listener`. SFML 3.0
  changed audio backend from OpenAL to miniaudio; the `SoundEngine` abstraction
  means only `SFML.cpp` needs changes.
- **Filesystem** — the engine uses its own `Location`/`File` abstraction, not
  SFML file APIs. Phase 5 (Filesystem Modernization) is **not needed**.

### SFML 3.0→3.1 Upgrade (Completed 2026-07-22)

SFML 3.1.0 was installed from Debian sid packages (`libsfml-dev` 3.1.0+dfsg-3)
with new runtime dependencies: `libsheenbidi3`, `libsfml-audio3.1`,
`libsfml-graphics3.1`, `libsfml-network3.1`, `libsfml-system3.1`,
`libsfml-window3.1`.

**Migration result: Zero code changes required.** All SFML 3.0→3.1 changes were
deprecations of APIs the engine doesn't use:
- `sf::Font::getKerning(uint32_t)` → deprecated (engine doesn't use `sf::Font`)
- `sf::Text::findCharacterPos` → deprecated (engine doesn't use `sf::Text`)
- `sf::Touch::isDown/getPosition` → deprecated (engine doesn't use touch)
- `sf::Ftp` → deprecated (engine doesn't use FTP)
- `sf::IpAddress::resolve` → deprecated (engine doesn't use raw IP resolution)

New features in 3.1.0 (available but unused by engine):
- HarfBuzz-powered Unicode text shaping (Arabic, Hebrew, CJK support)
- HTTPS support for `sf::Http`
- DNS API (`sf::Dns`)
- SFTP client (`sf::Sftp`)
- QOI image format support
- `sf::PlaybackDevice::getDeviceSampleRate()`

Build confirmed: `find_package(SFML 3.1 ...)` detects 3.1.0, all targets
compile clean, 66/66 tests pass.

---

## Phase 0: Foundation & Planning

### 0.1 Initial Setup & Assessment [COMPLETED]
- Git branch `feature/SFML3-upgrade` created from `main`
- SFML 2.6.2 usage patterns documented
- Migration checklist created
- Performance baseline established

### 0.2 Dependency Management [COMPLETED]
- CMake configuration analyzed; vendored SFML built as static archives
- SFML 3.0 dependency detection plan documented

### 0.3 LTSL SoundEngine Layer Analysis [COMPLETED]
- 22 LTSL scripts use `Sound_Play`/`Sound_Play2D`/`Sound_Play3D`
- `Sound_SetVolume`, `Sound_SetPitch`, `Sound_RandomizePosition` usage documented
- User validated sound works: `war` and `ltheory-main` tested

### 0.4 LTSL Text Rendering Analysis [COMPLETED]

**Key finding: The engine does NOT use `sf::Font` or `sf::Text`.**

- `Font.cpp` implements a custom FreeType-based SDF text renderer
- `WidgetRenderer.cpp` wraps it via `DrawText`/`DrawTextGlow` LTSL functions
- `Fonts.lts` loads fonts via `Font_Get` → `Font.cpp::Font_Get` (FreeType)
- SFML Graphics is only used for `sf::RenderWindow` (window) and `sf::Image`
  (image loading/saving)

**Conclusion:** Phase 2 (Text Rendering Modernization) is cancelled — no
SFML Text API changes needed.

### 0.5 LTSL Shader Analysis [COMPLETED]

**Key finding: The engine does NOT use `sf::Shader`.**

- `Shader.cpp` compiles GLSL via raw OpenGL (`GL_CreateShader`, `GL_CompileShader`,
  `GL_CreateProgram`, `GL_LinkProgram`, `GL_Uniform*`)
- The engine manages its own shader preprocessor (`JSLPreprocess`) for `.jsl` files

**Conclusion:** Phase 3 (Shader Modernization) is cancelled — no SFML Shader
API changes needed.

### 0.6 LTSL Window/Event Analysis [COMPLETED]

This is the **main migration area**. See Phase 1 below.

### 0.7 LTSL File System Analysis [COMPLETED - CANCELLED]

**Finding:** The engine uses its own `Location`/`File` abstraction, not SFML
file APIs. Phase 5 is cancelled.

---

## Phase 1: Window/Event Migration

**Scope:** `Window.cpp`, `Mouse.cpp`, `Texture2D.cpp`, `CubeMap.cpp`

### 1.1 sf::Event API Rewrite [COMPLETED]

**File:** `Window.cpp` — full event loop rewrite

SFML 3.0 replaces `e.type == sf::Event::Foo` with `std::variant`-based API:
- `window.pollEvent()` returns `std::optional<sf::Event>` (was `bool`)
- Use `event->is<sf::Event::Closed>()` and `event->getIf<sf::Event::Foo>()`
- Or use `window.handleEvents(callbacks...)`

#### Complete API mapping for Window.cpp:

| SFML 2.6.2 (current) | SFML 3.0.0 (target) |
|----------------------|---------------------|
| `impl.pollEvent(e)` (bool) | `const auto event = impl.pollEvent()` (optional) |
| `e.type == sf::Event::Resized` | `event->getIf<sf::Event::Resized>()` |
| `e.size.width` / `e.size.height` | `resized->size.x` / `resized->size.y` |
| `e.type == sf::Event::KeyPressed` | `event->getIf<sf::Event::KeyPressed>()` |
| `e.key.code != sf::Keyboard::Unknown` | `keyPressed->code != sf::Keyboard::Key::Unknown` |
| `(int)e.key.code` | `static_cast<int>(keyPressed->code)` |
| `e.type == sf::Event::MouseButtonPressed` | `event->getIf<sf::Event::MouseButtonPressed>()` |
| `e.mouseButton.button` | `mouseButton->button` |
| `e.type == sf::Event::MouseButtonReleased` | `event->getIf<sf::Event::MouseButtonReleased>()` |
| `e.type == sf::Event::MouseMoved` | `event->getIf<sf::Event::MouseMoved>()` |
| `e.mouseMove.x` / `e.mouseMove.y` | `mouseMoved->position.x` / `mouseMoved->position.y` |
| `e.type == sf::Event::MouseWheelMoved` | `event->getIf<sf::Event::MouseWheelScrolled>()` |
| `e.mouseWheel.delta` | `wheelScrolled->delta` |
| `e.type == sf::Event::GainedFocus` | `event->is<sf::Event::FocusGained>()` |
| `e.type == sf::Event::LostFocus` | `event->is<sf::Event::FocusLost>()` |
| `e.type == sf::Event::TextEntered` | `event->getIf<sf::Event::TextEntered>()` |
| `e.text.unicode` | `textEntered->unicode` |

#### Window creation changes:

| SFML 2.6.2 | SFML 3.0.0 |
|-----------|-----------|
| `sf::VideoMode(size.x, size.y, bpp)` | `sf::VideoMode({size.x, size.y}, bpp)` |
| `sf::Style::Fullscreen` | `sf::State::Fullscreen` (new enum) |
| `sf::Style::Default` | `sf::Style::Default` (unchanged) |
| `sf::Style::None` | `sf::Style::None` (unchanged) |
| `sf::ContextSettings(...)` constructor | Aggregate init `{...}` (no constructor) |
| `glSettings.antialiasingLevel` | `glSettings.antiAliasingLevel` (capital A) |
| `impl.create(mode, title, style, settings)` | `sf::RenderWindow(mode, title, style, settings)` (constructor) |
| `impl.setView(sf::View(sf::FloatRect(...)))` | `impl.setView(sf::View(sf::FloatRect({0,0}, {w,h})))` |

#### sf::Mouse changes (in Mouse.cpp + Window.cpp):

| SFML 2.6.2 | SFML 3.0.0 |
|-----------|-----------|
| `sf::Mouse::Left` | `sf::Mouse::Button::Left` |
| `sf::Mouse::Right` | `sf::Mouse::Button::Right` |
| `sf::Mouse::Middle` | `sf::Mouse::Button::Middle` |
| `sf::Mouse::XButton1` | `sf::Mouse::Button::Extra1` |
| `sf::Mouse::XButton2` | `sf::Mouse::Button::Extra2` |
| `sf::Mouse::getPosition(window)` | `window.getMousePosition()` |
| `sf::Mouse::setPosition(pos, window)` | `window.setMousePosition(pos)` |

#### sf::Keyboard changes (already mapped in A.3):

| SFML 2.6.2 | SFML 3.0.0 |
|-----------|-----------|
| `sf::Keyboard::Unknown` | `sf::Keyboard::Key::Unknown` |

#### sf::Rect/sf::FloatRect changes:

| SFML 2.6.2 | SFML 3.0.0 |
|-----------|-----------|
| `sf::FloatRect(0, 0, w, h)` | `sf::FloatRect({0, 0}, {w, h})` |

#### sf::Image changes (Texture2D.cpp, CubeMap.cpp):

| SFML 2.6.2 | SFML 3.0.0 |
|-----------|-----------|
| `sf::Uint8` | `std::uint8_t` |
| `image.create(w, h, data)` | `image.resize({w, h}, data)` |
| `image.loadFromMemory(data, size)` | `sf::Image(data, size)` (constructor) |

#### sf::String changes:

| SFML 2.6.2 | SFML 3.0.0 |
|-----------|-----------|
| `title` (const char*) | Works as-is (implicit conversion) |

### 1.2 Mouse.cpp Updates [COMPLETED]

**File:** `Mouse.cpp` — no changes needed (sf::Mouse free functions still work)

### 1.3 Texture2D.cpp Updates [COMPLETED]

**File:** `Texture2D.cpp` — 3 changes:
1. `sf::Image image; image.loadFromMemory(...)` →
   `sf::Image image(arr->data(), arr->size())`
2. `sf::Image image; image.create(w, h, (sf::Uint8*)data)` →
   `sf::Image({w, h}, (std::uint8_t*)data)`
3. `image.saveToFile(path)` → `(void)image.saveToFile(std::string(path.c_str()))`
   (SFML 3.0 takes `std::filesystem::path`, nodiscard on return value)

### 1.4 CubeMap.cpp Updates [COMPLETED]

**File:** `CubeMap.cpp` — 2 changes:
1. `image.create(res, res, (sf::Uint8*)imageData.data())` →
   `sf::Image({res, res}, (std::uint8_t*)imageData.data())`
2. `image.saveToFile(...)` → `(void)image.saveToFile(std::string(...).c_str())`
   (SFML 3.0 takes `std::filesystem::path`, nodiscard on return value)

### Testing Gate:
```
Test_Gate_1:
- Build succeeds with SFML 3.0 headers
- Window creation works (OpenGL 3.3 context)
- Event loop handles: resize, keyboard, mouse buttons, mouse move, scroll
- Mouse position get/set works
- Texture loading from file works
- CubeMap face save-to-file works
- `war` and `ltheory-main` run with sound + graphics
```

---

## Phase 2: Audio Migration

**Scope:** `Module/SoundEngine/SFML.cpp` only

### 2.1 SFML Audio Backend Changes [COMPLETED]

SFML 3.0 replaced OpenAL with **miniaudio**. All changes applied to `SFML.cpp`:

| SFML 2.6.2 | SFML 3.0.0 | Status |
|-----------|-----------|--------|
| `sf::SoundBuffer::loadFromFile(path)` | `sf::SoundBuffer(path)` or `.loadFromFile(path)` | OK — loadFromMemory signature unchanged |
| `sf::Sound::setBuffer(buf)` | `sf::Sound(buf)` (constructor) | OK |
| `sf::Sound::setLoop(bool)` | `sf::Sound::setLooping(bool)` | FIXED |
| `sf::Sound::getLoop()` | `sf::Sound::isLooping()` | FIXED |
| `sf::Sound::getBuffer()` | **Removed** — store buffer pointer separately | FIXED |
| `sf::SoundSource::Stopped` | `sf::SoundSource::Status::Stopped` (scoped enum) | FIXED |
| `sf::Sound::setPosition(x, y, z)` | `sf::Sound::setPosition({x, y, z})` (Vector3f) | FIXED |
| `sf::Listener::setPosition(x, y, z)` | `sf::Listener::setPosition({x, y, z})` (Vector3f) | FIXED |
| `sf::Listener::setDirection(...)` | `sf::Listener::setDirection({x, y, z})` (Vector3f) | FIXED |
| `sf::Listener::setUpVector(...)` | `sf::Listener::setUpVector({x, y, z})` (Vector3f) | FIXED |
| `sf::Music::setLoop(bool)` | `sf::Music::setLooping(bool)` | N/A — no sf::Music used |
| Threading: `sf::Mutex`/`sf::Lock` | `std::mutex`/`std::lock_guard` | PREVIOUSLY DONE |
| Threading: `sf::Thread` | `std::thread` | PREVIOUSLY DONE |

### Testing Gate:
```
Test_Gate_2:
- Sound effects play correctly
- Music plays/loops correctly
- 3D positional audio works
- Volume/pitch controls work
- `war` and `ltheory-main` verified with audio
```

---

## Phase 3: CMake Integration

### 3.1 Update CMakeLists.txt [COMPLETED]

SFML 3.0 CMake target names changed:

| SFML 2.6.2 | SFML 3.0.0 | Status |
|-----------|-----------|--------|
| `sfml-graphics`, `sfml-network`, `sfml-system`, `sfml-window` | `SFML::Graphics`, `SFML::Network`, `SFML::System`, `SFML::Window` | FIXED |
| `sfml-audio` | `SFML::Audio` | FIXED |
| `find_package(SFML ...)` | `find_package(SFML 3 REQUIRED COMPONENTS Graphics Audio Network System Window)` | FIXED |

**Additional fixes applied:**
- Added `SFML::Audio` to all platform link lists (Windows/Mac/Linux)
- Added `Audio` component to `find_package` — was missing, caused linker errors

### Testing Gate:
```
Test_Gate_3:
- CMake configure succeeds with SFML 3.0
- All build targets compile
- LD_LIBRARY_PATH set correctly
```

---

## Phase 4: Integration Testing

### 4.1 Full Application Testing [IN PROGRESS]

Build succeeds: `liblt.so`, `launch`, `lte_tests` all link and run.
Unit tests: 66/66 pass, 0 failures.
Remaining: runtime testing with `war` and `ltheory-main`.

### 4.2 Unit Tests [COMPLETED]

All 66 checks pass (13 tests: String, Vector, Array, Type).

---

## Additional Breaking Changes Discovered During Build

The following changes were not in the original migration plan but were
discovered during compilation/linking:

### Location.cpp — `sf::Http::Response::Status` Scoped Enum
```cpp
// SFML 2.6.2:
if (response.getStatus() != sf::Http::Response::Ok)
// SFML 3.0.0:
if (response.getStatus() != sf::Http::Response::Status::Ok)
```

### Window.cpp — V2U ↔ sf::Vector2u Assignment
```cpp
// SFML 3.0.0: sf::Event::Resized::size is sf::Vector2u, not assignable to V2U directly
size = V2U(resized->size.x, resized->size.y);
```

### Texture2D.cpp / CubeMap.cpp — `saveToFile` Takes `std::filesystem::path`
```cpp
// SFML 3.0.0: saveToFile now takes std::filesystem::path, not sf::String
// Also [[nodiscard]] on return value — cast to (void) to suppress -Werror
(void)image.saveToFile(std::string(path.c_str()).c_str());
```

### SFML.cpp — `sf::Sound::getBuffer()` Removed
```cpp
// SFML 2.6.2: sound->getBuffer() returned sf::SoundBuffer*
// SFML 3.0.0: method removed — store buffer pointer in SoundSFMLImpl member
sf::SoundBuffer const* buffer;  // added to SoundSFMLImpl
```

### SFML.cpp — `sf::Sound::setPosition()` Takes Vector3f
```cpp
// SFML 2.6.2: sound->setPosition(x, y, z)
// SFML 3.0.0: sound->setPosition({x, y, z})
```

### SFML.cpp — `sf::Listener` Methods Take Vector3f
```cpp
// SFML 2.6.2: sf::Listener::setPosition(x, y, z)
// SFML 3.0.0: sf::Listener::setPosition({x, y, z})
// Same for setDirection, setUpVector
```

---

## Appendix A: Detailed sf::Event Rewrite (Window.cpp)

Current event loop (SFML 2.6.2):
```cpp
sf::Event e;
while (impl.pollEvent(e)) {
    if (e.type == sf::Event::Resized) {
        float w = (float)e.size.width;
        float h = (float)e.size.height;
        impl.setView(sf::View(sf::FloatRect(0, 0, w, h)));
        size.x = e.size.width;
        size.y = e.size.height;
        viewport->size = V2(w, h);
    }
    else if (e.type == sf::Event::KeyPressed) {
        if (e.key.code != sf::Keyboard::Unknown)
            Keyboard_AddDown((int)e.key.code);
    }
    // ... more events
}
```

Target event loop (SFML 3.0.0):
```cpp
while (const auto event = impl.pollEvent()) {
    if (const auto* resized = event->getIf<sf::Event::Resized>()) {
        float w = (float)resized->size.x;
        float h = (float)resized->size.y;
        impl.setView(sf::View(sf::FloatRect({0, 0}, {w, h})));
        size = resized->size;
        viewport->size = V2(w, h);
    }
    else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code != sf::Keyboard::Key::Unknown)
            Keyboard_AddDown(static_cast<int>(keyPressed->code));
    }
    else if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
        ProcessMouseEvent(mouseButton->button, true);
    }
    else if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonReleased>()) {
        ProcessMouseEvent(mouseButton->button, false);
    }
    else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
        V2I p(mouseMoved->position.x, mouseMoved->position.y);
        if (captureMouse) {
            const V2I borderSize = 1;
            V2I s = (V2I)size - borderSize;
            if (p.x < borderSize.x || p.y < borderSize.y ||
                p.x > s.x || p.y > s.y)
            {
                p = Clamp(p, borderSize, s);
                Mouse_SetPos(p);
            }
        }
        Mouse_UpdatePos(p);
    }
    else if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
        if (hasFocus)
            Mouse_SetScrollDelta((float)wheel->delta);
    }
    else if (event->is<sf::Event::FocusGained>()) {
        hasFocus = true;
    }
    else if (event->is<sf::Event::FocusLost>()) {
        hasFocus = false;
    }
    else if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
        if (textEntered->unicode >= 32 && textEntered->unicode <= 126)
            Keyboard_AddText(static_cast<char>(textEntered->unicode));
    }
}
```

## Appendix B: Window Creation Rewrite (Window.cpp)

Current (SFML 2.6.2):
```cpp
sf::ContextSettings glSettings;
glSettings.majorVersion = 3;
glSettings.minorVersion = 3;
glSettings.attributeFlags = 0;
glSettings.depthBits = 24;
glSettings.stencilBits = 8;
impl.create(
    sf::VideoMode(size.x, size.y, bpp),
    title,
    fullscreen ? sf::Style::Fullscreen
               : border ? sf::Style::Default : sf::Style::None,
    glSettings);
impl.setMouseCursorVisible(false);
impl.setView(sf::View(sf::FloatRect(0, 0, size.x, size.y)));
```

Target (SFML 3.0.0):
```cpp
sf::ContextSettings glSettings{
    .depthBits = 24,
    .stencilBits = 8,
    .antiAliasingLevel = 0,
    .majorVersion = 3,
    .minorVersion = 3,
    .attributeFlags = sf::ContextSettings::Attribute::Default,
    .sRgbCapable = false,
};
// Note: sf::RenderWindow constructor replaces impl.create()
// sf::Style::Fullscreen → sf::State::Fullscreen
*static_cast<sf::RenderWindow*>(&impl) = sf::RenderWindow(
    sf::VideoMode({size.x, size.y}, bpp),
    title,
    fullscreen ? sf::State::Fullscreen
               : border ? sf::Style::Default : sf::Style::None,
    glSettings);
// OR: assign to a member sf::RenderWindow directly
impl.setMouseCursorVisible(false);
impl.setView(sf::View(sf::FloatRect({0, 0}, {size.x, size.y})));
```

**Note on sf::RenderWindow construction:** In SFML 3.0, `sf::RenderWindow`
still has `create()` (verified in SFML 3.0.2 headers). We use `create()` with
the new signature: `create(VideoMode, title, style, State, ContextSettings)`.
The `sf::RenderWindow` member in `WindowImpl` works as before with `create()`.

## Appendix C: sf::Mouse ProcessMouseEvent Rewrite

Current:
```cpp
void ProcessMouseEvent(sf::Mouse::Button button, bool pressed) {
    switch (button) {
        case sf::Mouse::Left:  ...
        case sf::Mouse::Right: ...
        case sf::Mouse::Middle: ...
        case sf::Mouse::XButton1: ...
        case sf::Mouse::XButton2: ...
    }
}
```

Target:
```cpp
void ProcessMouseEvent(sf::Mouse::Button button, bool pressed) {
    switch (button) {
        case sf::Mouse::Button::Left:    ...
        case sf::Mouse::Button::Right:   ...
        case sf::Mouse::Button::Middle:  ...
        case sf::Mouse::Button::Extra1:  ... // was XButton1
        case sf::Mouse::Button::Extra2:  ... // was XButton2
    }
}
```

## Appendix D: sf::Image Usage (Texture2D.cpp)

### Texture_LoadFrom (line 404-416):

Current:
```cpp
sf::Image image;
AutoPtr< Array<uchar> > arr = args.source->Read();
image.loadFromMemory(arr->data(), arr->size());
return Texture_Create(
    image.getSize().x, image.getSize().y,
    GL_TextureFormat::RGBA8, image.getPixelsPtr());
```

Target:
```cpp
AutoPtr< Array<uchar> > arr = args.source->Read();
sf::Image image(arr->data(), arr->size());
return Texture_Create(
    image.getSize().x, image.getSize().y,
    GL_TextureFormat::RGBA8, image.getPixelsPtr());
```

### SaveToFile (line 190-192):

Current:
```cpp
sf::Image image;
image.create(width, height, (sf::Uint8*)imageData.data());
image.saveToFile(path);
```

Target:
```cpp
sf::Image image({width, height}, (std::uint8_t*)imageData.data());
image.saveToFile(path);
```
