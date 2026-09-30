# Enemy Combat AI, Death, Hit Reactions, and Source HUD

This pass turns the vertical slice from a mission-state prototype into a more game-like combat loop.

## Enemy runtime components

When `AIGIEnemyAIController` possesses a pawn it now ensures the pawn has:

```text
IGIHealthComponent
IGIHitReactionComponent
IGIInventoryComponent
IGILootableInventoryComponent
IGIThermalSignatureComponent
```

Existing Blueprint-authored components are reused rather than duplicated.

This lets ordinary enemy Pawns participate in damage, hit reaction, corpse loot, thermal vision, and
death behavior without requiring a new C++ enemy-character class.

## Hit reaction

`UIGIHitReactionComponent` listens to Unreal point damage and exposes:

```text
OnHitReaction
GetLastHitLocation()
GetLastShotDirection()
GetLastHitBone()
GetLastHitDamage()
```

A small movement impulse is applied as a source-only physical fallback. Production Animation
Blueprints should consume `OnHitReaction` / last-hit data for directional flinch montages, additive
upper-body reactions, and bone-specific reactions.

The enemy controller pauses movement briefly after a hit, cancels low-priority distraction behavior,
and re-evaluates combat/cover after the reaction window.

## Enemy death

When IGI health reaches zero:

1. tactical AI state switches to `Dead`,
2. current movement/search/distraction is cancelled,
3. an ally-down BDFR distress event is emitted,
4. Character movement is disabled,
5. capsule collision is disabled,
6. skeletal-mesh ragdoll is enabled when a Physics Asset exists,
7. the AI controller unpossesses the corpse,
8. inventory/loot components remain on the corpse.

Default corpse lifetime is zero, meaning the corpse remains until level cleanup or mission logic removes it.

### Distress event

Nearby BDFR guards can hear:

```text
BDFR.Distress.AllyDown
```

so a silent takedown/combat kill can still influence nearby AI if the death is close enough to be
perceived through the existing BDFR social hearing path.

## Tactical state

`AIGIEnemyAIController` now exposes:

```text
Idle
Investigate
Search
TakeCover
Combat
Dead
```

through:

```text
GetTacticalState()
GetSearchCenter()
GetCoverLocation()
IsDead()
OnTacticalStateChanged
```

These are intended as the bridge to the production Behavior Tree.

## Search behavior

BDFR awareness provides a real `LastKnownLocation`. The IGI controller uses it rather than reading
the player's current hidden location.

When line of sight is lost after meaningful awareness:

- the guard moves to the last known location,
- then samples reachable navigation points around it,
- harder difficulty tiers search more points,
- persistent-hunt difficulty behavior continues searching,
- non-persistent guards eventually forget the target and return to Idle.

Defaults:

```text
Search Radius      = 750 cm
Base Search Points = 4
Search Step        = 2.4 s
```

The number of search points increases with BDFR difficulty.

## Cover behavior

When a guard has line of sight to a threat, the controller can sample reachable NavMesh locations and
test whether world geometry blocks visibility from the threat.

The selected candidate prefers:

- actual occlusion,
- short movement distance,
- a useful standoff distance from the threat.

Default cover tendency:

```text
Recruit   22%
Private   38%
Sergeant  62%
Commando  78%
SAS       90%
```

Low-health enemies receive an additional cover preference.

This is intentionally a source-only cover foundation. The next Behavior Tree/EQS pass can replace the
random NavMesh sampling while keeping the same tactical-state API.

## Source HUD

`AIGIHUD` is now the default HUD for `AIGIGameModeBase`.

It draws directly through Unreal Canvas and therefore needs no UMG assets to test the slice.

It currently shows:

```text
Mission objective / extraction state
Player HP bar and HP values
Med Kit count
Distraction-object count
Active firearm
Magazine ammo
Reserve ammo
```

This HUD is a functional development HUD, not the final art/UI architecture. It should eventually be
replaced or wrapped by production UMG/CommonUI widgets.

## Vertical slice effect

The playable loop is now closer to:

```text
Recon
  ↓
Stealth / distraction
  ↓
Enemy detects/searches
  ↓
Combat / hit reactions / cover
  ↓
Enemy death + persistent corpse loot
  ↓
Med Kit / ammo recovery
  ↓
Primary objective
  ↓
Flare extraction
  ↓
Mission complete
```

## Next quality pass

Recommended next tasks:

1. enemy firearm attack task / burst-fire logic,
2. directional hit-reaction montages,
3. death animation-to-ragdoll blending,
4. corpse interaction for ammo/Med Kit/weapon looting,
5. authored cover points or EQS cover query,
6. production HUD/interaction prompts,
7. checkpoint/save state.
