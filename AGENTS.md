# Project Development Instructions

This file is the primary project-level instruction source for both Codex and Claude Code.

Claude Code loads these instructions through `CLAUDE.md`. Do not duplicate the contents of this file in `CLAUDE.md`.

Project-specific facts such as the project name, architecture, executable names, directory layout, and test commands should be added to the section **Project-Specific Information**.

---

## 1. Authority and Scope

Follow instructions in this order:

1. The user's explicit instructions for the current task.
2. Project-specific instructions in this repository.
3. This file.
4. Existing code, documentation, tests, and established project conventions.
5. General engineering judgment.

When instructions conflict, follow the higher-priority instruction and mention the conflict when it affects the result.

Do not silently reinterpret a clear user request.

---

## 2. Core Development Philosophy

The project must remain understandable, self-contained, deterministic, and easy to build.

General principles:

- Target C99 unless the project explicitly requires another C standard.
- Prefer straightforward procedural code over abstraction-heavy designs.
- Prefer explicit data structures and small helper functions over complicated frameworks.
- Prefer readable code over clever code.
- Keep ownership, lifetime, memory use, and resource management explicit.
- Avoid unnecessary dynamic allocation.
- Avoid hidden global state when explicit state can be passed clearly.
- Avoid C++-style architecture patterns in C code.
- Preserve existing behavior unless the user explicitly requests a change.
- Make the smallest change that fully solves the task.
- Keep patches focused, reviewable, and easy to revert.
- Extend existing systems before creating parallel implementations.
- Do not perform unrelated cleanup or refactoring.
- Do not change public interfaces without a demonstrated need.
- Do not add speculative functionality.

Programs should be self-contained and fully functional without depending on external services, opaque runtime infrastructure, telemetry, automatic update mechanisms, or package-manager-controlled execution.

The complete source code needed to build the project must remain available in the repository.

---

## 3. Build System

Every project uses `nob.h` as its build system.

Rules:

- Use `nob.h` and the project's `nob.c` as the only build system.
- Do not introduce CMake, Meson, Ninja files, Premake, XMake, Bazel, Autotools, or another parallel build system.
- Do not create shell scripts that duplicate the normal build pipeline.
- When the build process must change, update `nob.c`.
- Integrate new source files, assets, code generation, tests, and build steps into the existing `nob.c` workflow.
- Preserve the existing bootstrap procedure.
- If the `nob` executable is missing, stale, or incompatible with the current platform, rebuild it from `nob.c`.
- Treat the build system as part of the project architecture.
- Replacing `nob.h` is not an acceptable refactoring unless the user explicitly requests it.

Before changing the build, inspect the existing `nob.c`, existing build targets, platform branches, generated-file rules, and dependency ordering.

Do not assume that a conventional command such as `make`, `cmake`, or a package-manager command is valid.

---

## 4. Dependencies and Third-Party Code

Minimize external dependencies.

Rules:

- Do not add a dependency unless it is strictly necessary.
- Before adding a dependency, determine whether the project or the C standard library already provides the required functionality.
- Prefer small, auditable, single-purpose libraries.
- Prefer libraries written in C and compatible with the project's C standard.
- Vendor third-party source code directly into the repository when practical.
- Prefer static linking when practical.
- Do not require a package manager for normal project builds.
- Do not fetch dependencies automatically during a normal build.
- Do not add telemetry, analytics, remote configuration, or automatic updates.
- Do not modify vendored third-party code unless necessary.
- Keep project-specific changes to third-party code clearly isolated and documented.
- Preserve third-party license files and attribution requirements.
- Do not upgrade an existing dependency as an incidental part of another task.

If a new dependency appears necessary, explain:

1. why it is needed;
2. which alternatives were considered;
3. how it will be built and distributed;
4. its license;
5. its effect on supported platforms and binary size.

Wait for explicit user approval before adding it unless the user already requested that specific dependency.

---

## 5. Cross-Platform Requirements

Preserve compatibility with:

- Linux;
- macOS;
- Windows with MinGW,

unless the project explicitly defines a different platform set.

Rules:

- Avoid platform-specific solutions unless required.
- Keep platform-specific code isolated behind small interfaces or compile-time branches.
- Do not break another supported platform to fix the current one.
- Use portable C99 where practical.
- Avoid assumptions about path separators, shell behavior, executable suffixes, file permissions, or compiler-specific extensions.
- When a task cannot be validated on every platform, state which platforms were actually tested.

