#pragma once

#include "FAutoSaveActor.generated.h"


class UAutoSaveComponent;
class AActor;



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadAndSaveEvent);



UENUM(BlueprintType)
enum class  EAutoSaveGroup : uint8
{
	Cheap UMETA(DisplayName = "Cheap"),
	Expensive UMETA(DisplayName = "Expensive")
};


USTRUCT()
struct FAutoSaveActor
{
	GENERATED_BODY()

	FAutoSaveActor()
	{
		Actor = nullptr;
		AutoSaveComponent = nullptr;
	}

	FAutoSaveActor(AActor* cActor, UAutoSaveComponent* cAutoSaveComponent)
		: Actor(cActor),
		  AutoSaveComponent(cAutoSaveComponent)
	{
	}

	UPROPERTY()
		AActor* Actor;
	UPROPERTY()
		UAutoSaveComponent* AutoSaveComponent;



	bool operator==(const FAutoSaveActor OtherAutoSaveActor) const
	{
		return Actor == OtherAutoSaveActor.Actor;
	}
};