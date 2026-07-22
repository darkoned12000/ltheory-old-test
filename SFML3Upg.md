# SFML 3.0.0 Upgrade Guide for Limit Theory Old

**Version:** 1.0  
**Last Updated:** 2026-07-22  
**Status:** [IN_PROGRESS]

## Executive Summary

Upgrading from **SFML 2.6.2** to **SFML 3.0.0** is a **major breaking change** requiring significant refactoring. The migration will affect approximately **40-50% of the codebase** and will require strategic testing at each phase.

### Key Challenges
- Complete LTSL SoundEngine abstraction rewrite
- Text rendering API modernization  
- Shader/variable binding updates
- Window context management overhaul
- Filesystem abstraction cleanup

### Upgrade Strategy
Implement a **phased migration** with comprehensive testing gates to ensure compatibility while maintaining progress.

---

## Phase 0: Foundation & Planning

### 0.1 Initial Setup & Assessment [PENDING]

#### Tasks:
- [ ] Create Git branch `feature/SFML3-upgrade`
- [ ] Document current SFML 2.6.2 usage patterns
- [ ] Create detailed migration checklist
- [ ] Set up testing framework for SFML 3.0
- [ ] Establish performance baseline

#### Testing Gates:
```
Test_Gate_0_1:
- Verify all existing tests pass (SFML 2.6.2 baseline)
- Document all SFML API usage in codebase
- Create migration impact matrix
- Estimate effort for each component

Success Criteria:
✓ All existing tests pass
✓ Documentation complete
✓ Effort estimates accurate
✓ Dependencies identified
```

### 0.2 Dependency Management [PENDING]

#### Tasks:
- [ ] Update CMake configuration for SFML 3.0
- [ ] Remove/poorly vendored SFML 2.6.2
- [ ] Configure system SFML 3.0 dependencies
- [ ] Test build with SFML 3.0

#### Testing Gates:
```
Test_Gate_0_2:
- Verify CMakeLists.txt updates work correctly
- Confirm SFML 3.0 headers are accessible
- Build both engine library and launcher
- Verify basic window creation works

Success Criteria:
✓ CMake configure succeeds with SFML 3.0
✓ All build targets compile
✓ Basic runtime functionality verified
✓ LD_LIBRARY_PATH set correctly
```

### 0.3 LTSL SoundEngine Layer Analysis [PENDING]

#### Tasks:
- [ ] Catalog all LTSL SoundEngine usage in scripts
- [ ] Identify SoundEngine API calls that need changes
- [ ] Plan SoundEngine abstraction layer rewrite
- [ ] Document all Sound-related LTSL bindings
- [ ] Create SoundEngine migration plan

#### Testing Gates:
```
Test_Gate_0_3:
- Verify SoundEngine usage documented across all .lts scripts
- Identify all SoundEngine API calls needing updates
- Create comprehensive SoundEngine migration plan
- Document all LTSL bindings that need changes

Success Criteria:
✓ All SoundEngine usage documented
✓ Migration plan detailed
✓ LTSL binding changes identified
✓ Replacement strategy defined
```

### 0.4 LTSL Text Rendering Analysis [PENDING]

#### Tasks:
- [ ] Document all LTSL Text API usage
- [ ] Identify Text rendering customizations
- [ ] Catalog Text styling and formatting
- [ ] Plan Text API migration (setColor -> setFillColor)
- [ ] Document font loading and glyph usage

#### Testing Gates:
```
Test_Gate_0_4:
- Document all LTSL Text API usage patterns
- Identify custom text rendering implementations
- Create Text API migration plan
- Document font and glyph usage

Success Criteria:
✓ All Text usage documented
✓ Migration plan complete
✓ Custom rendering identified
✓ Font usage cataloged
```

### 0.5 LTSL Shader Analysis [PENDING]

#### Tasks:
- [ ] Document all custom shader usage
- [ ] Identify shader parameter binding patterns
- [ ] Catalog shader uniform usage
- [ ] Plan shader API migration (setParameter -> setUniform)
- [ ] Document shader preprocessor usage

#### Testing Gates:
```
Test_Gate_0_5:
- Document all shader usage in LTSL scripts
- Identify custom parameter binding patterns
- Create shader migration plan
- Document shader preprocessor usage

Success Criteria:
✓ All shader usage documented
✓ Migration patterns identified
✓ Parameter binding updated
✓ Preprocessor usage preserved
```

### 0.6 LTSL Window/Event Analysis [PENDING]

#### Tasks:
- [ ] Document all window creation patterns
- [ ] Identify custom event handling
- [ ] Catalog input processing
- [ ] Plan window API migration
- [ ] Document context management

#### Testing Gates:
```
Test_Gate_0_6:
- Document all window creation patterns
- Identify custom event handling
- Create window migration plan
- Document context management

Success Criteria:
✓ All window usage documented
✓ Event handling patterns preserved
✓ Migration plan complete
✓ Context management updated
```

### 0.7 LTSL File System Analysis [PENDING]

#### Tasks:
- [ ] Document all custom file loading
- [ ] Identify resource management patterns
- [ ] Catalog configuration file usage
- [ ] Plan filesystem abstraction migration
- [ ] Document OS-specific file operations

#### Testing Gates:
```
Test_Gate_0_7:
- Document all custom file loading patterns
- Identify resource management implementations
- Create filesystem migration plan
- Document OS-specific operations

Success Criteria:
✓ All file loading documented
✓ Resource patterns preserved
✓ Migration plan complete
✓ OS operations updated
```

---

## Phase 1: LTSL SoundEngine Layer Rewrite

### 1.1 LTSL SoundEngine → Direct SFML Audio Binding [PENDING]

