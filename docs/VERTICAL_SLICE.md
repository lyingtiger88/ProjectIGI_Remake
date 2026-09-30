# Vertical Slice Mission Loop

This slice connects the existing tactical systems into a minimal playable mission flow:

```text
Mission Start
    ↓
Infiltrate / Recon
    ↓
Interact with Primary Objective
    ↓
Extraction becomes active
    ↓
Reach Extraction Zone
    ↓
Fire Rescue/Extraction Flare
    ↓
Mission Complete
```

If the player dies while the mission is active, the mission fails.

## New runtime systems

### Interaction

`IGIInteractable` is the generic interaction interface.

The player now performs a camera-centered visibility trace with:

```text
E = Interact
Interaction Distance = 350 cm
```

Any Actor implementing `IGIInteractable` can participate without adding its logic to the player class.

Public player call:

```text
TryInteract()
```

### Mission world subsystem

`UIGIMissionWorldSubsystem` owns the prototype mission state:

```text
Inactive
PrimaryObjective
Extraction
Completed
Failed
```

It exposes Blueprint events:

```text
OnMissionStateChanged
OnObjectiveCompleted
OnMissionCompleted
OnMissionFailed
```

The default objective identifier is:

```text
PrimaryIntel
```

and the default required extraction flare purpose is:

```text
RescueExtraction
```

### Mission Director

Place:

```text
IGI Vertical Slice Mission Director
```

in the level.

Defaults:

```text
Primary Objective Id      = PrimaryIntel
Extraction Flare Purpose  = RescueExtraction
Auto Start Mission        = true
```

This Actor configures and starts the world mission subsystem.

### Primary Objective

Place:

```text
IGI Mission Objective
```

where the player must retrieve intel or interact with the mission target.

Defaults:

```text
Objective Id        = PrimaryIntel
Interaction Prompt  = Retrieve Intel
Hide When Completed = true
```

A small Engine cube is used as source-only prototype presentation. Replace it with the actual laptop,
documents, terminal, briefcase, or other mission prop later.

Aim at the objective and press:

```text
E
```

When the objective is accepted, the mission enters Extraction.

### Extraction Zone

Place:

```text
IGI Extraction Zone
```

at the desired extraction location.

Default:

```text
Radius = 700 cm
```

The extraction zone registers itself with the mission subsystem.

## Flare extraction

Use the existing **IGI Weapon Pickup** with:

```text
Prototype Preset        = FlareGun
Prototype Flare Purpose = RescueExtraction
Auto Equip              = true
Initial Reserve Ammo    = 6
```

After the primary objective is completed, enter the extraction zone and fire the flare.

The mission only completes when:

1. the mission is in Extraction state,
2. a registered extraction zone exists,
3. the flare purpose matches `RescueExtraction`,
4. the flare signal is inside the extraction radius,
5. the flare belongs to the player.

A random illumination or air-support flare does not complete extraction.

## Suggested test-map layout

A useful first test layout is:

```text
Player Start
    |
    |  recon / binoculars / NVG
    v
Enemy Patrol ---- Distraction route
    |
Weapon / Med Kit pickup
    |
Primary Intel Objective
    |
Combat / stealth escape
    |
Flare Gun Pickup
    |
Extraction Zone
```

This lets one small map exercise the systems already present in the project:

- ALS locomotion,
- crouch / prone / prone roll,
- binoculars / NVG / thermal,
- BDFR sight / hearing,
- physical distraction objects,
- weapon pickup and combat,
- suppressors / shot acoustics,
- health and Med Kits,
- exact ammo loot,
- primary-objective interaction,
- flare extraction.

## Current feedback

Until the final HUD is authored, mission transitions produce:

- Output Log messages,
- temporary on-screen debug messages.

This keeps the vertical slice testable without introducing placeholder UMG architecture that would
need to be thrown away later.

## Next slice work

The next production passes should focus on quality rather than adding unrelated systems:

1. real HUD objective/status presentation,
2. enemy death and reaction flow,
3. search/cover Behavior Tree behavior,
4. final firearm ADS/recoil/impact feedback,
5. prone/supine and weapon animation layers,
6. checkpoint/save state for the mission,
7. authored test map and regression checklist.


## Enemy combat response

The slice now includes source-only enemy health/death, directional hit-reaction events,
last-known-location searching, difficulty-weighted cover use, and a Canvas development HUD.

See [ENEMY_COMBAT_AI_HUD.md](ENEMY_COMBAT_AI_HUD.md).
