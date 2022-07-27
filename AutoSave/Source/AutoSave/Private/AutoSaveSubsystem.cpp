#include "AutoSaveSubsystem.h"

#include "AutoSaveComponent.h"
#include "AutoSaveGame.h"
#include "Kismet/GameplayStatics.h"

#define DOT TEXT(".")



UAutoSaveSubsystem::UAutoSaveSubsystem()
{
	bShouldLoadNextLevel = true;
	bShouldSaveCurrentLevel = true;
	bIsCurrentLevelLoaded = false;
	bIsCurrentLevelSaved = false;
	bIsLevelActive = false;
	bShouldLoadFromCheckpoint = false;
	bIsLoadedFromCheckpoint = false;

	SaveGameSlotName = TEXT("SingleSaveSlot");
	CheckpointName = TEXT("SingleCheckpoint");

	SaveGameObject = nullptr;
	CurrentLevelREF = nullptr;
	CurrentLevelName = TEXT("");
}





void UAutoSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Bind events
	FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UAutoSaveSubsystem::OnWorldInitializedActors);
	FWorldDelegates::OnWorldCleanup.AddUObject(this, &UAutoSaveSubsystem::OnWorldCleanup);
}





void UAutoSaveSubsystem::OnWorldInitializedActors(const UWorld::FActorsInitializedParams& Params)
{
	if(!IsValid(GetWorld()))
		return;
	
	bShouldSaveCurrentLevel = true;
	bIsLevelActive = true;
	CurrentLevelREF = GetWorld();
	CurrentLevelName = CurrentLevelREF->GetName();


	
	// Load level, load all AutoSaveActors and Notify PreLoad & PostLoad
	OnWorldPreLoad.Broadcast();
	
	LoadGameForCurrentLevel();
	SaveGameObject->AutoLoadAllAutoSaveActors(&AutoSaveActorsToLoadAndSave, CurrentLevelREF);

	bShouldLoadNextLevel = true;
	bShouldLoadFromCheckpoint = false;
	
	OnWorldPostLoad.Broadcast();
}


void UAutoSaveSubsystem::OnWorldCleanup(UWorld* WorldToCleanup, bool bSessionEnded, bool bCleanupResources)
{
	if(CurrentLevelName != WorldToCleanup->GetName())
		return;


	
	// Save all AutoSaveActors which requested it, save level and notify PreSave & PostSave
	OnWorldPreSave.Broadcast();
	
	SaveGameObject->AutoSaveAllAutoSaveActors(&AutoSaveActorsToLoadAndSave, &SpawnedAutoSaveActorsToSave);
	SaveGameForCurrentLevel();

	OnWorldPostSave.Broadcast();


	
	// Mark current level as inactive, so we won't be able to save destroyed level by mistake
	bIsLevelActive = false;
}





bool UAutoSaveSubsystem::LoadGameForCurrentLevel()
{
	// If user didn't requested not to load and save game exists in disk, load it
	if(bShouldLoadNextLevel && UGameplayStatics::DoesSaveGameExist((bShouldLoadFromCheckpoint? SaveGameSlotName + DOT + CurrentLevelName + DOT + CheckpointName : SaveGameSlotName + DOT + CurrentLevelName), 0))
	{
		SaveGameObject = Cast<UAutoSaveGame>(UGameplayStatics::LoadGameFromSlot((bShouldLoadFromCheckpoint? SaveGameSlotName + DOT + CurrentLevelName + DOT + CheckpointName : SaveGameSlotName + DOT + CurrentLevelName), 0));

		bIsCurrentLevelLoaded = true;
		return true;
	}

	// Create a new SaveGameObject
	SaveGameObject = Cast<UAutoSaveGame>(UGameplayStatics::CreateSaveGameObject(UAutoSaveGame::StaticClass()));

	bIsCurrentLevelLoaded = false;
	return false;
}


void UAutoSaveSubsystem::SaveGameForCurrentLevel(bool bIsCheckpoint)
{
	// If SaveGameObject is not valid or user requested not to save level or level is already destroyed, we shouldn't try to save it
	if(!IsValid(SaveGameObject) || !bShouldSaveCurrentLevel || !bIsLevelActive)
	{
		bIsCurrentLevelSaved = false;
		return;
	}

	// Save level to disk
	UGameplayStatics::SaveGameToSlot(SaveGameObject, (bIsCheckpoint? SaveGameSlotName + DOT + CurrentLevelName + DOT + CheckpointName : SaveGameSlotName + DOT + CurrentLevelName), 0);
	bIsCurrentLevelSaved = true;
}





void UAutoSaveSubsystem::DoNotLoadNextLevel()
{
	// Marking bShouldLoad to false since user requested not to load next level
	bShouldLoadNextLevel = false;
}


void UAutoSaveSubsystem::DoNotSaveThisLevel()
{
	// Marking bShouldSave to false since user requested not to save level
	bShouldSaveCurrentLevel = false;
}


void UAutoSaveSubsystem::LoadNextLevelFromCheckpoint()
{
	// Marking bShouldLoadFromCheckpoint to true since user requested to load from checkpoint
	bShouldLoadFromCheckpoint = true;
}


