# SFML 3.1.0 Refactor Audit & Recommendations

This document outlines the current state of the SFML 3.1.0 upgrade and identifies areas for further refactoring to best utilize the new API.

## 1. Current State Assessment
The project has been successfully migrated from SFML 2.6.2 to 3.1.0. Core functionality is working, and the build is clean. The major breaking changes (Event API, scoped enums, Vector3f for Audio) have been addressed.

## 2. Areas for Further Refactoring

### 2.1 Window & Input (src/liblt/LTE/Window.cpp, src/liblt/LTE/Mouse.cpp)
- **Mouse Position Access:** 
    - In `src/liblt/LTE/Mouse.cpp`, `Mouse_GetPosImmediate` and `Mouse_SetPos` still cast `Window_Get()->GetImplData()` to `sf::RenderWindow*`. 
    - **Recommendation:** Add a wrapper method to the `Window` interface (or `WindowT`) to handle mouse positioning/getting, avoiding raw casts to SFML types in the LTE layer.
- **Event Loop Optimization:**
    - The event loop in `Window.cpp` uses `event->getIf<T>()` multiple times. 
    - **Recommendation:** While correct, ensure that the most frequent events (like `MouseMoved`) are checked first to minimize `std::variant` visits.

### 2.2 Audio (src/liblt/Module/SoundEngine/SFML.cpp)
- **Audio Positioning:**
    - 3D sounds are currently managed by manually subtracting `camPos` from object positions and setting the SFML listener to `{0, 0, 0}`.
    - **Recommendation:** Evaluate if SFML 3.1.0's `sf::Listener` and `sf::Sound::setPosition` can handle the global coordinates more efficiently, or if the current relative-positioning approach is still superior for precision.
- **Sound Buffer Management:**
    - `SoundSFMLImpl` now stores a raw pointer to `sf::SoundBuffer`.
    - **Recommendation:** Ensure the lifetime of `sf::SoundBuffer` in the `buffers` map is strictly managed to prevent dangling pointers if sounds are played across different scenes/contexts.

### 2.3 Graphics (src/liblt/LTE/Texture2D.cpp)
- **Image I/O:**
    - `Texture2D::SaveTo` uses `sf::Image` to save files. 
    - **Recommendation:** Verify if `std::filesystem::path` is used consistently for all SFML file operations to avoid potential issues with `sf::String` conversions.

## 3. Summary of Completed Migrations
- [x] `sf::Event` API converted to `std::variant` / `std::optional`.
- [x] `sf::Keyboard` and `sf::Mouse` enums migrated to scoped enums (e.g., `sf::Keyboard::Key::A`).
- [x] `sf::VideoMode` and `sf::FloatRect` updated to use aggregate initialization `{x, y}`.
- [x] Audio `setPosition`/`setDirection` updated to take `sf::Vector3f`.
- [x] `sf::Sound::setLoop` $\rightarrow$ `setLooping`.
- [x] `sf::Sound::getLoop` $\rightarrow$ `isLooping`.
- [x] `sf::Sound::getBuffer` removal handled by storing buffer pointer.
- [x] `sf::SoundSource::Stopped` $\rightarrow$ `sf::SoundSource::Status::Stopped`.
- [x] `sf::Http::Response::Status` scoped enum implemented.
- [x] `sf::Image` construction updated to use constructor instead of `loadFromMemory`/`create`.
