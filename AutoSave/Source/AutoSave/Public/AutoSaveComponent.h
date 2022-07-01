#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FAutoSaveActor.h"
#include "AutoSaveComponent.generated.h"



/** A component that handles loading & saving automatically */
UCLASS(ClassGroup = (AutoSave), meta = (BlueprintSpawnableComponent), HideCategories = (Variable, ComponentTick, ComponentReplication, Activation, Cooking, Collision, AssetUserData, Tags), AutoExpandCategories = ("Config | General", "Config | Placed", "Config | Spawned", "Info"))
class AUTOSAVE_API UAutoSaveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Making these classes friend so they can access our private members
	friend class UAutoSaveGame;
	friend class UAutoSaveSubsystem;	

	UAutoSaveComponent();

	// Delegates for owner actor
	UPROPERTY(BlueprintAssignable)
		FOnLoadAndSaveEvent OnActorPreLoad;
	UPROPERTY(BlueprintAssignable)
		FOnLoadAndSaveEvent OnActorPostLoad;
	
	UPROPERTY(BlueprintAssignable)
		FOnLoadAndSaveEvent OnActorPreSave;
	UPROPERTY(BlueprintAssignable)
		FOnLoadAndSaveEvent OnActorPostSave;

	
	virtual void OnRegister() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;


	// CPP getters
	FORCEINLINE bool IsPlacedInWorld() const;
	FORCEINLINE FString GetMapKey() const;
	FORCEINLINE bool IsLoaded() const;
	FORCEINLINE bool IsSaved() const;


private:
	// Config variable
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config | General", SaveGame, BlueprintSetter = "SetEnabled", meta = (AllowPrivateAccess))
		bool bEnabled;
	
public:
	// Setter for bEnabled
	UFUNCTION(BlueprintSetter)
		void SetEnabled(const bool NewValue);


	
	// Config variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config | General", SaveGame)
		bool bLoadAndSaveOtherComponents;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config | General", SaveGame)
		EAutoSaveGroup AutoLoadGroup;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config | General", SaveGame)
		EAutoSaveGroup AutoSaveGroup;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config | Placed", SaveGame)
		bool bSavePlacedActorWhenDestroyedBeforeWorldDestroys;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config | Spawned", SaveGame)
		bool bLoadAndSaveSpawnedActor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config | Spawned", SaveGame)
		bool bManualLoad;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config | Spawned", SaveGame, meta = (DisplayName = "Manual Load Key (Unique)"))
		FString ManualLoadKey;
	

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess))
		bool bActorPlacedInWorld;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess))
		FString MapKey;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess))
		bool bIsLoaded;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess))
		bool bIsSaved;

	
	
	UPROPERTY()
		UAutoSaveSubsystem* AutoSaveSubsystemREF;
};
