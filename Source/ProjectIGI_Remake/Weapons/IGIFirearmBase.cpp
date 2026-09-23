#include "Weapons/IGIFirearmBase.h"

#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Environment/IGIWeatherWorldSubsystem.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Inventory/IGIInventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Weapons/IGIShellCasingActor.h"
#include "Weapons/IGIWeaponAttachmentComponent.h"
#include "Weapons/IGIWeaponDataAsset.h"

AIGIFirearmBase::AIGIFirearmBase()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AIGIFirearmBase::BeginPlay()
{
    Super::BeginPlay();
    ResetRuntimeWeaponState();
}

void AIGIFirearmBase::InitializeFromData(UIGIWeaponDataAsset* InWeaponData)
{
    Super::InitializeFromData(InWeaponData);
    ResetRuntimeWeaponState();
}

int32 AIGIFirearmBase::GetMagazineCapacity() const
{
    if (!IsValid(WeaponData))
    {
        return 0;
    }

    const int32 AttachmentBonus = IsValid(AttachmentComponent)
        ? AttachmentComponent->GetMagazineCapacityBonus()
        : 0;

    return FMath::Max(0, WeaponData->MagazineCapacity + AttachmentBonus);
}

bool AIGIFirearmBase::SetFireMode(const EIGIFireMode NewFireMode)
{
    if (!IsValid(WeaponData) || !WeaponData->SupportedFireModes.Contains(NewFireMode))
    {
        return false;
    }

    CurrentFireMode = NewFireMode;
    return true;
}

bool AIGIFirearmBase::TryConsumeRound()
{
    if (CurrentMagazineAmmo <= 0)
    {
        return false;
    }

    --CurrentMagazineAmmo;
    return true;
}

int32 AIGIFirearmBase::ReloadFromInventory(UIGIInventoryComponent* Inventory)
{
    if (!IsValid(Inventory) || !IsValid(WeaponData) || WeaponData->AmmoType.IsNone())
    {
        return 0;
    }

    const int32 Capacity = GetMagazineCapacity();
    const int32 Needed = FMath::Max(0, Capacity - CurrentMagazineAmmo);

    if (Needed <= 0)
    {
        return 0;
    }

    const int32 Consumed = Inventory->ConsumeAmmo(WeaponData->AmmoType, Needed);
    CurrentMagazineAmmo += Consumed;

    if (Consumed > 0)
    {
        ReportReloadNoise();
    }

    return Consumed;
}

int32 AIGIFirearmBase::ExtractMagazineAmmo(const int32 Amount)
{
    if (Amount <= 0 || CurrentMagazineAmmo <= 0)
    {
        return 0;
    }

    const int32 Extracted = FMath::Min(CurrentMagazineAmmo, Amount);
    CurrentMagazineAmmo -= Extracted;
    return Extracted;
}

void AIGIFirearmBase::SetCurrentMagazineAmmo(const int32 NewAmount)
{
    CurrentMagazineAmmo = FMath::Clamp(NewAmount, 0, GetMagazineCapacity());
}

bool AIGIFirearmBase::NotifyShotFired()
{
    if (!TryConsumeRound())
    {
        return false;
    }

    ReportGunshotNoise();
    return true;
}

bool AIGIFirearmBase::FireHitscan(AController* InstigatorController)
{
    bLastShotHit = false;
    LastShotImpactLocation = FVector::ZeroVector;

    if (!IsValid(WeaponData) || !IsValid(InstigatorController) || !NotifyShotFired())
    {
        return false;
    }

    PlayShotEffects();

    FVector ViewLocation = GetActorLocation();
    FRotator ViewRotation = GetActorRotation();
    InstigatorController->GetPlayerViewPoint(ViewLocation, ViewRotation);

    const float SpreadRadians = FMath::DegreesToRadians(FMath::Max(0.0f, GetEffectiveSpread()));
    const FVector ShotDirection = SpreadRadians > KINDA_SMALL_NUMBER
        ? FMath::VRandCone(ViewRotation.Vector(), SpreadRadians)
        : ViewRotation.Vector();

    const FVector TraceEnd = ViewLocation + ShotDirection * FMath::Max(1.0f, WeaponData->EffectiveRangeCm);

    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(IGIWeaponHitscan), true, GetOwner());
    QueryParams.AddIgnoredActor(this);

    FHitResult Hit;
    UWorld* World = GetWorld();

    if (!IsValid(World) || !World->LineTraceSingleByChannel(Hit, ViewLocation, TraceEnd, WeaponTraceChannel, QueryParams))
    {
        LastShotImpactLocation = TraceEnd;
        return true;
    }

    bLastShotHit = true;
    LastShotImpactLocation = Hit.ImpactPoint;

    AActor* HitActor = Hit.GetActor();
    if (IsValid(HitActor))
    {
        UGameplayStatics::ApplyPointDamage(
            HitActor,
            WeaponData->BaseDamage,
            ShotDirection,
            Hit,
            InstigatorController,
            GetAcousticInstigator(),
            nullptr);
    }

    return true;
}

void AIGIFirearmBase::ResetRuntimeWeaponState()
{
    if (!IsValid(WeaponData))
    {
        CurrentMagazineAmmo = 0;
        CurrentFireMode = EIGIFireMode::SemiAutomatic;
        return;
    }

    CurrentFireMode = WeaponData->SupportedFireModes.Contains(WeaponData->DefaultFireMode)
        ? WeaponData->DefaultFireMode
        : (WeaponData->SupportedFireModes.IsEmpty()
            ? EIGIFireMode::SemiAutomatic
            : WeaponData->SupportedFireModes[0]);

    CurrentMagazineAmmo = bStartLoaded ? GetMagazineCapacity() : 0;
}