---

## 6. Existing Pattern First

Before implementing new behavior:

1. Search the repository for an analogous feature.
2. Identify the existing conventions for naming, state management, error handling, resource lifetime, UI layout, serialization, build integration, and tests.
3. Reuse the established pattern when it is appropriate.
4. Explain any deliberate deviation.

Do not introduce a new subsystem when the existing subsystem can be extended cleanly.

Use the `existing-pattern` skill for tasks that add or extend functionality.

---

## 7. Minimal Patch Policy

For bug fixes and narrowly scoped features:

- Change only what is necessary.
- Avoid broad renaming.
- Avoid reformatting unaffected code.
- Avoid moving files without need.
- Avoid rewriting a function when a small local correction is sufficient.
- Preserve APIs, file formats, serialized data, command-line behavior, and user-visible behavior unless the task requires a change.
- Keep generated diffs easy to inspect.
- Separate logically independent changes.
- Do not combine a feature, refactor, dependency update, and formatting pass in one patch.

Use the `minimal-patch` skill whenever the requested change should remain narrowly scoped.

---

## 8. Bug Investigation

Do not guess at the cause of a bug and patch the visible symptom immediately.

Follow this order:

1. Restate the expected and observed behavior.
2. Reproduce the problem when possible.
3. Locate the relevant entry points.
4. Trace data and control flow.
5. Identify the first point at which actual behavior diverges from expected behavior.
6. Find a similar working implementation.
7. Form a concrete root-cause hypothesis.
8. Validate the hypothesis.
9. Apply the smallest sufficient fix.
10. Add or update a regression check when practical.
11. Rebuild and test.

Distinguish clearly between:

- confirmed facts;
- strong inferences;
- unverified possibilities.

Use the `bug-investigation` skill for debugging tasks.

---

## 9. Large Changes and Execution Plans

A task requires an execution plan when it is likely to:

- affect multiple subsystems;
- require substantial architectural decisions;
- span many files;
- introduce or change a persistent data format;
- alter public APIs;
- require staged migration;
- take multiple development sessions;
- carry meaningful risk of regressions.

For such tasks:

1. Read `PLANS.md`.
2. Create a task-specific plan under `docs/plans/`.
3. Keep the plan updated while working.
4. Record discoveries, decisions, deviations, validation results, and remaining risks.
5. Do not treat the initial plan as immutable.
6. Continue to prefer incremental, reviewable changes.

Use the `large-feature-development` skill.

For small fixes, do not create unnecessary planning documents.

---

## 10. Generated Files and Assets

Generated files must not be edited manually.

Rules:

- Identify the original source asset or generator.
- Modify the source, generator, or build rule.
- Regenerate the derived file through `nob.c`.
- Do not hand-edit generated headers, embedded resources, generated shaders, generated tables, or other build products.
- Keep generated outputs deterministic.
- Avoid committing generated files that the repository intentionally excludes.
- If the repository intentionally tracks generated files, commit the source and generated results together.
- Verify that regeneration produces no unexplained diff.

When uncertain whether a file is generated, inspect the build scripts and file headers before editing it.

---

## 11. File Preservation

Do not permanently delete source files, assets, examples, or documentation merely because they appear unused.

When removal is requested or clearly necessary:

- verify references first;
- preserve recoverability;
- prefer moving uncertain or temporarily unused files into an existing `temp/` or archival location when consistent with the project;
- avoid committing arbitrary backups such as `.bak`, `.old`, or numbered copies;
- use Git history as the primary recovery mechanism for committed files.

Never delete user data, local configuration, credentials, or untracked work.

---

## 12. Code Quality

### Functions

- Keep functions focused on one coherent responsibility.
- Prefer small helpers when they clarify repeated or intricate logic.
- Do not split code into trivial wrappers that obscure control flow.
- Keep side effects explicit.
- Validate inputs at appropriate boundaries.
- Handle failure paths deliberately.

### Naming

- Follow the existing repository naming conventions.
- Use names that describe purpose, not implementation accidents.
- Avoid unexplained abbreviations.
- Preserve established public names unless a rename is part of the task.

### Memory and resources

