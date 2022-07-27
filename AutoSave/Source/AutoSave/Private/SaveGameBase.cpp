#include "SaveGameBase.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "GameFramework/Actor.h"


#define DOT TEXT(".")


// Passing false to parent constructor since we don't want to load null-referenced objects
FSaveGameArchive::FSaveGameArchive(FArchive& InInnerArchive) : FObjectAndNameAsStringProxyArchive(InInnerArchive, false)
{
	// Making ArIsSaveGame flag true so it will collect all UPROPERTY() with 'SaveGame' flag
	ArIsSaveGame = true;

	
	// Making ArNoDelta flag true so it will serialize default values too
	ArNoDelta = true;
}





void FObjectSaveRecord::LoadFromMemory(UObject* Object) const
{
	// Setting up this FObjectSaveRecord to get data from memory
	FMemoryReader MemoryStream(Data);
	FSaveGameArchive LoadStream(MemoryStream);

	// Passing data from this FObjectSaveRecord to object
	Object->Serialize(LoadStream);
}


FObjectSaveRecord FObjectSaveRecord::SaveToMemory(UObject* Object)
{
	FObjectSaveRecord Record;

	// Setting up FObjectSaveRecord (just we created at first line) to save data inside it
	FMemoryWriter MemoryStream(Record.Data);
	FSaveGameArchive SaveStream(MemoryStream);

	// Passing data from object to FObjectSaveRecord (just we created at first line) to save disk
	Object->Serialize(SaveStream);
	return Record;
}





void USaveGameBase::LoadObject(UObject* Object)
{
	if(IsValid(Object))
		LoadObjectInternal(Object, Object->GetPathName(), &SavedObjects);
}


void USaveGameBase::SaveObject(UObject* Object)
{
	if(IsValid(Object))
		SaveObjectInternal(Object, Object->GetPathName(), &SavedObjects);
}



void USaveGameBase::LoadObjectWithKey(UObject* Object, const FString MapKey)
{
	if(IsValid(Object))
		LoadObjectInternal(Object, MapKey, &SavedObjects);
}


void USaveGameBase::SaveObjectWithKey(UObject* Object, const FString MapKey)
{
	if(IsValid(Object))
		SaveObjectInternal(Object, MapKey, &SavedObjects);
}



void USaveGameBase::ClearSavedObjects()
{
	SavedObjects.Empty();
}



void USaveGameBase::LoadActor(AActor* Actor)
{
	if(!IsValid(Actor))
		return;
	
	// Load actor
	LoadObjectInternal(Actor, Actor->GetPathName(), &SavedObjects);


	// Get all components of actor
	TInlineComponentArray<UActorComponent*> Components(Actor);

	// Load all components
	for(UActorComponent* Component : Components)
	{
		LoadObjectInternal(Component, Component->GetPathName(), &SavedObjects);
	}
}


void USaveGameBase::SaveActor(AActor* Actor)
{
	if(!IsValid(Actor))
		return;
	
	// Save actor
	SaveObjectInternal(Actor, Actor->GetPathName(), &SavedObjects);


	// Get all components of actor
	TInlineComponentArray<UActorComponent*> Components(Actor);

	// Save all components
	for(UActorComponent* Component : Components)
	{
		SaveObjectInternal(Component, Component->GetPathName(), &SavedObjects);
	}
}



void USaveGameBase::LoadActorWithKey(AActor* Actor, const FString MapKey)
{
	if(!IsValid(Actor))
		return;
	
	// Load actor
	LoadObjectInternal(Actor, MapKey, &SavedObjects);


	// Load actor
	TInlineComponentArray<UActorComponent*> Components(Actor);

	// Load all components
	for(UActorComponent* Component : Components)
	{
		LoadObjectInternal(Component, MapKey + DOT + Component->GetName(), &SavedObjects);
	}
}


void USaveGameBase::SaveActorWithKey(AActor* Actor, const FString MapKey)
{
	if(!IsValid(Actor))
		return;
	
	// Save actor
	SaveObjectInternal(Actor, MapKey, &SavedObjects);


	// Get all components of actor
	TInlineComponentArray<UActorComponent*> Components(Actor);

	// Save all components
	for(UActorComponent* Component : Components)
	{
		SaveObjectInternal(Component, MapKey + DOT + Component->GetName(), &SavedObjects);
	}
}
