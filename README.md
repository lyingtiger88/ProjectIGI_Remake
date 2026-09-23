# ProjectIGI Remake

<p align="center">
  <strong>Unreal Engine 5.8 C++ tactical third-person shooter / CQC prototype</strong>
</p>

<p align="center">
  <img alt="Unreal Engine" src="https://img.shields.io/badge/Unreal%20Engine-5.8-0E1128?logo=unrealengine&logoColor=white">
  <img alt="C++" src="https://img.shields.io/badge/C%2B%2B-Gameplay-00599C?logo=cplusplus&logoColor=white">
  <img alt="Platform" src="https://img.shields.io/badge/Platform-Windows%20x64-0078D6?logo=windows">
  <img alt="ALS" src="https://img.shields.io/badge/Locomotion-ALS%20Refactored-2ea44f">
  <img alt="AI" src="https://img.shields.io/badge/AI-BDFR%20Interactive%20AI-7c3aed">
  <img alt="Status" src="https://img.shields.io/badge/Status-Work%20in%20Progress-orange">
</p>

> **Status:** early gameplay foundation. The current focus is a stable ALS-based player framework that can later support shooting, melee/CQC, stealth, AI, and tactical interaction.

## Overview

**ProjectIGI Remake** is an Unreal Engine 5.8 C++ project focused on combining:

- third-person tactical shooting,
- close-quarter combat / melee,
- responsive locomotion based on **Advanced Locomotion System Refactored**,
- Enhanced Input,
- a project-owned player character layer that stays separate from the ALS plugin,
- a clean architecture that can grow into weapon, damage, stealth, cover, and AI systems.

The project currently uses this inheritance path:

```text
AAlsCharacter
    └── AIGIPlayerCharacter
            └── BP_IGI_Player
```

This keeps project-specific gameplay code out of the ALS plugin and makes future upgrades or replacement of the locomotion layer much safer.

## Current foundation

- [x] Unreal Engine 5.8 C++ project
- [x] ALS Refactored integration
- [x] Project-owned `AIGIPlayerCharacter`
- [x] ALS mesh / animation instance setup
- [x] ALS character and movement settings
- [x] Enhanced Input integration
- [x] Third-person Spring Arm camera
- [x] Walk / Run / Sprint / Crouch / Jump / Aim hooks
- [x] Clean-rebuild utility for Windows
- [x] BDFR Interactive AI integration
- [x] Player footprint + scent emission
- [x] Physical-surface-aware tracking strength
- [x] Human footprint tracking controller
- [x] Canine scent tracking controller
- [ ] Weapon framework
- [ ] ADS / hip-fire / recoil
- [ ] Damage and hit reactions
- [ ] Melee / CQC combo framework
- [ ] Takedowns / executions
- [ ] Stealth and awareness
- [ ] Tactical enemy AI
- [ ] Cover / interaction systems

## Requirements

- **Unreal Engine 5.8**
- **Visual Studio 2022**
  - Desktop development with C++
  - Game development with C++
  - a compatible Windows SDK
- **Git**
- Git submodule support

## Clone

Because **ALS Refactored** and **BDFR Interactive AI** are linked as submodules, clone recursively:

```bash
git clone --recurse-submodules https://github.com/lyingtiger88/ProjectIGI_Remake.git
cd ProjectIGI_Remake
```

If you already cloned the repository without submodules:

```bash
git submodule update --init --recursive
```

## Build

1. Make sure Unreal Editor and Visual Studio are closed.
2. Right-click `ProjectIGI_Remake.uproject`.
3. Select **Generate Visual Studio project files**.
4. Open the generated solution.
5. Build:
   - **Configuration:** Development Editor
   - **Platform:** Win64
   - **Target:** ProjectIGI_RemakeEditor
6. Open `ProjectIGI_Remake.uproject` in Unreal Engine 5.8.

For a more detailed build and recovery guide, see [docs/BUILD.md](docs/BUILD.md).

## Clean rebuild

A Windows cleanup script is included:

