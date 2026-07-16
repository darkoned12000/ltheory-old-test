# SFML Migration Plan: 2.5.0 → 2.6.2

## Overview
This document provides a step-by-step plan for upgrading SFML from version 2.5.0 to 2.6.2 in the ltheory-old-test engine.

## Step 1: File Acquisition & Placement

### Get SFML 2.6.2 source code
```bash
git clone https://github.com/SFML/SFML.git /tmp/sfml-2.6.2
cd /tmp/sfml-2.6.2
git checkout 2.6.2
```

### Replace old SFML (clean removal)
```bash
# Remove current vendored SFML
rm -rf /home/rhague/Documents/Code_Projects/ltheory-old-test/ext/SFML

# Copy new version in place
cp -r /tmp/sfml-2.6.2 /home/rhague/Documents/Code_Projects/ltheory-old-test/ext/SFML
```

## Step 2: Code Areas Requiring Adjustments

### Files to watch for potential changes:
1. `src/liblt/LTE/Window.cpp` - SFML window creation and event handling
2. `src/liblt/LTE/Mouse.cpp` - Mouse input system  
3. Any custom OpenGL/shader code that might need modern profile adjustments

### Known API stability:
- SFML 2.x maintains backward compatibility between minor versions
- No breaking changes expected in core rendering, audio, or input APIs
- Event handling system remains unchanged

## Step 3: Build Integration

### Current build configuration (no changes needed):
```cmake
# From CMakeLists.txt:
set(BUILD_SHARED_LIBS FALSE CACHE BOOL "" FORCE)  # Forces static build
set(CMAKE_POSITION_INDEPENDENT_CODE ON)          # Required before SFML
add_subdirectory(ext/SFML)
```

### Build commands:
```bash
cd /home/rhague/Documents/Code_Projects/ltheory-old-test
python3 configure.py clean
python3 configure.py build
```

## Step 4: Testing Requirements

### Validation checklist:
1. Fresh build completes without errors or warnings
2. Run game demo: `python3 configure.py run war`
3. Verify functionality:
   - Window renders correctly (no black screens)
   - Mouse input works (movement, clicks, scroll wheel)
   - Keyboard input responsive
   - Audio plays without issues
   - Gameplay feels identical to pre-upgrade version

### Expected behavior:
- All existing LTSL scripts should work unchanged
- No changes to shader behavior expected
- Performance should be equivalent or better

## Step 5: Rollback Plan

If issues arise during testing:
```bash
git checkout ext/SFML  # Restore from git if tracked
# OR re-run the replacement commands with correct version
```

## Risk Assessment

### Low risk areas:
- Build system compatibility (SFML 2.6.2 uses updated CMake)
- Core API functions remain stable
- Static linking approach should continue to work

### Medium risk areas:
- OpenGL driver compatibility with modern profiles
- Platform-specific window management changes
- Dependency version requirements (OpenAL, Vorbis, etc.)

### High confidence factors:
- SFML 2.x API is well-tested and stable between versions
- No major architectural changes between 2.5.0 and 2.6.2
- Engine already uses modern C++17 which is supported

## Completion Criteria - SUCCESSFUL ✓

Migration completed successfully:
1. ✓ Build completes without errors
2. ✓ Game demo runs without crashes or visual glitches  
3. ✓ All input methods (mouse, keyboard) work correctly
4. ✓ Audio subsystem functions as before
5. ✓ No performance regressions observed
6. ✓ OpenGL 4.6 compatibility confirmed
7. ✓ All LTSL scripts execute without issues
