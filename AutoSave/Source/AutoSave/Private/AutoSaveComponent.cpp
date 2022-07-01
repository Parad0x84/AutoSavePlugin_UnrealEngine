#include "AutoSaveComponent.h"
#include "AutoSaveGame.h"
#include "AutoSaveSubsystem.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#define DOT TEXT(".")

// Sets default values for this component's properties
UAutoSaveComponent::UAutoSaveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	AutoLoadGroup = EAutoSaveGroup::Cheap;
	AutoSaveGroup = EAutoSaveGroup::Cheap;
	

	bEnabled = true;
	bLoadAndSaveOtherComponents = false;
	
	bSavePlacedActorWhenDestroyedBeforeWorldDestroys = false;
	
	bLoadAndSaveSpawnedActor = false;
	bManualLoad = false;
	ManualLoadKey = TEXT("");

	
	bActorPlacedInWorld = false;
	MapKey = TEXT("");
	
	bIsLoaded = false;
	bIsSaved = false;
}





void UAutoSaveComponent::OnRegister()
{
	Super::OnRegister();

	// Make sure the component didn't added at runtime
	check(CreationMethod != EComponentCreationMethod::UserConstructionScript)
}


void UAutoSaveComponent::InitializeComponent()
{
	Super::InitializeComponent();

	
	if(!IsValid(GetWorld()) || !IsValid(UGameplayStatics::GetGameInstance(GetWorld())))
		return;

	// Fill variables
	bActorPlacedInWorld = GetOwner()->bNetStartup;
	MapKey = GetWorld()->GetName() + DOT + GetOwner()->GetName();
	AutoSaveSubsystemREF = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UAutoSaveSubsystem>();

	// If user disabled component or we can't access UAutoSaveSubsystem we shouldn't continue
	if( !bEnabled || !IsValid(AutoSaveSubsystemREF))
		return;


	// Save actor to proper array based on if it's placed in world or spawned
	if(bActorPlacedInWorld)
	{
		AutoSaveSubsystemREF->AutoSaveActorsToLoadAndSave.AddUnique(FAutoSaveActor(GetOwner(), this));
		return;
	}

	if(bLoadAndSaveSpawnedActor)
		AutoSaveSubsystemREF->SpawnedAutoSaveActorsToSave.AddUnique(FAutoSaveActor(GetOwner(), this));
}

void UAutoSaveComponent::UninitializeComponent()
{
	Super::UninitializeComponent();

	// If user disabled component or doesn't requested saving when actor destroyed or actor already saved or actor is spawned at runtime, we shouldn't save it when destroying actor
	if(!bEnabled || !bSavePlacedActorWhenDestroyedBeforeWorldDestroys || !bIsSaved || !bActorPlacedInWorld)
		return;

	// Otherwise save actor
	AutoSaveSubsystemREF->SaveGameObject->SaveAutoSaveActor(FAutoSaveActor(GetOwner(), this));
}





bool UAutoSaveComponent::IsPlacedInWorld() const
{
	return bActorPlacedInWorld;
}

FString UAutoSaveComponent::GetMapKey() const
{
	return MapKey;
}

bool UAutoSaveComponent::IsLoaded() const
{
	return bIsLoaded;
}

bool UAutoSaveComponent::IsSaved() const
{
	return bIsSaved;
}





// Setter for bEnabled
void UAutoSaveComponent::SetEnabled(const bool NewValue)
{
	bEnabled = NewValue;

	if(bEnabled)
	{
		if(bActorPlacedInWorld)
		{
			AutoSaveSubsystemREF->AutoSaveActorsToLoadAndSave.AddUnique(FAutoSaveActor(GetOwner(), this));
			return;
		}

		if(bLoadAndSaveSpawnedActor)
		{
			AutoSaveSubsystemREF->SpawnedAutoSaveActorsToSave.AddUnique(FAutoSaveActor(GetOwner(), this));
			return;
		}
	}

	
	if(bActorPlacedInWorld)
	{
		AutoSaveSubsystemREF->AutoSaveActorsToLoadAndSave.RemoveSingle(FAutoSaveActor(GetOwner(), this));
		return;
	}

	AutoSaveSubsystemREF->SpawnedAutoSaveActorsToSave.RemoveSingle(FAutoSaveActor(GetOwner(), this));
}