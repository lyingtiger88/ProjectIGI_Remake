# Flare Gun

ProjectIGI includes a specialized single-shot flare gun built on the normal firearm/inventory framework.

## Catalog and ammo

```text
WeaponId          FlareGun
AmmoType          Flare
MagazineCapacity  1
MaxReserveAmmo    6
FireMode          SemiAutomatic
```

The flare gun uses `AIGIFlareGunBase`, which overrides the normal firearm hitscan shot and launches
an `AIGIFlareProjectileActor`.

Because it still derives from `AIGIFirearmBase`, normal inventory, equip, reload, gunshot acoustics,
remaining-NPC-ammo loot, and combat-state handling continue to work.

## Flare purposes

```text
Illumination
AirSupportMarker
RescueExtraction
```

The purpose is stored in `UIGIWeaponDataAsset::FlarePurpose`.

A flare reports its purpose and position through `UIGIFlareSignalWorldSubsystem`.

Mission logic can bind to:

```text
OnFlareSignal
```

and read:

```text
HasActiveFlareSignal
GetLastFlarePurpose
GetLastFlareLocation
```

This is the hook for later air-support, rescue/extraction, or illumination objectives.

## Projectile

The flare projectile:

- uses ProjectileMovement with gravity,
- follows the camera/aim direction,
- is pushed by the current world wind,
- emits a point light,
- can use a Niagara trail,
- has a configurable lifetime,
- broadcasts its mission signal,
- reports a small `BDFR.Acoustic.Weapon.FlareBurn` acoustic event.

Weather can alter the flare's visible intensity. Storm and sandstorm conditions reduce effective
illumination, while wind changes its trajectory.

## Signal colors

The default point-light color depends on purpose:

```text
Illumination       warm white/orange
AirSupportMarker   red
RescueExtraction   orange-red
```

Production Niagara flare effects can use their own matching colors.

## Production data fields

`UIGIWeaponDataAsset` exposes:

```text
FlarePurpose
FlareProjectileClass
FlareProjectileMesh
FlareTrailEffect
FlareLaunchSpeed
FlareLifeSeconds
```

The normal weapon fields still control the flare gun mesh, muzzle socket, muzzle flash/smoke,
gunshot hearing radius, weight, carry slots, recoil, and ammo.

## Prototype test

Place an **IGI Weapon Pickup** and set:

```text
Prototype Preset = FlareGun
Auto Equip       = true
Initial Reserve Ammo = 6
```

Optional prototype fields:

```text
Prototype Weapon Mesh
Prototype Muzzle Flash Effect
Prototype Muzzle Smoke Effect
Prototype Flare Projectile Mesh
Prototype Flare Trail Effect
Prototype Flare Purpose
```

The pickup automatically spawns `AIGIFlareGunBase` for the FlareGun preset.

Controls remain:

```text
LMB  fire
R    reload
RMB  aim
```

The first round is loaded when the weapon spawns. Each shot consumes exactly one flare. Reload pulls
one real flare from the inventory reserve.

## NPC / enemy ownership

NPCs can carry the same flare gun.

If an NPC has three reserve flares, fires one loaded flare, reloads, then fires again, the loot system
does not regenerate a fixed flare count on death. The player receives only the flares actually still
present in that NPC's magazine/reserve state.