void AIGIFirearmBase::PlayShotEffects()
{
    if (!IsValid(WeaponData))
    {
        return;
    }

    USceneComponent* VisualComponent = GetWeaponVisualComponent();
    if (!IsValid(VisualComponent))
    {
        return;
    }

    if (!WeaponData->MuzzleFlashEffect.IsNull())
    {
        if (UNiagaraSystem* FlashSystem = WeaponData->MuzzleFlashEffect.LoadSynchronous();
            IsValid(FlashSystem))
        {
            if (UNiagaraComponent* Flash = UNiagaraFunctionLibrary::SpawnSystemAttached(
                    FlashSystem,
                    VisualComponent,
                    WeaponData->MuzzleSocket,
                    FVector::ZeroVector,
                    FRotator::ZeroRotator,
                    EAttachLocation::SnapToTarget,
                    true);
                IsValid(Flash))
            {
                Flash->SetWorldScale3D(
                    FVector(FMath::Max(0.0f, GetEffectiveMuzzleFlashScale())));
            }
        }
    }

    if (!WeaponData->MuzzleSmokeEffect.IsNull())
    {
        if (UNiagaraSystem* SmokeSystem = WeaponData->MuzzleSmokeEffect.LoadSynchronous();
            IsValid(SmokeSystem))
        {
            if (UNiagaraComponent* Smoke = UNiagaraFunctionLibrary::SpawnSystemAttached(
                    SmokeSystem,
                    VisualComponent,
                    WeaponData->MuzzleSocket,
                    FVector::ZeroVector,
                    FRotator::ZeroRotator,
                    EAttachLocation::SnapToTarget,
                    true);
                IsValid(Smoke))
            {
                float SmokeScale = FMath::Max(0.0f, WeaponData->MuzzleSmokeScale);

                if (IsSuppressed())
                {
                    SmokeScale *= WeaponData->SuppressedMuzzleSmokeMultiplier;
                }

                Smoke->SetWorldScale3D(FVector(SmokeScale));

                if (UWorld* World = GetWorld(); IsValid(World))
                {
                    if (const UIGIWeatherWorldSubsystem* Weather =
                            World->GetSubsystem<UIGIWeatherWorldSubsystem>();
                        IsValid(Weather))
                    {
                        const FIGIWeatherState WeatherState = Weather->GetWeatherState();

                        Smoke->SetVariableVec3(
                            TEXT("User.WindVelocity"),
                            Weather->GetWindVelocity());

                        Smoke->SetVariableFloat(
                            TEXT("User.PrecipitationIntensity"),
                            WeatherState.PrecipitationIntensity);

                        Smoke->SetVariableFloat(
                            TEXT("User.SurfaceWetness"),
                            WeatherState.SurfaceWetness);
                    }
                }
            }
        }
    }

    SpawnShellCasing();
}

void AIGIFirearmBase::SpawnShellCasing()
{
    if (!IsValid(WeaponData) || WeaponData->ShellCasingMesh.IsNull())
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!IsValid(World))
    {
        return;
    }

    UStaticMesh* CasingMesh = WeaponData->ShellCasingMesh.LoadSynchronous();
    if (!IsValid(CasingMesh))
    {
        return;
    }

    const TSubclassOf<AIGIShellCasingActor> CasingClass =
        WeaponData->ShellCasingClass
            ? WeaponData->ShellCasingClass
            : AIGIShellCasingActor::StaticClass();

    const FTransform EjectionTransform =
        GetWeaponSocketTransform(WeaponData->CasingEjectionSocket);

    const FVector SideDirection = EjectionTransform.GetUnitAxis(EAxis::Y);
    const float ConeRadians =
        FMath::DegreesToRadians(
            FMath::Clamp(WeaponData->CasingEjectionRandomConeDegrees, 0.0f, 45.0f));

    const FVector EjectionDirection =
        ConeRadians > KINDA_SMALL_NUMBER
            ? FMath::VRandCone(SideDirection, ConeRadians)
            : SideDirection;

    const FVector InitialVelocity =
        EjectionDirection * WeaponData->CasingEjectionSpeed +
        FVector::UpVector * WeaponData->CasingUpwardSpeed;

    const FVector InitialAngularVelocity(
        FMath::FRandRange(-1.0f, 1.0f),
        FMath::FRandRange(-1.0f, 1.0f),
        FMath::FRandRange(-1.0f, 1.0f));

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = GetOwner();
    SpawnParameters.Instigator = GetInstigator();
    SpawnParameters.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AIGIShellCasingActor* Casing = World->SpawnActor<AIGIShellCasingActor>(
        CasingClass,
        EjectionTransform,
        SpawnParameters);

    if (!IsValid(Casing))
    {
        return;
    }

    Casing->InitializeCasing(
        CasingMesh,
        InitialVelocity,
        InitialAngularVelocity.GetSafeNormal() *
            WeaponData->CasingAngularSpeedDegrees,
        WeaponData->CasingLifeSeconds);
}

FTransform AIGIFirearmBase::GetWeaponSocketTransform(const FName SocketName) const
{
    if (IsValid(WeaponMesh) &&
        IsValid(WeaponMesh->GetSkeletalMeshAsset()) &&
        WeaponMesh->DoesSocketExist(SocketName))
    {
        return WeaponMesh->GetSocketTransform(SocketName, RTS_World);
    }

    if (IsValid(StaticWeaponMesh) &&
        IsValid(StaticWeaponMesh->GetStaticMesh()) &&
        StaticWeaponMesh->DoesSocketExist(SocketName))
    {
        return StaticWeaponMesh->GetSocketTransform(SocketName, RTS_World);
    }

    return GetActorTransform();
}