- Make ownership clear.
- Pair allocation with cleanup.
- Handle partial initialization safely.
- Check allocation and I/O failures where relevant.
- Avoid leaks on early returns and error paths.
- Avoid unnecessary allocation in hot paths.
- Do not introduce undefined behavior for performance.

### Comments

- Explain why, constraints, invariants, and non-obvious decisions.
- Do not narrate obvious syntax.
- Remove misleading comments when behavior changes.
- Do not leave commented-out code as a substitute for version control.

### Error handling

- Follow existing error-reporting conventions.
- Do not silently ignore errors.
- Include enough context to diagnose failures.
- Avoid terminating the entire program from low-level reusable code unless that is the established design.

---

## 13. Documentation

Documentation is part of the implementation.

Update documentation when a change affects:

- build or installation;
- user-visible behavior;
- public APIs;
- command-line options;
- configuration;
- persistent formats;
- architecture;
- extension procedures;
- supported platforms;
- known limitations.

Rules:

- Keep documentation close to the relevant code when practical.
- Preserve the terminology used by the project.
- Prefer concrete examples.
- Remove obsolete statements.
- Do not create documentation for behavior that does not exist.
- Do not rewrite unrelated documentation during a narrow task.

Use the `documentation-update` skill.

---

## 14. Testing and Validation

Before claiming completion:

1. Determine the project's supported build and test commands from `nob.c`, existing documentation, and repository conventions.
2. Build the affected target.
3. Run the relevant tests.
4. Exercise the changed behavior directly when possible.
5. Inspect warnings and errors.
6. Review the final diff.
7. Confirm that no unrelated files changed.

Rules:

- Do not claim that a command succeeded unless it was actually run successfully.
- Do not hide failing tests.
- Distinguish pre-existing failures from failures introduced by the change.
- Do not weaken, skip, or delete tests merely to obtain a passing result.
- Add a regression test for a bug when practical.
- Keep tests deterministic.
- State clearly what could not be tested and why.

Use the `build-verification` skill during implementation and `release-checklist` before final delivery.

---

## 15. Code Review Expectations

When reviewing code:

- inspect correctness before style;
- check behavior on failure paths and boundary cases;
- inspect ownership and resource lifetime;
- check for undefined behavior;
- check compatibility with C99 and supported compilers;
- verify consistency with existing architecture;
- identify unnecessary dependencies and abstractions;
- check build integration;
- check test coverage;
- distinguish blocking defects from optional improvements.

Report findings in descending order of severity and include precise file and code references.

Do not report speculative issues as confirmed bugs.

Use the `code-review` skill.

---

## 16. Git Safety

Detailed Git procedures are defined in the `git-workflow` skill.

Always follow these baseline rules:

- Inspect repository status before modifying files.
- Preserve all pre-existing user changes.
- Do not overwrite or discard uncommitted work.
- Do not use destructive Git commands without explicit user approval.
- Do not commit, pull, rebase, merge, push, force-push, create tags, or change remotes unless the user requested or authorized that operation.
- Never commit secrets, credentials, private keys, tokens, local environment files, or build artifacts.
- Never identify Claude, Claud Code, Codex, OpenAI, or any other AI tool as an author or co-author of this repository. Do not add AI attribution, `Co-Authored-By` trailers, generated-by notices, signatures, or equivalent credit to commits, pull requests, source files, documentation, release notes, or other project artifacts.
- Keep commits logically focused.
- Do not amend or rewrite shared history without explicit approval.
- Review the staged diff before every commit.
- Confirm the current branch and remote before any push.

---

## 17. Interaction and Reporting

Before editing:

- inspect the relevant code and instructions;
- identify ambiguity that materially affects correctness;
- when practical, state the intended approach for a substantial task.

During work:

- report important discoveries early;
- do not overwhelm the user with low-level operational details;
- mention scope changes or unexpected risks.

At completion, report:

- what changed;
- why it changed;
- which files were affected;
- which validation commands were run;
- their results;
- any limitations, untested areas, or remaining risks;
- Git actions performed, if any.

Do not say that work is complete when important required steps remain unfinished.

---

## 18. Completion Checklist

A task is complete only when all applicable items are satisfied:

