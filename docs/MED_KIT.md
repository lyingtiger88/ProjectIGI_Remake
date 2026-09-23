# Med Kit and Health Foundation

ProjectIGI now has a generic health component plus inventory-backed Med Kits.

## Health component

`UIGIHealthComponent` is a reusable Actor Component.

Defaults:

```text
Max Health = 100
Start At Full Health = true
```

It automatically listens to Unreal's `OnTakeAnyDamage` event, so existing
`ApplyPointDamage` firearm hits reduce health on any actor that owns this component.

Blueprint-facing API:

```text
GetHealth()
GetMaxHealth()
GetHealthNormalized()
IsAlive()
IsFullHealth()
Heal()
ApplyDamage()
SetHealth()

OnHealthChanged
OnDeath
```

The player character owns `IGIHealth` by default.

## Med Kit inventory

`EIGIEquipmentType::MedKit` is a real inventory item.

Default player carry limit:

```text
MaxMedKits = 3
```

Each kit contributes about 0.45 kg to carried weight and participates in the
existing equipment/load acoustic model.

## Using a Med Kit

Temporary source-only control:

```text
H = Use Med Kit
```

Default heal:

```text
MedKitHealAmount = 45 HP
```

A kit is not consumed if:

- the player is already at full health,
- the player is dead,
- no kit is available,
- a prone roll is currently active.

The heal amount is clamped by Max Health.

The public player function is:

```text
UseMedKit()
```

so UI/Enhanced Input can replace the temporary direct H binding later.

## World pickup

Place:

```text
IGI Med Kit Pickup
```

in the level.

Settings:

```text
Med Kit Count = 1
Pickup Mesh = optional Static Mesh
```

Walking into the pickup adds as many kits as the player's remaining capacity permits.
If the inventory is already full, the pickup remains in the world.

## NPC / enemy loot

Actors with both:

```text
IGIInventoryComponent
IGILootableInventoryComponent
```

can carry and loot Med Kits deterministically.

Available calls:

```text
LootAllMedKits(...)
LootAllMedKitsToInventory(...)
GetRemainingMedKitCount()
```

Only the actual remaining Med Kits are transferred. If the recipient can only
accept part of the stack, the rest remain on the source NPC.

## Next presentation work

The gameplay foundation is complete, but production presentation still needs:

- use/heal animation,
- hand-held Med Kit prop,
- use sound,
- HUD health and kit counter,
- optional use duration/interruption,
- AI/NPC healing behavior.