void UAutoSaveSubsystem::SaveCheckpoint()
{
	SaveGameForCurrentLevel(true);
}





UWorld* UAutoSaveSubsystem::GetCurrentLevel() const
{
	return CurrentLevelREF;
}


FString UAutoSaveSubsystem::GetCurrentLevelName() const
{
	return CurrentLevelName;
}



bool UAutoSaveSubsystem::ShouldLoadNextLevel() const
{
	return bShouldLoadNextLevel;
}


bool UAutoSaveSubsystem::ShouldSaveCurrentLevel() const
{
	return bShouldSaveCurrentLevel;
}


bool UAutoSaveSubsystem::IsCurrentLevelLoaded() const
{
	return bIsCurrentLevelLoaded;
}


bool UAutoSaveSubsystem::IsCurrentLevelSaved() const
{
	return bIsCurrentLevelSaved;
}


bool UAutoSaveSubsystem::IsLevelActive() const
{
	return bIsLevelActive;
}


bool UAutoSaveSubsystem::ShouldLoadFromCheckpoint() const
{
	return bShouldLoadFromCheckpoint;
}


bool UAutoSaveSubsystem::IsLoadedFromCheckpoint() const
{
	return bIsLoadedFromCheckpoint;
}





// Wrapper function around UAutoSaveGame::ManualLoadSpawnedAutoSaveActor
AActor* UAutoSaveSubsystem::ManualLoadSpawnedAutoSaveActor(FString ManualLoadKey, bool& bSuccess)
{
	// If SaveGameObject is not valid or level already destroyed we shouldn't try to load actor
	if(!IsValid(SaveGameObject) || !bIsLevelActive)
	{
		bSuccess = false;
		return nullptr;
	}


	// Load actor
	return SaveGameObject->ManualLoadSpawnedAutoSaveActor(CurrentLevelREF, ManualLoadKey, bSuccess);
}





// Loads an object which saved to CurrentLevel
void UAutoSaveSubsystem::LoadObjectFromThisLevel(UObject* Object, const FString MapKey)
{
	if(!IsValid(SaveGameObject))
		return;

	SaveGameObject->LoadObjectWithKey(Object, MapKey);
}


// Save an object to CurrentLevel
void UAutoSaveSubsystem::SaveObjectToThisLevel(UObject* Object, const FString MapKey)
{
	if(!IsValid(SaveGameObject))
		return;

	SaveGameObject->SaveObjectWithKey(Object, MapKey);
}

// Delete an objects data from CurrentLevel
bool UAutoSaveSubsystem::DeleteSavedObjectDataFromThisLevel(const FString MapKey)
{
	return SaveGameObject->SavedObjects.Find(MapKey) ? SaveGameObject->SavedObjects.Remove(MapKey) > 0 : false;
}



// Loads an Actor which saved to CurrentLevel
void UAutoSaveSubsystem::LoadActorFromThisLevel(AActor* Actor, const FString MapKey)
{
	if(!IsValid(SaveGameObject))
		return;

	SaveGameObject->LoadActorWithKey(Actor, MapKey);
}


// Save an Actor to CurrentLevel
void UAutoSaveSubsystem::SaveActorToThisLevel(AActor* Actor, const FString MapKey)
{
	if(!IsValid(SaveGameObject))
		return;

	SaveGameObject->SaveActorWithKey(Actor, MapKey);
}

// Delete an actors data from CurrentLevel
bool UAutoSaveSubsystem::DeleteSavedActorDataFromThisLevel(const FString MapKeyIfNonAutoSaveActor)
{
	return SaveGameObject->SavedObjects.Find(MapKeyIfNonAutoSaveActor) ? SaveGameObject->SavedObjects.Remove(MapKeyIfNonAutoSaveActor) > 0 : false;
}





bool UAutoSaveSubsystem::DeleteSaveSlot(const FString SlotName, const FString WorldName, const bool bIsCheckpoint, const FString CheckpointSlotName)
{
	return UGameplayStatics::DeleteGameInSlot(bIsCheckpoint? SlotName + DOT + WorldName + DOT + CheckpointSlotName : SlotName + DOT + WorldName, 0);
}


bool UAutoSaveSubsystem::DeleteSavedAutoSaveActorDataFromThisLevel(const AActor* Actor)
{
	const UAutoSaveComponent* AutoSaveComponent = Actor->FindComponentByClass<UAutoSaveComponent>();
	
	if(!IsValid(AutoSaveComponent))
		return false;

	if(AutoSaveComponent->bActorPlacedInWorld)
		return SaveGameObject->AutoSavedObjects.Find(AutoSaveComponent->MapKey + DOT + AutoSaveComponent->GetName()) ? SaveGameObject->AutoSavedObjects.Remove(AutoSaveComponent->MapKey + DOT + AutoSaveComponent->GetName()) > 0 : false;

	return SaveGameObject->SavedSpawnedActorsAutoLoad.Find(FSpawnedActorData(Actor))? SaveGameObject->SavedSpawnedActorsAutoLoad.Remove(FSpawnedActorData(Actor)) > 0 : false;
}