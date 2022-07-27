#pragma once

#include "CoreMinimal.h"
#include "FAutoSaveActor.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/World.h"
#include "AutoSaveSubsystem.generated.h"


class UAutoSaveGame;
class AActor;


/** A GameInstanceSubsystem that helps us with loading & saving */
UCLASS()
class AUTOSAVE_API UAutoSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Making UAutoSaveComponent friend, so it can access our private members
	friend class UAutoSaveComponent;
	
	UAutoSaveSubsystem();


	// World Delegates
	UPROPERTY(BlueprintAssignable)
		FOnLoadAndSaveEvent OnWorldPreLoad;
	UPROPERTY(BlueprintAssignable)
		FOnLoadAndSaveEvent OnWorldPostLoad;
	
	UPROPERTY(BlueprintAssignable)
		FOnLoadAndSaveEvent OnWorldPreSave;
	UPROPERTY(BlueprintAssignable)
		FOnLoadAndSaveEvent OnWorldPostSave;

	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;


	
	UFUNCTION(BlueprintCallable, Category = "AutoSaveSubsystem")
		void DoNotLoadNextLevel();
	UFUNCTION(BlueprintCallable, Category = "AutoSaveSubsystem")
		void DoNotSaveThisLevel();
	UFUNCTION(BlueprintCallable, Category = "AutoSaveSubsystem")
		void LoadNextLevelFromCheckpoint();
	UFUNCTION(BlueprintCallable, Category = "AutoSaveSubsystem")
		void SaveCheckpoint();
	
	// CPP getters
	FORCEINLINE UWorld* GetCurrentLevel() const;
	FORCEINLINE FString GetCurrentLevelName() const;
	
	FORCEINLINE bool ShouldLoadNextLevel() const;
	FORCEINLINE bool ShouldSaveCurrentLevel() const;
	FORCEINLINE bool IsCurrentLevelLoaded() const;
	FORCEINLINE bool IsCurrentLevelSaved() const;
	FORCEINLINE bool IsLevelActive() const;
	FORCEINLINE bool ShouldLoadFromCheckpoint() const;
	FORCEINLINE bool IsLoadedFromCheckpoint() const;


	// Loads, saves and deletes object from/to CurrentLevel
	UFUNCTION(BlueprintCallable, Category = "AutoSaveSubsystem")
		void LoadObjectFromThisLevel(UObject* Object, const FString MapKey);
	UFUNCTION(BlueprintCallable, Category = "AutoSaveSubsystem")
		void SaveObjectToThisLevel(UObject* Object, const FString MapKey);
	UFUNCTION(BlueprintCallable, Category = "AutoSaveSubsystem")
		bool DeleteSavedObjectDataFromThisLevel(const FString MapKey);


	// Loads, saves and deletes actor from/to CurrentLevel
	UFUNCTION(BlueprintCallable, Category = "AutoSaveSubsystem")
		void LoadActorFromThisLevel(AActor* Actor, const FString MapKey);
	UFUNCTION(BlueprintCallable, Category = "AutoSaveSubsystem")
		void SaveActorToThisLevel(AActor* Actor, const FString MapKey);
	UFUNCTION(BlueprintCallable, Category = "AutoSaveSubsystem")
		bool DeleteSavedActorDataFromThisLevel(const FString MapKeyIfNonAutoSaveActor = TEXT(""));


	// Wrapper function around UAutoSaveGame::ManualLoadSpawnedAutoSaveActor
	UFUNCTION(BlueprintCallable, Category = "AutoSaveSubsystem")
		AActor* ManualLoadSpawnedAutoSaveActor(FString ManualLoadKey, bool& bSuccess);


	// Function that deletes save slot from disk
	UFUNCTION(BlueprintCallable, Category = "AutoSaveSubsystem")
		bool DeleteSaveSlot(const FString SlotName, const FString WorldName, const bool bIsCheckpoint = false, const FString CheckpointSlotName = TEXT(""));


	// Function that deletes actors data from this level
	UFUNCTION(BlueprintCallable, Category = "AutoSaveSubsystem")
		bool DeleteSavedAutoSaveActorDataFromThisLevel(const AActor* Actor);
	

	

	UPROPERTY(BlueprintReadWrite, Category = "AutoSaveSubsystem")
		FString SaveGameSlotName;
	UPROPERTY(BlueprintReadWrite, Category = "AutoSaveSubsystem")
		FString CheckpointName;

private:
	bool LoadGameForCurrentLevel();
	void SaveGameForCurrentLevel(const bool bIsCheckpoint = false);

	
	void OnWorldInitializedActors(const UWorld::FActorsInitializedParams& Params);
	void OnWorldCleanup(UWorld* WorldToCleanup, bool bSessionEnded, bool bCleanupResources);


	

	UPROPERTY(BlueprintReadOnly, Category = "AutoSaveSubsystem", meta = (AllowPrivateAccess))
		UWorld* CurrentLevelREF;
	UPROPERTY(BlueprintReadOnly, Category = "AutoSaveSubsystem", meta = (AllowPrivateAccess))
		FString CurrentLevelName;

	UPROPERTY(BlueprintReadOnly, Category = "AutoSaveSubsystem", meta = (AllowPrivateAccess))
		bool bShouldLoadNextLevel;
	UPROPERTY(BlueprintReadOnly, Category = "AutoSaveSubsystem", meta = (AllowPrivateAccess))
		bool bShouldSaveCurrentLevel;
	UPROPERTY(BlueprintReadOnly, Category = "AutoSaveSubsystem", meta = (AllowPrivateAccess))
		bool bIsCurrentLevelLoaded;
	UPROPERTY(BlueprintReadOnly, Category = "AutoSaveSubsystem", meta = (AllowPrivateAccess))
		bool bIsCurrentLevelSaved;
	UPROPERTY(BlueprintReadOnly, Category = "AutoSaveSubsystem", meta = (AllowPrivateAccess))
		bool bIsLevelActive;
	UPROPERTY(BlueprintReadOnly, Category = "AutoSaveSubsystem", meta = (AllowPrivateAccess))
		bool bShouldLoadFromCheckpoint;
	UPROPERTY(BlueprintReadOnly, Category = "AutoSaveSubsystem", meta = (AllowPrivateAccess))
		bool bIsLoadedFromCheckpoint;



	
	// A TArray dedicated to hold reference to placed AutoSaveActors which wants to be loaded & saved
	UPROPERTY()
		TArray<FAutoSaveActor> AutoSaveActorsToLoadAndSave;
	
	// A TArray dedicated to hold reference to spawned AutoSaveActors which wants to be saved
	UPROPERTY()
		TArray<FAutoSaveActor> SpawnedAutoSaveActorsToSave;

	UPROPERTY()
		UAutoSaveGame* SaveGameObject;
};