- [ ] The requested behavior is implemented.
- [ ] The root cause was identified for bug fixes.
- [ ] The implementation follows an existing project pattern where appropriate.
- [ ] The patch is minimal and focused.
- [ ] No unrelated behavior was changed.
- [ ] No unnecessary dependency was added.
- [ ] `nob.c` remains the single build entry point.
- [ ] Generated files were handled through the build pipeline.
- [ ] The affected target builds.
- [ ] Relevant tests pass.
- [ ] The changed behavior was exercised when possible.
- [ ] Documentation was updated where necessary.
- [ ] The final diff was reviewed.
- [ ] No secrets, binaries, temporary files, or accidental artifacts were added.
- [ ] Unsupported or untested aspects are clearly reported.
- [ ] Any execution plan was updated to reflect the final state.

---

## 19. Project-Specific Information

### Project overview

- Project name: AntTweakBarC99
- Purpose: A from-scratch, in-place rewrite of the AntTweakBar (ATB) library,
  currently implemented in C++, into strict C99. This is a fork of
  `AntTweakBarGLFW3` (GLFW3 + OpenGL Core Profile, `nob.c`-based build). The
  public API (`include/AntTweakBar.h`) is already a flat, `extern "C"`
  function/struct API with only one C++-specific leak (`TW_TYPE_STDSTRING`
  and the `std::string&`-taking helper functions) — see
  `docs/plans/c99-rewrite.md` for the full audit and migration plan. The
  rewrite also formalizes GLFW3 as a hard, direct dependency of the core
  library (already true in practice today via `TwBar.cpp`'s clipboard
  code) and, on that basis, deletes rather than ports the custom OpenGL
  function-pointer loaders, the hand-rolled cross-platform timer, all
  native (non-callback) cursor code on every platform, and every event
  backend for a toolkit other than GLFW3 — see the plan for the full
  reasoning. This is a large, multi-session effort: read that plan before
  starting any part of the rewrite.
- Primary executable or library: `lib/libAntTweakBarC99.{a,so/dylib/dll}`
  (named to avoid colliding with the sibling `AntTweakBarGLFW3` fork's own
  build of the same, still-C++, library)
- Main source directories: `src/` (implementation), `include/` (public
  header), `examples/` (sample programs), `vendor/` (vendored GLFW3/GLAD/
  nob.h), `docs/plans/` (task plans)

### Build

- Bootstrap command: `gcc nob.c -o nob`
- Normal build command: `./nob`
- Debug build command: none separate; `./nob` builds both static and shared
  library variants
- Clean/rebuild command: `./nob -clean`
- Test command: none automated; `./nob -examples` builds the four kept
  GLFW3/Core-Profile examples for manual smoke-testing against a real window

### Architecture

- Main modules (all under `src/`, current C++ implementation):
  - `TwMgr.cpp`/`.h` — global manager (`CTwMgr`), window/event routing,
    variable-type registry (`CStruct`, `CEnum`, `CCustom`), cursor handling.
  - `TwBar.cpp`/`.h` — tweak-bar object (`CTwBar`) and the variable-node
    hierarchy (`CTwVar` → `CTwVarAtom`/`CTwVarGroup`), UI layout and drawing.
  - `TwFonts.cpp`/`.h` — bitmap font data (`CTexFont`) and generation; almost
    entirely embedded data tables, very little logic.
  - `TwColors.cpp`/`.h` — color conversions; already nearly pure C.
  - `TwGraph.h` — `ITwGraph`, the abstract renderer interface (pure virtual).
  - `TwOpenGL.cpp`/`.h`, `TwOpenGLCore.cpp`/`.h` — `ITwGraph` implementations
    for the OpenGL compatibility and Core profiles.
  - `LoadOGL.cpp`/`.h`, `LoadOGLCore.cpp`/`.h` — runtime OpenGL
    function-pointer loaders (namespace-scoped globals as a poor man's
    module). GLAD (already vendored, already linked into the library,
    generated for the full GL 2.1–4.6 compatibility profile) already
    resolves everything both renderers need — these two loaders are
    **deleted, not ported**, by the C99 rewrite.
  - `TwEventGLFW.c` (already C) — the only event backend kept. Every other
    backend (`TwEventGLUT.c`/`TwEventSDL.c`/`TwEventSDL12.c`/
    `TwEventSDL13.c`/`TwEventX11.c`/`TwEventSFML.cpp`) and its private
    `MiniXxx.h` stand-in headers are **deleted, not ported**, by the C99
    rewrite — none of them serve a purpose once the core library formally
    hard-depends on GLFW3 (see Purpose above).
  - `TwDirect3D9/10/11.cpp`/`.h`, `TwEventWin.c`, `d3d10vs2003.h` — dead
    code, not referenced by `nob.c`'s `common_sources`; out of scope for
    this fork already (see `docs/plans/remove-legacy-directx-win-backend.md`)
    and out of scope for the C99 rewrite too.
