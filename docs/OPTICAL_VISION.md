# Binoculars, Night Vision, and Thermal / Infrared Vision

ProjectIGI now has an equipment-gated optical vision system owned by `UIGIVisionComponent`.

## Vision modes

```text
Normal
Binoculars
NightVision
Thermal
```

The modes are mutually exclusive. Selecting the active mode again returns to Normal.

## Prototype controls

```text
B                 Toggle Binoculars
Mouse Wheel Up    Zoom binoculars in
Mouse Wheel Down  Zoom binoculars out
N                 Toggle Night Vision
V                 Toggle Thermal / Infrared
```

Binoculars block firearm aim/fire/reload while active. Night vision and thermal vision can remain
active while using weapons.

The source-only bindings are a prototype path; production input assets can call the same public
component functions later.

## Equipment requirements

By default, vision modes require matching inventory equipment:

```text
Binoculars   -> EIGIEquipmentType::Binoculars
NightVision  -> EIGIEquipmentType::NightVisionGoggles
Thermal      -> EIGIEquipmentType::ThermalViewer
```

Each optical device has a carry limit of one.

Default player loadout:

```text
Start With Binoculars          = true
Start With Night Vision Goggles = false
Start With Thermal Viewer       = false
```

These are editable on `AIGIPlayerCharacter`.

If mission design should allow vision modes without equipment, disable
`bRequireOwnedEquipment` on the `IGIVisionComponent`.

## World pickups

Place:

```text
IGI Vision Equipment Pickup
```

and choose:

```text
Binoculars
NightVisionGoggles
ThermalViewer
```

in `Vision Equipment Type`.

Assign any Static Mesh to `Pickup Mesh` for world presentation.

## Binoculars

Binoculars use the real player camera and provide variable FOV zoom.

Defaults:

```text
Default FOV = 28
Minimum FOV = 10
Maximum FOV = 48
Zoom step   = 4 degrees
```

An optional `BinocularPostProcessMaterial` can provide the binocular mask, lens edge, range markings,
chromatic aberration, dirt, or vignette.

## Night vision goggles

Night vision is a wearable-equipment mode rather than a weapon attachment.

Assign a Post Process material to:

```text
NightVisionPostProcessMaterial
```

Recommended presentation:

- green phosphor / monochrome mapping,
- boosted dark-scene visibility,
- soft bloom around bright lights,
- grain/noise,
- vignette,
- overexposure from strong lights.

A built-in green-tinted fallback grade is applied even when no Material is assigned, so the mode is
immediately testable from C++ alone. An editor-authored Material can then add grain, phosphor noise,
lens distortion, bloom shaping, and other final presentation without rebuilding C++.

## Thermal / infrared

Thermal mode uses:

```text
ThermalPostProcessMaterial
```

and an opt-in `UIGIThermalSignatureComponent`.

A built-in warm/high-contrast fallback grade makes Thermal mode visibly testable even without a
custom Material. Selective hot-target rendering still requires the production Thermal Post Process
material described below.

The thermal signature component marks its owner's primitive components through Custom Depth / Custom
Stencil.

Default stencil:

```text
246
```

Project configuration enables:

```text
r.CustomDepth=3
```

so the Thermal Post Process material can read Custom Stencil.

The default IGI enemy AI controller automatically adds a thermal signature to possessed enemy pawns.
The IGI canine controller does the same for dogs. Other living actors can opt in by adding
`IGIThermalSignatureComponent`.

A production thermal material can therefore:

1. read `CustomStencil == 246`,
2. render those pixels as hot targets,
3. keep the world in a colder palette,
4. optionally vary heat color/intensity by later gameplay parameters.

This avoids treating every world object as a heat source.

## Post-process material setup

Create three optional Material assets with:

```text
Material Domain = Post Process
```

Suggested assets:

```text
M_PP_Binoculars
M_PP_NightVision
M_PP_Thermal
```

Assign them on the player's `IGIVisionComponent`.

The component owns an isolated runtime Post Process component and only inserts its own blendable, so
switching vision modes does not require changing the map's global Post Process Volume.

## Public API

```text
SetVisionMode()
ToggleBinoculars()
ToggleNightVision()
ToggleThermal()
AdjustBinocularZoom()
DisableVision()

GetVisionMode()
IsBinocularsActive()
IsNightVisionActive()
IsThermalActive()
HasRequiredEquipment()
GetFieldOfViewOverride()

OnVisionModeChanged
```

## Future presentation work

The gameplay/rendering hooks are in place. Production polish can add:

- binocular raise/lower animation,
- binocular hand-held mesh,
- NVG flip-down/up animation,
- NVG battery drain,
- thermal heat decay on recently fired weapons,
- hot vehicle engines and warm footprints,
- HUD indicators for active optical mode,
- mission-specific optical restrictions.
