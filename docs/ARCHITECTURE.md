# Architecture

## Goals

The project keeps locomotion, combat, inventory, stealth/perception, and future CQC logic separated so each system can evolve without rewriting the player character or ALS.

## Character inheritance

```text
AAlsCharacter
    └── AIGIPlayerCharacter
            └── Blueprint player layer
```

`AAlsCharacter` provides locomotion behavior.

`AIGIPlayerCharacter` is the project-owned integration layer. It owns high-level gameplay components and project camera/input wiring.

The Blueprint layer should primarily configure content, tune Data Assets, and connect animation/presentation behavior.

## Runtime gameplay components

```text
AIGIPlayerCharacter
├── ALS locomotion
├── Camera / Enhanced Input
├── UIGICombatComponent
├── UIGIInventoryComponent
├── UIGIAcousticSignatureComponent
├── UBDFRTrackEmitterComponent
└── UIGITrackingSurfaceComponent
```

### Tactical stance layer

ProjectIGI owns a three-state stance layer above ALS:

```text
Standing
Crouching
Prone
```

ALS remains responsible for standing/crouching locomotion. Prone is represented by `EIGIPlayerStance::Prone`; while prone, ALS stays in its crouched state so its movement and aiming pipeline remains active.

The player character owns:

- tap-crouch standing/crouch transitions,
- hold-crouch prone entry,
- prone collision height,
- clearance checks before standing up,
- stance-aware camera offsets and FOV,
- right/left shoulder camera switching,
- slower camera-relative prone movement,
- aiming without forcing a stance change,
- Blueprint-facing prone animation state.

Dedicated prone animation assets remain project-owned presentation content rather than an ALS plugin modification.

See [STANCE_PRONE_AIM.md](STANCE_PRONE_AIM.md).

### Combat coordinator

`UIGICombatComponent` owns high-level combat state:

```text
Unarmed
Armed
Aiming
Firing
Reloading
Melee
Takedown
```

It tracks the active weapon and aim/reload state without moving firearm implementation into ALS.

### Weapon system

```text
AIGIWeaponBase
    └── AIGIFirearmBase

UIGIWeaponDataAsset
UIGIWeaponAttachmentComponent
UIGIWeaponAttachmentDataAsset
```

Responsibilities:

- data-driven weapon identity/family/handling,
- magazine and reserve-ammo interaction,
- fire modes,
- physical equip/holster sockets,
- weapon weight and carry-noise contribution,
- attachment compatibility,
- attachment visual meshes,
- recoil/spread/handling modifiers,
- suppressor state,
- weapon acoustic events.

Individual weapon models should normally be Data Assets rather than one C++ subclass per model. New C++ subclasses are justified only when behavior materially differs.

See [WEAPONS_INVENTORY_ATTACHMENTS.md](WEAPONS_INVENTORY_ATTACHMENTS.md).

### Inventory and physical carry model

`UIGIInventoryComponent` is a tactical loadout, not an unlimited backpack.

It provides four physical weapon slots, a dedicated knife slot, ammo pools, grenade limits, and a shared utility-explosive limit.

```text
SCK_Weapon_Back_01
SCK_Weapon_Back_02
SCK_Weapon_Hip_R
SCK_Weapon_Special
SCK_Knife
SCK_Grenade_Frag
SCK_Grenade_Flash
SCK_Grenade_Smoke
SCK_Utility_Explosive
```

Weapon Data Assets define compatible carry slots, so body/socket compatibility is part of weapon configuration.

### Acoustic stealth

`UIGIAcousticSignatureComponent` combines movement and loadout into a BDFR hearing signature.

```text
Movement speed
x stance
x physical surface
x carried weapon/equipment load
= AI-hearable movement signature
```

The component reports footsteps, gear rattle, landing, and vault events. Weapons report equip, reload, attachment handling, and gunshots.

Suppressors reduce gunshot hearing range/strength and muzzle flash but do not make the player silent.

See [ACOUSTIC_STEALTH.md](ACOUSTIC_STEALTH.md).

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

Prefer communication through components, gameplay tags, Data Assets, interfaces, and well-defined events instead of adding weapon/CQC knowledge directly to ALS classes.

## BDFR stealth/perception boundary

ProjectIGI owns game-specific emitters/adapters while BDFR owns reusable perception, awareness, tracking, and acoustics.

```text
ProjectIGI                              BDFR Interactive AI
-----------                             -------------------
AIGIPlayerCharacter                ->  UBDFRTrackEmitterComponent
UIGITrackingSurfaceComponent       ->  EBDFRTrackSurfaceType
UIGIAcousticSignatureComponent     ->  UBDFRAcousticEventLibrary
AIGIWeaponBase                     ->  UBDFRAcousticEventLibrary
AIGIEnemyAIController              ->  ABDFRAIController
AIGIDogAIController                ->  ABDFRCanineAIController
                                          |
                     +--------------------+--------------------+
                     v                                         v
           UBDFRTrackingWorldSubsystem                  UAISense_Hearing
```

The player emits lightweight footprint/scent samples and semantic hearing events.

Physical surfaces are shared context: mud, snow, grass, concrete, metal, water, and other mapped surfaces can influence both tracking and movement acoustics.

Normal movement/gear events affect AI hearing/awareness but do not apply physical acoustic exposure. Gunshots/explosions retain BDFR's hearing-exposure behavior.

See [TRACKING_SCENT.md](TRACKING_SCENT.md) for footprint/scent integration.
