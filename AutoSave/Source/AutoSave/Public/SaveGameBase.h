#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "SaveGameBase.generated.h"

class AActor;

// A child archive that we can edit it's default values via it's constructor
class AUTOSAVE_API FSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
public:
	FSaveGameArchive(FArchive& InInnerArchive);
};



// A struct that holds objects data as array and handles loading & saving
USTRUCT()
struct AUTOSAVE_API FObjectSaveRecord
{
	GENERATED_BODY()

public:
	void LoadFromMemory(UObject* Object) const;
	static FObjectSaveRecord SaveToMemory(UObject* Object);

private:
	UPROPERTY()
		TArray<uint8> Data;
};



/** A SaveGame class that can handle loading and saving properties marked with 'SaveGame' */
UCLASS()
class AUTOSAVE_API USaveGameBase : public USaveGame
{
	GENERATED_BODY()

protected:
	// Internal functions to load & save object
	FORCEINLINE void LoadObjectInternal(UObject* Object, const FString MapKey, TMap<FString, FObjectSaveRecord>* MapToLoadFrom) const;
	FORCEINLINE void SaveObjectInternal(UObject* Object, const FString MapKey, TMap<FString, FObjectSaveRecord>* MapToSaveTo) const;

	// TMap to store saved objects data
	UPROPERTY()
		TMap<FString, FObjectSaveRecord> SavedObjects;
	
public:
	// Next 4 functions are wrapper functions around internal ones
	UFUNCTION(BlueprintCallable, Category = "AutoSaveGameBase")
		void LoadObject(UObject* Object);
	UFUNCTION(BlueprintCallable, Category = "AutoSaveGameBase")
		void SaveObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "AutoSaveGameBase")
		void LoadObjectWithKey(UObject* Object, const FString MapKey);
	UFUNCTION(BlueprintCallable, Category = "AutoSaveGameBase")
		void SaveObjectWithKey(UObject* Object, const FString MapKey);


	// A function that clears all saved objects
	UFUNCTION(BlueprintCallable, Category = "AutoSaveGameBase")
		void ClearSavedObjects();
	

	// Next 4 functions are wrapper functions around internal ones. They loads & saves actors with their components
	UFUNCTION(BlueprintCallable, Category = "AutoSaveGameBase")
		void LoadActor(AActor* Actor);
	UFUNCTION(BlueprintCallable, Category = "AutoSaveGameBase")
		void SaveActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "AutoSaveGameBase")
		void LoadActorWithKey(AActor* Actor, const FString MapKey);
	UFUNCTION(BlueprintCallable, Category = "AutoSaveGameBase")
		void SaveActorWithKey(AActor* Actor, const FString MapKey);
};
