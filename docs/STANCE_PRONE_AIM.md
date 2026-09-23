# Stance, Prone, and Aiming System

ProjectIGI uses a three-state tactical stance model inspired by the feel of modern stealth-action third-person games.

The implementation is project-owned and layered on top of ALS rather than modifying ALS itself.

## States

```text
Standing
Crouching
Prone
```

ALS natively supports standing and crouching. ProjectIGI adds its own `EIGIPlayerStance::Prone` state and keeps ALS in its crouched locomotion state while prone so existing movement/aim infrastructure remains usable.

## Input behavior

The existing ALS crouch input is reused:

```text
Tap crouch:
Standing <-> Crouching

Hold crouch for 0.35 s:
Standing/Crouching -> Prone

Tap crouch while Prone:
Prone -> Crouching

Jump while Prone:
Prone -> Crouching

Jump while Crouching:
Crouching -> Standing

Sprint while Crouching/Prone:
Attempts to stand first, then sprint
```

The hold duration is editable through `HoldCrouchToProneSeconds`.

## Aim in every stance

Aim input never forces the player back to standing.

```text
Standing + Aim
Crouching + Aim
Prone + Aim
```

all remain valid combinations.

`IsProneAiming()` is exposed to Blueprint/animation code for a dedicated prone-aim pose or state machine.

## Camera behavior

The spring-arm camera interpolates between stance-aware third-person targets:

- standing hip,
- crouching hip,
- prone hip,
- standing aim,
- crouching aim,
- prone aim.

Aiming moves the camera closer over the shoulder and narrows field of view.

Pressing `Q` swaps right/left shoulder. The shoulder swap works in all three stances and while aiming.

Important tunables on `AIGIPlayerCharacter`:

```text
StandingCameraOffset
CrouchingCameraOffset
ProneCameraOffset

StandingAimCameraOffset
CrouchingAimCameraOffset
ProneAimCameraOffset

HipCameraArmLength
ProneHipCameraArmLength
AimCameraArmLength
ProneAimCameraArmLength

HipFieldOfView
AimFieldOfView
ProneAimFieldOfView
CameraTransitionSpeed
```

## Prone collision

ProjectIGI changes the crouched collision half-height while prone rather than adding a new ALS stance tag.

Defaults:

```text
ALS crouch half-height   56 cm
IGI prone half-height   34 cm
```

Feet are kept at approximately the same world height when the capsule changes.

Before expanding from prone to crouch or standing, a capsule overlap test checks available clearance. If there is not enough room, the stance change is rejected.

## Prone movement

Prone movement uses normal camera-relative movement input but scales the input magnitude.

Default:

```text
ProneMovementInputScale = 0.36
```

This keeps the existing ALS movement component while making crawling much slower than crouched/standing movement.

Animation-friendly values exposed by the player:

```text
GetPlayerStance()
IsProne()
IsProneAiming()
GetProneMovementDirectionAngle()
GetProneNormalizedSpeed()
OnPlayerStanceChanged
```

These are intended for the future prone animation layer.

## Animation requirement

The gameplay, collision, movement, aiming, camera, and AI-hearing behavior are implemented in C++.

ALS-Refactored does **not** ship a prone locomotion set. Until dedicated prone animations are authored and wired into the project's animation layer, ALS will still provide its crouched animation presentation while the gameplay state is prone.

The production animation layer should add:

- enter prone,
- exit prone to crouch,
- prone idle,
- prone forward crawl,
- prone backward crawl,
- prone left/right crawl,
- prone turn,
- prone aim idle,
- prone aim locomotion,
- prone firearm recoil,
- prone reload variants where required.

Do not modify the upstream ALS plugin for these animations; use a ProjectIGI-owned linked/layered animation setup.

## Acoustic stealth

Prone is quieter than crouch for BDFR hearing.

Default stance multipliers:

```text
Standing = 1.00
Crouch   = 0.50
Prone    = 0.28
```

The stance multiplier is still combined with movement speed, surface type, and carried equipment load.

This means crawling slowly on grass with a light load produces a much smaller AI-hearing signature than running upright on metal with a heavy load.
