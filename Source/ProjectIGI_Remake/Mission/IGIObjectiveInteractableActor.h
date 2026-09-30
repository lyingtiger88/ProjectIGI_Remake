#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/IGIInteractable.h"
#include "IGIObjectiveInteractableActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI Mission Objective"))
class PROJECTIGI_REMAKE_API AIGIObjectiveInteractableActor
    : public AActor
    , public IIGIInteractable
{
    GENERATED_BODY()

public:
    AIGIObjectiveInteractableActor();

    virtual bool CanInteract_Implementation(AActor* Interactor) override;
    virtual void Interact_Implementation(AActor* Interactor) override;
    virtual FText GetInteractionPrompt_Implementation() override;

    UFUNCTION(BlueprintPure, Category = "IGI|Mission")
    bool IsObjectiveCompleted() const { return bCompleted; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Mission")
    TObjectPtr<UBoxComponent> InteractionBounds;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Mission")
    TObjectPtr<UStaticMeshComponent> ObjectiveMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Mission")
    FName ObjectiveId = TEXT("PrimaryIntel");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Mission")
    FText InteractionPrompt = FText::FromString(TEXT("Retrieve Intel"));

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Mission")
    bool bHideWhenCompleted = true;

private:
    UPROPERTY(Transient)
    bool bCompleted = false;
};
