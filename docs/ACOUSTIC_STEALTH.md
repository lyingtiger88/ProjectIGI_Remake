# Acoustic Stealth and AI Hearing

ProjectIGI treats movement and carried equipment as part of the player's stealth signature.

## Signal path

```text
Player movement / weapon handling
        |
        v
UIGIAcousticSignatureComponent
        |
        +-- Footstep
        +-- Gear rattle
        +-- Landing
        +-- Vault
        |
Weapon framework
        |
        +-- Equip
        +-- Reload
        +-- Attachment handling
        +-- Gunshot / suppressed gunshot
        |
        v
UBDFRAcousticEventLibrary
        |
        v
UAISense_Hearing
        |
        v
BDFR Awareness / investigation
```

BDFR now exposes `ReportAcousticEvent` for lightweight semantic hearing events that do not apply physical hearing damage/exposure.

## Semantic tags

```text
BDFR.Acoustic.Movement.Footstep
BDFR.Acoustic.Movement.Gear
BDFR.Acoustic.Movement.Landing
BDFR.Acoustic.Movement.Vault
BDFR.Acoustic.Movement.ProneRoll

BDFR.Acoustic.Weapon.Equip
BDFR.Acoustic.Weapon.Reload
BDFR.Acoustic.Weapon.Attachment

BDFR.Acoustic.Gunshot
BDFR.Acoustic.Gunshot.Suppressed
```

## Movement noise

Automatic footstep emission is distance-based rather than frame-based. The component accumulates planar movement and emits a footstep when the configured stride distance is reached.

The final footstep signature combines:

```text
movement speed
x stance
x physical surface
x inventory/load noise
x event intensity
```

Default surface direction:

| Surface | Relative acoustic effect |
| --- | ---: |
| Grass | quiet |
| Sand | quiet |
| Mud | low |
| Dirt | low |
| Snow | medium |
| Concrete | louder |
| Water | loud |
| Metal | loudest |

The same physical-surface resolver already used for footprint/scent tracking supplies the acoustic surface category.

## Carry load

`UIGIInventoryComponent` exposes `GetMovementNoiseMultiplier`.

The multiplier rises with:

- more/louder weapons,
- heavier weapons,
- heavier attachments,
- attachments that increase gear noise,
- grenades and deployable equipment,
- total carry weight above the configured threshold.

This means inventory choice is a stealth choice.

## Gear rattle

Gear noise is rate-limited and emitted only while the player is moving and carrying enough equipment to exceed the quiet baseline.

It is intentionally separate from footsteps so AI can still react to equipment movement even on a relatively quiet surface.

## Landing and vaulting

`AIGIPlayerCharacter::Landed` reports a landing acoustic event with intensity scaled from vertical speed.

Vault noise is exposed as `ReportVault` and is ready to be called by the future vault/mantle gameplay hook.

## Weapons

Weapon equip, reload, attachment changes, and gunshots report through BDFR.

Suppressors reduce the gunshot hearing radius/strength but do not remove the event. Suppressed shots remain detectable at short range.

Normal movement and gear events do not apply BDFR acoustic exposure/hearing damage. Gunshots and explosions keep their existing physical-exposure behavior.

## AI behavior

A heard movement event should normally build suspicion and provide a last-heard location rather than automatically granting visual confirmation.

BDFR difficulty hearing multipliers and canine species hearing multipliers continue to apply, so higher-difficulty enemies and dogs can react to weaker acoustic signatures sooner.

## Tuning

Important project-level tuning points:

- `BaseStepDistanceCm`
- `BaseFootstepLoudness`
- `BaseFootstepRadius`
- `BaseGearLoudness`
- `BaseGearRadius`
- `GearNoiseIntervalSeconds`
- `CrouchNoiseMultiplier`
- weapon `CarryNoiseContribution`
- attachment `MovementNoiseMultiplier`
- suppressor `GunshotNoiseMultiplier`

Automatic footsteps can later be disabled in favor of precise animation-notify-driven calls without changing the BDFR integration.


## Prone rolling

Prone left/right rolls emit `BDFR.Acoustic.Movement.ProneRoll`. The event is intentionally modest,
but its range/loudness still scales with the player's current carried-load multiplier.
