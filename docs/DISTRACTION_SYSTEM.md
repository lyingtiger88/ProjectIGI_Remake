# Distraction System

ProjectIGI supports physical, throwable distraction objects that create semantic BDFR hearing events
when they strike the environment.

The system is designed so easy guards are readily lured, while difficulty tiers 3-5 become
progressively harder to manipulate and quickly learn to ignore repeated tricks.

## Player flow

Temporary source-only input:

```text
T = throw distraction object
```

The player must have at least one:

```text
EIGIEquipmentType::DistractionObject
```

in inventory.

Default carry limit:

```text
MaxDistractionObjects = 5
```

The object is physical, uses gravity/Chaos physics, spins in flight, and reports a distraction only
on its first meaningful impact.

## Pickup

Place:

```text
IGI Distraction Pickup
```

in the level.

Set:

```text
Distraction Object Count = 1
Pickup Mesh = optional
```

Partial pickup is supported when the player's inventory is nearly full.

The thrown object uses a small Engine sphere as a visible prototype when no project mesh is assigned.
A custom throw mesh can be assigned on the player through `DistractionThrowableMesh`.

## Acoustic event

Impact reports one of:

```text
BDFR.Acoustic.Distraction.Impact.Generic
BDFR.Acoustic.Distraction.Impact.Dirt
BDFR.Acoustic.Distraction.Impact.Mud
BDFR.Acoustic.Distraction.Impact.Snow
BDFR.Acoustic.Distraction.Impact.Sand
BDFR.Acoustic.Distraction.Impact.Grass
BDFR.Acoustic.Distraction.Impact.Concrete
BDFR.Acoustic.Distraction.Impact.Metal
BDFR.Acoustic.Distraction.Impact.Water
```

Metal and concrete are intentionally stronger lures. Snow, mud, sand, and grass damp the impact.

The acoustic instigator is the thrown object, **not the player**. This is important: the guard can
investigate the impact point without the hearing event directly confirming the player's position.

## Enemy response

`AIGIEnemyAIController` listens to BDFR semantic acoustic events and evaluates distraction sounds
separately from normal threat awareness.

When accepted, it exposes:

```text
HasActiveDistraction()
GetDistractionLocation()
GetDistractionStrength()
OnDistractionAccepted
```

For the prototype, `bAutoMoveToAcceptedDistraction` is enabled, so a guard directly moves toward the
accepted impact point. Later Behavior Trees can disable that flag and consume the same event/state.

If real awareness reaches Alerted or Confirmed Threat, the distraction is immediately cancelled.

## Difficulty resistance

BDFR tiers map to gameplay levels:

```text
1 = Recruit
2 = Private
3 = Sergeant
4 = Commando
5 = SAS
```

Default distraction tuning:

| Tier | Min strength | Score threshold | Response chance | Max distance | Investigate |
| --- | ---: | ---: | ---: | ---: | ---: |
| Recruit | 0.10 | 0.18 | 100% | 3500 cm | 10 s |
| Private | 0.18 | 0.28 | 90% | 3200 cm | 9 s |
| Sergeant | 0.35 | 0.52 | 60% | 2600 cm | 7 s |
| Commando | 0.50 | 0.68 | 35% | 2100 cm | 5.5 s |
| SAS | 0.65 | 0.82 | 18% | 1600 cm | 4 s |

Tiers 3-5 also ignore distraction attempts once they are already Alerted.

All tiers ignore distraction lures after a target is a Confirmed Threat.

## Anti-exploit behavior

A lure is not evaluated from loudness alone. The response score combines:

- effective BDFR hearing strength,
- distance to the impact,
- current awareness,
- repeated-use penalty.

Repeated impacts near the same area within the memory window increase a penalty.

Defaults:

```text
RepeatedDistractionRadius = 650 cm
RepeatedDistractionMemorySeconds = 22 s
```

The repeat penalty itself becomes much stronger by difficulty:

```text
Recruit  0.07 per repeat
Private  0.10
Sergeant 0.16
Commando 0.22
SAS      0.30
```

This prevents repeatedly throwing objects at the same corner from reliably pulling high-tier guards
away from their duties.

## Behavior Tree integration

For production AI, turn off:

```text
bAutoMoveToAcceptedDistraction
```

and use `OnDistractionAccepted` or the Blueprint getters to enter an Investigate branch.

The system intentionally does not overwrite BDFR's real target awareness with the distraction object.
A distraction is a temporary investigation goal, not a hostile target.
