# ProjectIGI Remake - Language & Technology Stack

## Overview

ProjectIGI Remake uses a hybrid Unreal Engine architecture:

- **C++** for core gameplay systems, AI integration, components, and engine-level logic.
- **Unreal Engine Blueprint Visual Scripting** for content configuration, prototyping, animation wiring, and designer-facing setup.
- **INI configuration** for Unreal project settings and runtime configuration.
- **HLSL / Material Graph** for future rendering, shaders, and visual effects.
- **Python (optional tooling)** for external pipeline utilities, asset processing, and automation scripts.

## Primary Languages

| Language | Usage |
|---|---|
| C++ | Player character, gameplay components, AI adapters, tracking systems, weapons, combat logic |
| Blueprint | Character setup, animation graphs, gameplay tuning, UI wiring, level scripting |
| Unreal Header Tool (UHT) macros | Reflection, UObject systems, Blueprint exposure, serialization |
| INI | Engine, input, physics, plugin and project configuration |
| HLSL / Material Graph | Shader work, materials, VFX rendering |

## C++ Module Structure

```text
Source/ProjectIGI_Remake/
│
├── IGIPlayerCharacter.*
│       Player framework based on ALS Character
│
├── AI/
│   ├── IGIEnemyAIController.*
│   └── IGIDogAIController.*
│
└── Tracking/
    ├── IGITrackingSurfaceComponent.*
    └── BDFR tracking integration
```

## Plugin Languages

### ALS Refactored

Language stack:

- C++ engine/gameplay code
- Blueprint animation/content integration

Role:

- character locomotion
- gait system
- stance system
- rotation modes
- animation framework

### BDFR Interactive AI

Language stack:

- C++ AI framework
- Blueprint extension points
- Unreal data assets/configuration

Role:

- perception
- awareness
- difficulty systems
- footprint tracking
- canine scent tracking
- AI extensions

## Development Rules

### C++ owns:

- reusable systems
- performance-critical logic
- gameplay frameworks
- AI integrations
- networking-sensitive systems

### Blueprint owns:

- asset references
- animation setup
- designer tuning
- level-specific behavior
- rapid prototyping

### Configuration owns:

- engine settings
- input mappings
- physical surfaces
- plugin settings

## Coding Standards

C++ follows Unreal Engine conventions:

- `A` prefix for Actor classes
- `U` prefix for UObject/Component classes
- `F` prefix for structs
- `E` prefix for enums
- Unreal reflection macros where required:
  - `UCLASS`
  - `USTRUCT`
  - `UENUM`
  - `UPROPERTY`
  - `UFUNCTION`

## Build Environment

Required languages/toolchains:

- C++20 compatible compiler through Visual Studio 2022
- Unreal Build Tool
- Unreal Header Tool
- Unreal Automation Tool

Target engine:

```text
Unreal Engine 5.8
```

## Future Planned Systems

| System | Primary Implementation |
|---|---|
| Shooting | C++ framework + Blueprint weapon data |
| Melee/CQC | C++ combat framework + Animation Blueprints |
| Stealth | C++ gameplay tags/components + AI Blueprint logic |
| Missions | Blueprint/Data Assets with C++ support |
| UI | UMG + Blueprint + C++ controllers |

The goal is to keep the project modular: engine-level systems remain in C++, while content creation remains flexible through Unreal tools.