#### Tasks:
- [ ] Create LTSL Sound bindings using SFML 3.0 audio API
- [ ] Remove SoundEngine interface layer
- [ ] Update all LTSL scripts to use new API
- [ ] Test sound loading and playback
- [ ] Verify audio positioning and spatialization

#### Testing Gates:
```
Test_Gate_1_1:
- Verify Sound bindings compile correctly
- Test basic sound loading from files
- Test sound playback control (play/pause/stop)
- Test audio volume and pitch adjustments
- Test 3D audio positioning

Success Criteria:
✓ All Sound bindings compile
✓ Basic sound loading verified
✓ Playback control functions
✓ Volume/pitch adjustments tested
✓ 3D audio working
```

### 1.2 LTSL ScriptAPI Sound Bindings Update [PENDING]

#### Tasks:
- [ ] Update LTSL ScriptAPI sound bindings
- [ ] Add sound creation and management functions
- [ ] Update existing sound-related script functions
- [ ] Test script loading with sound APIs
- [ ] Verify backward compatibility

#### Testing Gates:
```
Test_Gate_1_2:
- Verify ScriptAPI bindings compile
- Test sound creation from scripts
- Test sound manipulation in LTSL
- Verify existing script compatibility
- Test sound event integration

Success Criteria:
✓ All ScriptAPI bindings compile
✓ Sound creation from LTSL works
✓ Script-side sound control verified
✓ Existing scripts still compatible
✓ Event integration functional
```

---

## Phase 2: LTSL Text Rendering Modernization

### 2.1 Text API Migration (setColor → setFillColor) [PENDING]

#### Tasks:
- [ ] Update Text class API to use SFML 3.0 Text API
- [ ] Migrate Text color handling to FillColor/OutlineColor
- [ ] Update font glyph loading
- [ ] Test text rendering with new API
- [ ] Verify underline/strikethrough support

#### Testing Gates:
```
Test_Gate_2_1:
- Verify Text API compile with SFML 3.0
- Test basic text rendering
- Test fill color setting
- Test outline color and thickness
- Test text positioning and alignment

Success Criteria:
✓ Text API compiles
✓ Basic rendering verified
✓ Color setters functional
✓ Outline support tested
✓ Positioning accurate
```

---

## Phase 3: LTSL Shader Modernization

### 3.1 Shader Parameter Migration (setParameter → setUniform) [PENDING]

#### Tasks:
- [ ] Update shader parameter binding to use SFML 3.0 uniforms
- [ ] Migrate custom parameter types
- [ ] Update shader preprocessor integration
- [ ] Test shader uniform setting
- [ ] Verify shader variable types

#### Testing Gates:
```
Test_Gate_3_1:
- Verify shader compilation with SFML 3.0
- Test basic shader uniforms
- Test float, vec2, vec3, vec4 parameters
- Test matrix uniforms
- Test texture samplers

Success Criteria:
✓ Shader compilation successful
✓ Basic uniform setting works
✓ All vector types supported
✓ Matrix uniforms functional
✓ Texture sampling verified
```

---

## Phase 4: LTSL Window/Event Modernization

### 4.1 Window Context Migration [PENDING]

#### Tasks:
- [ ] Update window creation to use SFML 3.0 ContextSettings
- [ ] Update event handling to SFML 3.0 Event API
- [ ] Migrate input system (keyboard, mouse, joystick)
- [ ] Update window state management
- [ ] Test window creation and events

#### Testing Gates:
```
Test_Gate_4_1:
- Verify window creation with SFML 3.0
- Test window states (focus, visibility, resizing)
- Test keyboard input (keys, text entry)
- Test mouse input (position, buttons, scrolling)
- Test event system

Success Criteria:
✓ Window creation works
✓ Window states functional
✓ Input handling accurate
✓ Event system reliable
```

---

## Phase 5: LTSL Filesystem Modernization

### 5.1 File System Abstraction Migration [PENDING]

#### Tasks:
- [ ] Update file loading to use SFML 3.0 resources
- [ ] Migrate configuration file reading
- [ ] Update resource path handling
- [ ] Test file loading with new API
- [ ] Verify cross-platform compatibility

#### Testing Gates:
```
Test_Gate_5_1:
- Verify file loading with SFML 3.0
- Test texture/font loading
- Test configuration file parsing
- Test resource path resolution
- Test error handling

Success Criteria:
✓ File loading functional
✓ Resource loading verified
✓ Config parsing works
✓ Path resolution correct
✓ Error handling robust
```

---

## Phase 6: Integration Testing

### 6.1 LTSL Application Compatibility [PENDING]

#### Tasks:
- [ ] Test all LTSL applications with new APIs
- [ ] Verify script compilation and execution
- [ ] Update application scripts for API changes
- [ ] Test application integration
- [ ] Performance benchmarking

#### Testing Gates:
```
Test_Gate_6_1:
- Test war application with new SFML APIs
- Test colony application with new APIs
- Test ltheory-main application
- Test all LTSL scripts compile and run
- Compare performance with baseline

Success Criteria:
✓ war application functional
✓ colony application functional
✓ ltheory-main application runs
✓ All LTSL scripts compile
✓ Performance acceptable
```

---

## Phase 7: Engine Core Testing

### 7.1 Unit Tests Update [PENDING]

#### Tasks:
- [ ] Update all LTE unit tests for SFML 3.0
- [ ] Add new unit tests for migrated APIs
- [ ] Verify test coverage
- [ ] Run comprehensive test suite
- [ ] Fix any test failures

#### Testing Gates:
```
Test_Gate_7_1:
- Run all existing LTE unit tests
- Verify test results match baseline
- Fix any compilation or runtime errors
- Add missing test coverage
- Benchmark test performance

Success Criteria:
✓ All existing tests pass
✓ New tests added
✓ Test coverage verified
✓ No regressions
✓ Performance acceptable
```