- Important state objects: `g_TwMgr`/`CTwMgr` (one global manager),
  `CTwBar` (one per tweak bar, owns a tree of `CTwVar`), `CTexFont` (shared
  default fonts), `ITwGraph` (one renderer instance per manager, currently
  selected via C++ virtual dispatch — the main structural thing the C99
  rewrite must replace, e.g. with an explicit function-pointer vtable
  struct).
- Persistent formats: none (no on-disk save format; bar/variable definitions
  are created and configured entirely through the runtime API).
- Generated sources/assets: none checked into `src/`; `TwFonts.cpp`'s bitmap
  tables are hand-generated once from source bitmaps (see file header
  comment) and are treated as source, not build output.

### Project conventions

- Naming: existing ATB naming is preserved during the rewrite — public
  `Tw`-prefixed functions/types, internal `C`-prefixed structs (`CTwBar`,
  `CTwVar`, ...), `m_`-prefixed struct fields, `_`-prefixed parameter names.
  Do not rename public or internal identifiers as part of the C99 port
  unless a name is impossible to keep (e.g. a removed C++-only symbol).
- Formatting: match the surrounding file's existing style; this codebase
  predates a formatter and is not reformatted wholesale.
- Error handling: existing ATB convention — functions return `int`/`bool`
  success codes or `NULL`/sentinel values, set a last-error string, and
  never throw or abort the process. No exceptions exist anywhere in the
  current C++ implementation to preserve or replace.
- Logging: none (library has no logging subsystem beyond the last-error
  string returned by `TwGetLastError`).
- Testing: none automated; validated by building and running the examples
  under `examples/` (see `./nob -examples`).
- Supported platforms (today, pre-rewrite): Linux, macOS (Cocoa cursor
  integration requires the library's C++ translation units to be compiled
  as Objective-C++ today — `-x objective-c++` in `nob.c`), Windows (MinGW).
  This macOS build requirement goes away entirely once the C99 rewrite
  lands — see "Known constraints" below.

### Relevant documentation

- Architecture: `docs/plans/c99-rewrite.md` (C++ feature audit and rewrite
  plan), `README.md`
- Build: `docs/plans/nob-build-system.md`, `README.md`
- Testing: none dedicated; see "Testing" above
- File formats: not applicable
- Other: `docs/glfw3-cursor-integration.md` (cursor-handling root-cause
  analysis, relevant to the Cocoa/X11 platform code touched by the rewrite)

### Known constraints

- `include/AntTweakBar.h` defines `TW_TYPE_STDSTRING` using `sizeof(std::string)`
  and declares `TwCopyStdStringToLibrary`/`TwCopyStdStringToClient` taking
  `std::string&` parameters. This is the only C++ leak in the public API and
  cannot be expressed in a C99 header — its removal or replacement is a
  public API breaking change and must be decided explicitly (see the plan's
  Open Questions), not assumed.
- On macOS, the current C++ implementation is compiled as Objective-C++
  (`-x objective-c++`) because of native (non-callback) cursor code in
  `TwMgr.cpp`. The C99 rewrite makes GLFW3 a hard, direct dependency of the
  core library (`#include <GLFW/glfw3.h>` outside just the event-backend
  file — already true today in `TwBar.cpp` for clipboard) and, on that
  basis, deletes the native cursor implementation on every platform
  outright rather than isolating it: `TwSetCursorCallback()` becomes the
  only cursor-shape mechanism everywhere. Result: no Objective-C, and no
  shim file, anywhere in the rewritten library. The new hard GLFW3
  dependency is itself a user-visible build requirement that must be
  documented in `README.md` — anyone embedding the library must link
  GLFW3 even if they don't use `TwEventGLFW.c`'s translation helpers.
- File-scope `const` globals in the current C++ headers (e.g.
  `TwColors.h`'s `COLOR32_BLACK` etc.) have internal linkage in C++ but
  external linkage in C, and these headers are `#include`d by many
  translation units — a mechanical but easy-to-miss source of duplicate-
  symbol link errors if copied verbatim into C99 headers.
