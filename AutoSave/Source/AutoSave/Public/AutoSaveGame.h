#pragma once

#include "CoreMinimal.h"
#include "SaveGameBase.h"
#include "FAutoSaveActor.h"
#include "GameFramework/Actor.h"
#include "AutoSaveGame.generated.h"


// A struct that holds required data to spawn actor
USTRUCT()
struct FSpawnedActorData
{
	GENERATED_BODY()

	UPROPERTY()
		EAutoSaveGroup AutoLoadGroup;

	UPROPERTY()
		FObjectSaveRecord ActorData;

	UPROPERTY()
		TSubclassOf<AActor> ActorClass;
	UPROPERTY()
		FTransform ActorTransform;

	UPROPERTY()
		bool bLoadAndSaveOtherComponents;
	UPROPERTY()
		TMap<FString, FObjectSaveRecord> ComponentData;
};



/** A SaveGame class that have AutoSave functions */
UCLASS(NotBlueprintable, NotBlueprintType)
class UAutoSaveGame : public USaveGameBase
{
	GENERATED_BODY()

public:
	// Making these classes friend so they can access our private members
	friend class UAutoSaveSubsystem;
	friend class UAutoSaveComponent;

private:
	// A TMap dedicated to auto saved objects
	UPROPERTY()
		TMap<FString, FObjectSaveRecord> AutoSavedObjects;

	// A TArray dedicated to actors that spawned at runtime an auto loaded
	UPROPERTY()
		TArray<FSpawnedActorData> SavedSpawnedActorsAutoLoad;

	// A TArray dedicated to actors that spawned at runtime an manual loaded
	UPROPERTY()
		TMap<FString, FSpawnedActorData> SavedSpawnedActorsManualLoad;


	// Load & save AutoSaveActor
	void LoadAutoSaveActor(FAutoSaveActor AutoSaveActor);
	void SaveAutoSaveActor(FAutoSaveActor AutoSaveActor);


	// Auto load & save all AutoSaveActors
	void AutoLoadAllAutoSaveActors(TArray<FAutoSaveActor>* AutoSaveActorArray, UWorld* WorldREF);
	void AutoSaveAllAutoSaveActors(TArray<FAutoSaveActor>* AutoSaveActorArray, TArray<FAutoSaveActor>* SpawnedAutoSaveActorsArray);

	
	// Auto load & save all cheap AutoSaveActors
	void AutoLoadAllAutoSaveActorsCheap(TArray<FAutoSaveActor>* AutoSaveActorArrayCheap, TArray<FSpawnedActorData>* SavedSpawnedActorsAutoLoadCheap, UWorld* WorldREF);
	void AutoSaveAllAutoSaveActorsCheap(TArray<FAutoSaveActor>* AutoSaveActorArrayCheap, TArray<FAutoSaveActor>* SavedSpawnedActorsAutoSaveCheap);

	// Auto load & save all expensive AutoSaveActors
	void AutoLoadAllAutoSaveActorsExpensive(TArray<FAutoSaveActor>* AutoSaveActorArrayExpensive, TArray<FSpawnedActorData>* SavedSpawnedActorsAutoLoadExpensive, UWorld* WorldREF);
	void AutoSaveAllAutoSaveActorsExpensive(TArray<FAutoSaveActor>* AutoSaveActorArrayExpensive, TArray<FAutoSaveActor>* SavedSpawnedActorsAutoSaveExpensive);

	
	// Manual load SpawnedAutoSaveActor
	AActor* ManualLoadSpawnedAutoSaveActor(UWorld* WorldREF, FString ManualLoadKey, bool& bSuccess);
};
