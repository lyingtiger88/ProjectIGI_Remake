#pragma once

#include "CoreMinimal.h"
#include "IGIWeaponTypes.generated.h"

UENUM(BlueprintType)
enum class EIGIWeaponId : uint8
{
    None,
    CombatKnife,
    Makarov,
    Glock17,
    Glock17SD,
    ColtAnaconda,
    DesertEagle,
    SOCOM,
    SMG2,
    MAC10,
    Uzi,
    TwinUzi,
    MP5A3,
    MP5SD3,
    Type64,
    AK47,
    G36,
    M16A2,
    AUG,
    G11,
    SVDDragunov,
    PSG1,
    PSG1SD,
    M82A1,
    M82A1T,
    SPAS12,
    M1014,
    Jackhammer,
    LAW80,
    RPG7,
    FNMinimi,
    M2HB,
    FlareGun,
    SmokeGrenade,
    Flashbang,
    L2A2HandGrenade,
    ProximityMine,
    Claymore,
    C4
};

UENUM(BlueprintType)
enum class EIGIWeaponFamily : uint8
{
    Knife,
    Pistol,
    Revolver,
    SMG,
    AssaultRifle,
    SniperRifle,
    Shotgun,
    LMG,
    Launcher,
    MountedWeapon,
    Throwable,
    DeployableExplosive
};

UENUM(BlueprintType)
enum class EIGIHandlingProfile : uint8
{
    Knife,
    Pistol,
    DualCompact,
    SMG,
    Rifle,
    Sniper,
    Shotgun,
    LMG,
    Launcher,
    Mounted
};

UENUM(BlueprintType)
enum class EIGICarrySlot : uint8
{
    Weapon01,
    Weapon02,
    Weapon03,
    Weapon04,
    Knife,
    FragGrenade,
    Flashbang,
    SmokeGrenade,
    UtilityExplosive
};

UENUM(BlueprintType)
enum class EIGIAttachmentSlot : uint8
{
    Muzzle,
    Optic,
    Underbarrel,
    SideRail,
    Magazine,
    Stock,
    Barrel,
    Special
};

UENUM(BlueprintType)
enum class EIGIAttachmentType : uint8
{
    Suppressor,
    FlashHider,
    Compensator,
    RedDot,
    Holographic,
    Scope,
    NightOptic,
    VerticalGrip,
    AngledGrip,
    Bipod,
    Laser,
    Flashlight,
    ComboLightLaser,
    ExtendedMagazine,
    DrumMagazine,
    CollapsibleStock,
    PrecisionStock,
    ShortBarrel,
    LongBarrel,
    IntegralSuppressor,
    RailAdapter,
    Special
};

UENUM(BlueprintType)
enum class EIGIFireMode : uint8
{
    Safe,
    SemiAutomatic,
    Burst,
    Automatic
};

UENUM(BlueprintType)
enum class EIGIEquipmentType : uint8
{
    FragGrenade,
    Flashbang,
    SmokeGrenade,
    C4,
    ProximityMine,
    Claymore,
    MedKit,
    DistractionObject,
    Binoculars,
    NightVisionGoggles,
    ThermalViewer
};

UENUM(BlueprintType)
enum class EIGISmokePurpose : uint8
{
    Tactical,
    AirSupportMarker,
    RescueExtraction
};

UENUM(BlueprintType)
enum class EIGIFlarePurpose : uint8
{
    Illumination,
    AirSupportMarker,
    RescueExtraction
};
