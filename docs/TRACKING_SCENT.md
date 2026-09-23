# Footprints & Scent Tracking

ProjectIGI integrates its tracking gameplay with the reusable
[BDFR Interactive AI](https://github.com/lyingtiger88/BDFR_InteractiveAI) plugin.

The integration deliberately keeps **track generation** on the player and **track interpretation**
inside BDFR AI controllers. ALS remains responsible only for locomotion.

## Runtime architecture

```text
AIGIPlayerCharacter
├── ALS locomotion
├── UBDFRTrackEmitterComponent
│   ├── Footprint samples by distance
│   └── Scent samples by time
└── UIGITrackingSurfaceComponent
    └── Physical Material -> BDFR surface type

                         World
                           │
                           v
              UBDFRTrackingWorldSubsystem
                     stores samples
                    /              \
                   /                \
                  v                  v
 AIGIEnemyAIController       AIGIDogAIController
 UBDFRFootprintTracking      UBDFRCanineTracking
      visual trail               scent trail
```

## Player emission

`AIGIPlayerCharacter` owns a `UBDFRTrackEmitterComponent`.

The current BDFR defaults emit approximately:

- one footprint every **90 cm** of horizontal movement,
- one scent sample every **1 second**.

A sample stores:

- source actor,
- world position,
- movement direction,
- timestamp,
- strength,
- sequence number,
- surface category.

BDFR keeps the logical tracking samples independent from visible decals. This means AI tracking
continues to work even when no footprint material is assigned.

## Ground surface mapping

`UIGITrackingSurfaceComponent` traces down from the player, requests the hit Physical Material,
and converts Unreal physical surfaces to BDFR track surfaces.

ProjectIGI uses this convention:

| Unreal Physical Surface | Name | BDFR Surface |
|---|---|---|
| SurfaceType1 | Dirt | Dirt |
| SurfaceType2 | Mud | Mud |
| SurfaceType3 | Snow | Snow |
| SurfaceType4 | Sand | Sand |
| SurfaceType5 | Grass | Grass |
| SurfaceType6 | Concrete | Concrete |
| SurfaceType7 | Metal | Metal |
| SurfaceType8 | Water | Water |

Create Physical Material assets for the materials that matter to stealth gameplay and assign the
matching Surface Type. Materials without a configured physical surface fall back to
`BDFRTrackSurfaceType::Default`.

The BDFR emitter already applies surface-specific footprint and scent strength multipliers.
For example, mud/snow preserve strong footprints while metal and water preserve little or none.

## Human footprint tracking

Use `AIGIEnemyAIController` for normal hostile AI that should use BDFR perception.

The base `ABDFRAIController` already owns `UBDFRFootprintTrackingComponent`. Tracking is gated by
the BDFR difficulty profile:

| Difficulty | Tracks Footprints | Sensitivity | Max Initial Footprint Age |
|---|:---:|---:|---:|
| Recruit | No | 0.50x | 20 s |
| Private | No | 0.75x | 35 s |
| Sergeant | Yes | 1.00x | 75 s |
| Commando | Yes | 1.30x | 120 s |
| SAS | Yes | 1.60x | 180 s |

A human tracker:

1. searches the BDFR tracking world subsystem,
2. checks footprint strength,
3. checks field of view,
4. performs a visibility trace to the footprint,
5. raises awareness toward the source actor,
6. stores the source and next trail location,
7. continues through newer sequence samples.

Useful Blueprint/C++ state:

```text
GetFootprintTrackingComponent()
    ├── HasActiveFootprintTrail()
    ├── GetTrackedActor()
    ├── GetNextTrackLocation()
    └── GetLastDetectedSample()
```

Suggested behavior flow:

```text
Patrol
  ↓
Footprint detected
  ↓
Investigate GetNextTrackLocation()
  ↓
Newer footprint?
  ├── Yes -> continue trail
  └── No  -> local search / return to patrol
```

A footprint increases awareness but does not by itself confirm line-of-sight contact.

## Canine scent tracking

Use `AIGIDogAIController` for dog AI.

It inherits `ABDFRCanineAIController`, which adds:

- `UBDFRCanineTrackingComponent`,
- `UBDFRCanineAttentionComponent`,
- a default **1.75x hearing multiplier**.

The scent tracker does not require line of sight. It acquires a recent scent sample and follows
newer samples from the same source actor.

Current BDFR defaults:

```text
Scent acquisition radius : 3000 cm (30 m)
Maximum acquisition age  : 240 s
Trail step radius         : 3500 cm (35 m)
Scan interval             : 0.20 s
Minimum scent strength    : 0.08
```

Useful state:

```text
GetCanineTrackingComponent()
    ├── HasScentTarget()
    ├── GetScentTarget()
    ├── GetNextScentLocation()
    └── GetLastScentSample()
```

Suggested dog behavior:

```text
Hear / search
   ↓
Acquire scent
   ↓
Move to GetNextScentLocation()
   ↓
Newer scent?
   ├── Yes -> follow
   ├── Target visible -> chase / alert handler
   └── No -> search around last sample
```

## Controller filtering

`AIGIEnemyAIController` and `AIGIDogAIController` currently accept
`AIGIPlayerCharacter` as their perception target. This prevents generic world actors from being
promoted to hostile BDFR targets.

Only the player currently emits BDFR tracks in ProjectIGI. If enemies or civilians later receive
track emitters, extend source filtering in the tracking layer before enabling cross-faction trails.

## Optional visible footprint decals

BDFR exposes:

`UBDFRTrackEmitterComponent::OnTrackSampleEmitted`

ProjectIGI can bind a presentation component to this event and spawn decals or Niagara effects for:

- mud,
- snow,
- wet footprints,
- blood trails,
- dust disturbances.

Keep this visual layer separate from the logical AI trail. Visual decals may be culled or faded
without deleting the AI sample.

## Performance model

The system does not spawn one Actor per scent/footprint. Samples are lightweight structs stored by
`UBDFRTrackingWorldSubsystem`.

BDFR prunes old data when the collection becomes large. For large open maps, consider adding
spatial partitioning/grid lookup if profiling shows track queries becoming significant.

## Setup checklist

1. Clone submodules recursively.
2. Verify **ALS** and **BDFR Interactive AI** are enabled.
3. Regenerate Visual Studio project files.
4. Build `ProjectIGI_RemakeEditor`.
5. Assign `AIGIEnemyAIController` to human hostile Pawns/Characters.
6. Assign `AIGIDogAIController` to dog Pawns/Characters.
7. Create Physical Materials and assign Surface Types 1-8 using the table above.
8. Add Behavior Tree logic that consumes the next footprint/scent location.

## Source ownership

ProjectIGI owns only the game-specific integration:

- `AIGIPlayerCharacter`
- `UIGITrackingSurfaceComponent`
- `AIGIEnemyAIController`
- `AIGIDogAIController`

BDFR owns the reusable mechanics:

- track emitter,
- world track storage,
- footprint tracker,
- canine scent tracker,
- awareness/difficulty integration.

This boundary allows BDFR to evolve independently of ALS and ProjectIGI.