```text
Tools/Clean_ProjectIGI_Remake.bat
```

It removes generated build/cache data such as:

```text
Binaries/
Intermediate/
DerivedDataCache/
.vs/
plugin Binaries/
plugin Intermediate/
generated Visual Studio files
```

It does **not** delete project source, configuration, or plugin source/content. Deleting `Saved/` is optional and requires confirmation.

## Repository layout

```text
ProjectIGI_Remake/
├── Config/                         Unreal project configuration
├── Plugins/
│   ├── ALS/                       ALS Refactored submodule
│   └── BDFR_InteractiveAI/        Tactical AI / tracking submodule
├── Source/
│   ├── ProjectIGI_Remake/
│   │   ├── IGIPlayerCharacter.*   Project player character
│   │   └── ...
│   ├── ProjectIGI_Remake.Target.cs
│   └── ProjectIGI_RemakeEditor.Target.cs
├── Tools/
│   └── Clean_ProjectIGI_Remake.bat
├── docs/
│   ├── ARCHITECTURE.md
│   ├── BUILD.md
│   ├── TRACKING_SCENT.md
│   └── ROADMAP.md
└── ProjectIGI_Remake.uproject
```

## Architecture

The project intentionally separates locomotion from combat systems.

```text
                 Player Character
                       │
        ┌──────────────┴──────────────┐
        │                             │
   Locomotion                      Combat
      ALS                    ┌────────┴────────┐
                             │                 │
                          Ranged             CQC
                             │                 │
                         Weapons        Melee / Takedown
```

ALS should remain responsible for **how the character moves**. Combat code decides **what the character is doing**, while BDFR owns reusable perception/awareness/tracking logic.

The player now emits logical footprint and scent trails consumed by BDFR human and canine AI. See [docs/TRACKING_SCENT.md](docs/TRACKING_SCENT.md) for setup, tuning, Physical Material mapping, and Behavior Tree integration.

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for the intended gameplay-module boundaries.

## Input

The player code currently binds ALS-compatible Enhanced Input actions for:

- look,
- move,
- sprint,
- walk toggle,
- crouch,
- jump,
- aim.

The actual keyboard/controller keys come from the project's Enhanced Input mapping context and may change during development.

## Source-only note

This repository is currently focused on **source code and configuration**. Unreal binary assets such as `.uasset` and `.umap` may be delivered separately rather than stored directly in Git.

If binary project content is added later, Git LFS is strongly recommended.

## Roadmap

The high-level roadmap is tracked in [docs/ROADMAP.md](docs/ROADMAP.md).

The next major milestone is:

```text
Stable Locomotion
      ↓
Weapon Core
      ↓
Third-Person Shooting
      ↓
Melee / CQC
      ↓
Damage + Reactions
      ↓
Stealth / AI
```

## BDFR Interactive AI

ProjectIGI uses **BDFR Interactive AI** as a separate plugin/submodule for reusable tactical AI systems, including awareness, difficulty, footprint tracking, canine scent tracking, acoustics, and indirect perception.

Upstream project:
https://github.com/lyingtiger88/BDFR_InteractiveAI

Project-specific adapters stay in `ProjectIGI_Remake`; reusable tracking logic stays in the plugin.

## ALS Refactored

This project uses **Advanced Locomotion System Refactored** as an external dependency/submodule.

Upstream project:
https://github.com/Sixze/ALS-Refactored

ALS Refactored is distributed under its own license. Keep third-party copyright and license notices intact when redistributing its source.

## Project status & disclaimer

This repository is a development/learning project and is **not affiliated with, sponsored by, or endorsed by the original Project I.G.I. developers, publishers, or rights holders**.

Names and trademarks belong to their respective owners. Do not redistribute third-party game assets unless you have the rights to do so.

## Contributing

Bug reports and technical suggestions are welcome. Please use the issue templates so crash logs, engine version, reproduction steps, and build information are included.

For code changes, keep project gameplay logic outside the ALS plugin whenever possible.
