# Weapon Shot FX, Weather-Aware Casings, and Exact NPC Ammo Loot

This system extends the firearm loop with visual shot effects, physical shell casings, environmental response, and deterministic ammunition looting.

## Shot FX

Each `UIGIWeaponDataAsset` can now define:

```text
MuzzleFlashEffect
MuzzleSmokeEffect
MuzzleSmokeScale
SuppressedMuzzleSmokeMultiplier
```

On every successful shot, `AIGIFirearmBase`:

1. consumes a round,
2. reports the gunshot to BDFR,
3. spawns muzzle flash,
4. spawns muzzle smoke,
5. ejects a shell casing,
6. performs the hit trace / damage.

Suppressor attachments still reduce muzzle flash through the existing attachment multiplier. Suppressed weapons can also reduce smoke independently.

### Required weapon sockets

Production firearm meshes should contain:

```text
Muzzle
SCK_Casing_Eject
```

Those names are configurable per weapon Data Asset.

If a socket is missing, the prototype still runs, but the effect/casing falls back to the weapon actor transform and will not be visually aligned.

## Niagara weather parameters

The smoke Niagara component receives these optional user parameters:

```text
User.WindVelocity
User.PrecipitationIntensity
User.SurfaceWetness
```

Niagara systems do not have to use them, but production smoke systems should expose them so wind/rain can push or dissipate smoke naturally.

## Shell casings

`AIGIShellCasingActor` is a physical actor with gravity and collision.

Weapon Data Assets define:

```text
ShellCasingClass
ShellCasingMesh
CasingEjectionSocket
CasingEjectionSpeed
CasingUpwardSpeed
CasingEjectionRandomConeDegrees
CasingAngularSpeedDegrees
CasingLifeSeconds
```

The casing ignores Pawn collision to avoid interfering with movement and is automatically destroyed after its configured lifetime.

Once its rigid body goes to sleep, casing Tick is disabled.

## Weather response

`UIGIWeatherWorldSubsystem` stores the current world weather:

```text
Clear
Rain
Snow
Storm
Sandstorm
```

with:

```text
PrecipitationIntensity
WindDirection
WindSpeedCmPerSecond
SurfaceWetness
SnowDepthFactor
```

Casings react directly to the weather state.

### Clear

- normal ejection energy,
- low damping,
- very small wind response.

### Rain

- slightly reduced ejection energy,
- increased linear/angular damping,
- wetness increases damping,
- moderate wind response.

### Snow

- noticeably reduced casing movement,
- strong damping,
- deeper snow further damps motion,
- low-to-moderate wind response.

### Storm

- near-normal initial ejection,
- strong continuous wind influence.

### Sandstorm

- reduced initial ejection energy,
- increased damping,
- strongest wind response.

The current weather can be set from Blueprint by getting the `IGIWeatherWorldSubsystem` and calling `SetWeatherState`.

## Glock prototype testing

The source-only `IGI Weapon Pickup` exposes:

```text
Prototype Muzzle Flash Effect
Prototype Muzzle Smoke Effect
Prototype Casing Mesh
```

so the Glock test does not require a production Data Asset yet.

Assign Niagara systems and a casing Static Mesh there, then fire with the existing prototype controls.

## Exact NPC / enemy ammunition

Ammo loot is not randomized.

An NPC/enemy should own:

```text
UIGIInventoryComponent
UIGILootableInventoryComponent
```

and use normal `AIGIFirearmBase` weapons.

While the NPC is alive:

- firing decrements the real magazine,
- reloading consumes the real reserve pool,
- partially used magazines remain partially used.

When looted:

`UIGILootableInventoryComponent::LootAllRemainingAmmunition(...)`

transfers the exact rounds still present in:

1. the source reserve ammo pools,
2. the loaded magazines, when `bIncludeLoadedMagazines` is true.

No extra loot roll or random ammo quantity is generated.

If the recipient cannot carry everything, only the accepted rounds are removed from the NPC. The unaccepted remainder stays on the source inventory instead of disappearing.

### Example

Enemy starts with:

```text
AK magazine: 30
Reserve:     90
Total:      120
```

Enemy fires 13 rounds and reloads once after using the rest of the magazine.

Whatever the actual runtime state becomes is exactly what the player can loot. The loot system does not restore the enemy's original loadout.

## Authoring partial NPC magazines

For mission setup or save/load:

```text
AIGIFirearmBase::SetCurrentMagazineAmmo(...)
```

can initialize a weapon with an exact partial magazine.

Reserve ammo is authored through `UIGIInventoryComponent::AddAmmo(...)`.

This means enemy ammunition is persistent gameplay state rather than a death-time random reward.
