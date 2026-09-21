# Architecture

## Goals

The project is structured to keep locomotion, ranged combat, melee/CQC, and game-state logic independent enough to evolve without rewriting the player character.

## Character inheritance

```text
AAlsCharacter
    └── AIGIPlayerCharacter
            └── Blueprint player layer
```

`AAlsCharacter` provides locomotion behavior.

`AIGIPlayerCharacter` is the project-owned integration layer. It is the correct place for project camera/input wiring and high-level gameplay components.

The Blueprint layer should primarily configure assets, tune values, and connect content-specific behavior.

## Planned gameplay boundaries

```text
AIGIPlayerCharacter
├── Locomotion / ALS
├── Camera
├── Input
├── Combat Coordinator
│   ├── Weapon / Ranged
│   └── Melee / CQC
├── Health / Damage
├── Interaction
└── Stealth / Awareness hooks
```

### Locomotion

Responsibilities:

- gait,
- stance,
- rotation mode,
- movement state,
- locomotion animation,
- aim locomotion state.

ALS should not need to understand individual weapons or combo attacks.

### Weapon system

Planned responsibilities:

- weapon ownership/equip state,
- fire mode,
- ammo,
- reload,
- ADS,
- recoil,
- traces/projectiles,
- weapon-specific animation requests.

### Melee / CQC

Planned responsibilities:

- light/heavy attacks,
- combo windows,
- block/parry if used,
- hit detection,
- stagger,
- contextual takedowns,
- animation montage requests.

### Damage

Planned responsibilities:

- health,
- damage routing,
- hit reactions,
- death state,
- damage tags/types.

## Design rule

Prefer communication through components, gameplay tags, interfaces, or well-defined events rather than adding weapon/CQC knowledge directly to ALS classes.

That separation makes it easier to update ALS and keeps combat code reusable.
