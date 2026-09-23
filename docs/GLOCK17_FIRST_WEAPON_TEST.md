# Glock 17 First Weapon Loop

This prototype exists to validate the complete gameplay path before production weapon assets and animation content are authored.

## What is implemented

The current test loop covers:

```text
World pickup
  -> physical inventory slot
  -> auto equip
  -> aim through existing ALS input
  -> hitscan fire
  -> magazine consumption
  -> reserve-ammo reload
  -> BDFR gunshot hearing
  -> suppressor pickup
  -> quieter BDFR gunshot
```

The runtime prototype presets are development helpers. Production weapons should still use
`UIGIWeaponDataAsset` and `UIGIWeaponAttachmentDataAsset`.

## Prototype controls

```text
Left Mouse    Fire active firearm
Right Mouse   Aim (existing ALS mapping)
R             Reload
1             Equip Weapon01
2             Equip Weapon02
3             Equip Weapon03
4             Equip Weapon04
5             Equip Knife
```

The temporary combat bindings use direct key bindings so this source-only test does not require
new binary Enhanced Input assets.

## Place a Glock pickup

After rebuilding the editor:

1. Search the Place Actors / class picker for **IGI Weapon Pickup**.
2. Place it in the level.
3. Set:
   - `Prototype Preset = Glock17`
   - `Auto Equip = true`
   - `Initial Reserve Ammo = 34` or another test value.
4. If you have a Glock skeletal mesh, assign it to `Prototype Weapon Mesh`.
5. If you only have a static mesh, assign it to the pickup component `Pickup Mesh`.

When `Prototype Weapon Mesh` is empty, the prototype now automatically reuses the `Pickup Mesh`
static mesh as the equipped weapon visual. This makes the first pickup test visible without requiring
a skeletal weapon asset.

If no production Weapon Data Asset is assigned, the pickup creates temporary Glock 17 data with:

```text
Family              Pistol
Ammo                 9x19mm
Magazine             17
Max reserve          102
Fire mode            Semi Automatic
Base damage          28
Effective range      5000 cm
Base spread          0.35 degrees
Gunshot hearing      6500 cm
Preferred carry      Weapon03 / hip
Attachments          Muzzle, Optic, SideRail, Magazine
```

Walking into the pickup stores it in the first compatible free slot and equips it.

## Socket fallbacks

Production character meshes should eventually contain the documented IGI sockets.

For prototype testing the inventory now falls back to common ALS/mannequin bones when a custom socket
does not exist:

```text
Equipped weapon      hand_r
Back/special weapon  spine_03
Hip weapon           thigh_r
Knife                thigh_l
```

This is only a fallback. Final placement should use authored sockets with weapon-specific offsets.

## Fire test

After pickup:

1. Aim with Right Mouse.
2. Fire with Left Mouse.
3. Watch Output Log for magazine count.
4. Fire until empty.
5. Press `R` to reload from the reserve ammo pool.

Shots originate from the controller/camera view direction. The firearm applies configured spread,
traces to `EffectiveRangeCm`, applies point damage on a hit, consumes one round, and reports the
gunshot to BDFR.

## Suppressor test

Place an **IGI Attachment Pickup** after the Glock pickup.

Set:

```text
Use Prototype Pistol Suppressor = true
```

Optionally assign a static mesh to `Prototype Suppressor Mesh`.

When the player walks into it while the Glock is active, it installs on the muzzle slot.

Prototype suppressor tuning:

```text
Gunshot noise multiplier    0.30
Muzzle flash multiplier     0.20
Recoil multiplier           0.96
Spread multiplier           0.98
Weight                      0.24 kg
Suppresses weapon           true
```

The suppressor is intentionally not silent. BDFR still receives a suppressed gunshot event at a
smaller effective hearing range.

## Production asset path

Once the loop is proven, replace the runtime presets with assets:

```text
DA_WPN_Glock17
DA_ATT_PistolSuppressor
```

Then assign real meshes, sockets, sounds, muzzle effects, animation profiles, and final balance data.

The pickup actors already prefer assigned production Data Assets over prototype presets.


## Pickup debugging

Weapon pickup overlap explicitly enables overlap events and prints diagnostic messages to Output Log.

Useful messages include:

```text
IGI Weapon Pickup ready
IGI picked up weapon 'Glock 17'...
AutoEquip failed
no compatible/free carry slot
failed to spawn weapon class
```

The actor also exposes `TryPickupByActor` to Blueprint for manual interaction/debug paths.


## Shot FX test

The prototype pickup now exposes:

```text
Prototype Muzzle Flash Effect
Prototype Muzzle Smoke Effect
Prototype Casing Mesh
```

Assign Niagara muzzle flash/smoke systems and a casing Static Mesh, then fire the Glock.

For correct production placement, add these sockets to the weapon mesh:

```text
Muzzle
SCK_Casing_Eject
```

If your Niagara smoke uses the optional weather inputs, expose:

```text
User.WindVelocity
User.PrecipitationIntensity
User.SurfaceWetness
```
