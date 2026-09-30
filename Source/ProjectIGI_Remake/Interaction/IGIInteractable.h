#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IGIInteractable.generated.h"

UINTERFACE(BlueprintType)
class PROJECTIGI_REMAKE_API UIGIInteractable : public UInterface
{
    GENERATED_BODY()
};

class PROJECTIGI_REMAKE_API IIGIInteractable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IGI|Interaction")
    bool CanInteract(AActor* Interactor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IGI|Interaction")
    void Interact(AActor* Interactor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IGI|Interaction")
    FText GetInteractionPrompt();
};
