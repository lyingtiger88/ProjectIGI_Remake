#include "UI/IGIHUD.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Health/IGIHealthComponent.h"
#include "IGIPlayerCharacter.h"
#include "Inventory/IGIInventoryComponent.h"
#include "Mission/IGIMissionWorldSubsystem.h"
#include "Weapons/IGIFirearmBase.h"
#include "Weapons/IGIWeaponDataAsset.h"

void AIGIHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!IsValid(Canvas) || !IsValid(PlayerOwner) || GEngine == nullptr)
    {
        return;
    }

    DrawMissionStatus(ScreenMargin, ScreenMargin);
    DrawPlayerStatus(ScreenMargin, Canvas->ClipY - 92.0f);

    const float WeaponX = FMath::Max(
        ScreenMargin,
        Canvas->ClipX - 330.0f);
    DrawWeaponStatus(WeaponX, Canvas->ClipY - 68.0f);
}

void AIGIHUD::DrawMissionStatus(const float X, const float Y)
{
    UWorld* World = GetWorld();
    if (!IsValid(World))
    {
        return;
    }

    const UIGIMissionWorldSubsystem* Mission =
        World->GetSubsystem<UIGIMissionWorldSubsystem>();

    if (!IsValid(Mission) ||
        Mission->GetMissionState() == EIGIMissionState::Inactive)
    {
        return;
    }

    FString Status;

    switch (Mission->GetMissionState())
    {
        case EIGIMissionState::PrimaryObjective:
            Status = FString::Printf(
                TEXT("OBJECTIVE  |  Secure %s"),
                *Mission->GetRequiredObjectiveId().ToString());
            break;

        case EIGIMissionState::Extraction:
            Status = TEXT("EXTRACTION  |  Reach the zone and fire Rescue flare");
            break;

        case EIGIMissionState::Completed:
            Status = TEXT("MISSION COMPLETE");
            break;

        case EIGIMissionState::Failed:
            Status = TEXT("MISSION FAILED");
            break;

        case EIGIMissionState::Inactive:
        default:
            return;
    }

    Canvas->DrawText(
        GEngine->GetMediumFont(),
        Status,
        X,
        Y,
        1.0f,
        1.0f,
        FFontRenderInfo());
}

void AIGIHUD::DrawPlayerStatus(const float X, const float Y)
{
    AIGIPlayerCharacter* Player =
        Cast<AIGIPlayerCharacter>(PlayerOwner->GetPawn());

    if (!IsValid(Player))
    {
        return;
    }

    const UIGIHealthComponent* Health = Player->GetHealthComponent();
    const UIGIInventoryComponent* Inventory = Player->GetInventoryComponent();

    const float HealthAlpha = IsValid(Health)
        ? Health->GetHealthNormalized()
        : 0.0f;

    DrawRect(
        FLinearColor(0.04f, 0.04f, 0.04f, 0.80f),
        X,
        Y,
        HealthBarWidth,
        HealthBarHeight);

    DrawRect(
        FLinearColor(0.75f, 0.06f, 0.04f, 0.95f),
        X,
        Y,
        HealthBarWidth * HealthAlpha,
        HealthBarHeight);

    const FString HealthText = IsValid(Health)
        ? FString::Printf(
            TEXT("HP %.0f / %.0f"),
            Health->GetHealth(),
            Health->GetMaxHealth())
        : TEXT("HP --");

    Canvas->DrawText(
        GEngine->GetSmallFont(),
        HealthText,
        X,
        Y - 22.0f);

    const int32 MedKits = IsValid(Inventory)
        ? Inventory->GetEquipmentCount(EIGIEquipmentType::MedKit)
        : 0;

    const int32 Distractions = IsValid(Inventory)
        ? Inventory->GetEquipmentCount(EIGIEquipmentType::DistractionObject)
        : 0;

    Canvas->DrawText(
        GEngine->GetSmallFont(),
        FString::Printf(
            TEXT("MED %d   DISTRACTION %d"),
            MedKits,
            Distractions),
        X,
        Y + 20.0f);
}

void AIGIHUD::DrawWeaponStatus(const float X, const float Y)
{
    AIGIPlayerCharacter* Player =
        Cast<AIGIPlayerCharacter>(PlayerOwner->GetPawn());

    if (!IsValid(Player) || !IsValid(Player->GetInventoryComponent()))
    {
        return;
    }

    const UIGIInventoryComponent* Inventory = Player->GetInventoryComponent();
    const AIGIFirearmBase* Firearm =
        Cast<AIGIFirearmBase>(Inventory->GetActiveWeapon());

    if (!IsValid(Firearm) || !IsValid(Firearm->GetWeaponData()))
    {
        Canvas->DrawText(
            GEngine->GetSmallFont(),
            TEXT("UNARMED"),
            X,
            Y);
        return;
    }

    const UIGIWeaponDataAsset* Data = Firearm->GetWeaponData();
    const int32 Reserve = Data->AmmoType.IsNone()
        ? 0
        : Inventory->GetAmmoCount(Data->AmmoType);

    Canvas->DrawText(
        GEngine->GetMediumFont(),
        FString::Printf(
            TEXT("%s   %d / %d   RES %d"),
            *Data->DisplayName.ToString(),
            Firearm->GetCurrentMagazineAmmo(),
            Firearm->GetMagazineCapacity(),
            Reserve),
        X,
        Y);
}
