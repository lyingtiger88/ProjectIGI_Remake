# Weapons, Inventory, Carry Sockets, and Attachments

ProjectIGI uses a data-driven weapon catalog plus a physically limited player loadout.

The weapon catalog answers **what exists in the game**. The inventory answers **what the player is actually carrying**.

## Carry model

The player can carry at most four regular/special weapons on physical body sockets, plus a dedicated knife slot and separate pouches/utility space for grenades and deployable explosives.

```text
SCK_Weapon_Back_01
SCK_Weapon_Back_02
SCK_Weapon_Hip_R
SCK_Weapon_Special

SCK_Knife

SCK_Grenade_Frag
SCK_Grenade_Flash
SCK_Grenade_Smoke

SCK_Utility_Explosive
```

`UIGIInventoryComponent` enforces the four weapon slots and knife slot. Grenades and utility explosives are counted separately and have their own limits.

A weapon Data Asset defines the body slots it is allowed to use through `CompatibleCarrySlots`. That prevents a launcher or long rifle from being placed in a pistol/hip slot merely because a slot is empty.

## Current weapon catalog

| Category | Weapons |
| --- | --- |
| Knife | Combat Knife |
| Sidearms | Makarov, Glock 17, Glock 17 SD, Colt Anaconda, Desert Eagle, SOCOM, SMG-2 |
| SMGs | MAC-10, Uzi, Twin Uzi, MP5A3, MP5SD3, Type 64 |
| Assault rifles | AK-47, G36, M16A2, AUG, G11 |
| Sniper rifles | SVD Dragunov, PSG-1, PSG-1SD, M82A1, M82A1-T |
| Shotguns | SPAS-12, M1014, Jackhammer |
| Heavy | LAW 80, RPG-7, FN Minimi, M2HB |
| Signal weapon | Flare Gun |
| Grenades | Smoke Grenade, Flashbang, L2A2 Hand Grenade |
| Deployables | Proximity Mine, Claymore, C4 |

Every catalog entry has an `EIGIWeaponId`. Individual weapons should normally be authored as `UIGIWeaponDataAsset` assets rather than adding a new C++ class for every model.

## Runtime class model

```text
AIGIWeaponBase
    └── AIGIFirearmBase
```

`AIGIWeaponBase` owns the skeletal mesh, attachment component, acoustic signature, weight, and carry-noise contribution.

`AIGIFirearmBase` adds magazine ammo, fire-mode selection, reload-from-inventory, and shot notification.

Weapon behavior families are represented in data through `EIGIWeaponFamily` and `EIGIHandlingProfile`. New specialized actor subclasses should only be added when behavior genuinely differs, for example launchers, mounted weapons, or deployable explosives.

## Ammo

Ammo is not an unlimited backpack.

`UIGIInventoryComponent` stores reserve ammo pools by ammo ID. Each weapon Data Asset defines:

- `AmmoType`
- `MagazineCapacity`
- `MaxReserveAmmo`
- supported fire modes

`AIGIFirearmBase::ReloadFromInventory` transfers only the amount needed to fill the magazine.

## Attachments

Supported attachment slots:

```text
Muzzle
Optic
Underbarrel
SideRail
Magazine
Stock
Barrel
Special
```

Standard weapon-mesh sockets:

```text
SCK_Attachment_Muzzle
SCK_Attachment_Optic
SCK_Attachment_Underbarrel
SCK_Attachment_SideRail
SCK_Attachment_Magazine
SCK_Attachment_Stock
SCK_Attachment_Barrel
SCK_Attachment_Special
```

Initial attachment types include:

- suppressor,
- flash hider,
- compensator,
- red dot,
- holographic sight,
- scope / night optic,
- vertical or angled grip,
- bipod,
- laser,
- flashlight,
- light/laser combo,
- extended or drum magazine,
- collapsible or precision stock,
- short or long barrel,
- rail adapter,
- special weapon-specific parts.

`UIGIWeaponAttachmentDataAsset` defines compatibility and modifiers. Compatibility can target exact weapon IDs, whole weapon families, or be universal when no explicit compatibility is listed.

`UIGIWeaponAttachmentComponent` installs/removes attachment data, creates the attachment mesh on the weapon socket, and calculates combined modifiers.

## Suppressors

Suppressors directly affect BDFR AI hearing.

A detachable suppressor uses:

```text
bSuppressesWeapon = true
GunshotNoiseMultiplier < 1.0
MuzzleFlashMultiplier < 1.0
```

Weapons with an integral suppressor use:

```text
bIntegralSuppressor = true
IntegralSuppressorNoiseMultiplier < 1.0
```

A suppressed weapon is quieter, not silent. Nearby AI can still hear the shot, and movement, gear rattle, impact sounds, reload, equip, and attachment handling can still reveal the player.

## Inventory acoustic cost

Every carried weapon contributes weight and `CarryNoiseContribution`. Attachments can change both weight and movement noise.

`UIGIInventoryComponent::GetMovementNoiseMultiplier` combines:

- carried weapon noise,
- attachment movement-noise modifiers,
- carried equipment count,
- weight above the free-weight threshold.

This multiplier feeds `UIGIAcousticSignatureComponent`, so a lightly equipped stealth loadout is quieter than a four-weapon/heavy-equipment loadout.

## Smoke grenade purposes

`EIGISmokePurpose` reserves three gameplay meanings:

- Tactical
- AirSupportMarker
- RescueExtraction

Mission logic can decide which smoke purpose is valid without changing the core throwable/inventory model.

## Content setup checklist

For each weapon:

1. Create a `UIGIWeaponDataAsset`.
2. Set `WeaponId`, family, handling profile, mesh, ammo and fire modes.
3. Set compatible carry slots.
4. Set supported attachment slots.
5. Set weight and acoustic values.
6. Add the required carry and attachment sockets to the relevant skeletal meshes.
7. Create attachment Data Assets only for combinations the real/gameplay weapon should support.


## Flare gun

The flare gun is a specialized one-shot firearm using `AIGIFlareGunBase` and
`AIGIFlareProjectileActor`.

It uses the normal inventory/ammo model but launches a physical, wind-affected flare instead of a
hitscan shot. Flare purposes include illumination, air-support marking, and rescue/extraction
signaling.

See [FLARE_GUN.md](FLARE_GUN.md).


## Medical equipment

Med Kits are counted inventory equipment rather than weapon slots.

Default carry limit:

```text
MaxMedKits = 3
```

Each Med Kit contributes to carried weight and equipment noise. The player can
consume one to heal through the health component, and NPC Med Kits can be looted
using their exact remaining inventory count.

See [MED_KIT.md](MED_KIT.md).
