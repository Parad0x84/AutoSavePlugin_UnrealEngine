#include "AutoSaveGame.h"
#include "AutoSaveComponent.h"
#include "Engine/World.h"


#define DOT TEXT(".")



void UAutoSaveGame::LoadAutoSaveActor(FAutoSaveActor AutoSaveActor)
{
	if(!IsValid(AutoSaveActor.Actor))
		return;
	
	// Notify PreLoad & load actor
	AutoSaveActor.AutoSaveComponent->OnActorPreLoad.Broadcast();
	LoadObjectInternal(AutoSaveActor.Actor, AutoSaveActor.AutoSaveComponent->MapKey, &AutoSavedObjects);

	
	// If requested we load all components. If it's not requested we only load AutoSaveComponent
	if(AutoSaveActor.AutoSaveComponent->bLoadAndSaveOtherComponents)
	{
		TInlineComponentArray<UActorComponent*> Components(AutoSaveActor.Actor);

		for(UActorComponent* Component : Components)
		{
			LoadObjectInternal(Component, AutoSaveActor.AutoSaveComponent->MapKey + DOT + Component->GetName(), &AutoSavedObjects);
		}
	}
	else
	{
		LoadObjectInternal(AutoSaveActor.AutoSaveComponent, AutoSaveActor.AutoSaveComponent->MapKey + DOT + AutoSaveActor.AutoSaveComponent->GetName(), &AutoSavedObjects);
	}
	
	// Notify PostLoad
	AutoSaveActor.AutoSaveComponent->bIsLoaded = true;
	AutoSaveActor.AutoSaveComponent->OnActorPostLoad.Broadcast();
}


void UAutoSaveGame::SaveAutoSaveActor(FAutoSaveActor AutoSaveActor)
{
	if(!IsValid(AutoSaveActor.Actor))
		return;
	
	// Notify PreSave & save actor
	AutoSaveActor.AutoSaveComponent->OnActorPreSave.Broadcast();
	SaveObjectInternal(AutoSaveActor.Actor, AutoSaveActor.AutoSaveComponent->MapKey, &AutoSavedObjects);

	// If requested we save all components. If it's not requested we only save AutoSaveComponent
	if(AutoSaveActor.AutoSaveComponent->bLoadAndSaveOtherComponents)
	{
		TInlineComponentArray<UActorComponent*> Components(AutoSaveActor.Actor);

		for(UActorComponent* Component : Components)
		{
			SaveObjectInternal(Component, AutoSaveActor.AutoSaveComponent->MapKey + DOT + Component->GetName(), &AutoSavedObjects);
		}
	}
	else
	{
		SaveObjectInternal(AutoSaveActor.AutoSaveComponent, AutoSaveActor.AutoSaveComponent->MapKey + DOT + AutoSaveActor.AutoSaveComponent->GetName(), &AutoSavedObjects);
	}

	// Notify PostSave
	AutoSaveActor.AutoSaveComponent->bIsSaved = true;
	AutoSaveActor.AutoSaveComponent->OnActorPostSave.Broadcast();
}





// Separate AutoSaveActors based on their load group & load them
void UAutoSaveGame::AutoLoadAllAutoSaveActors(TArray<FAutoSaveActor>* AutoSaveActorArray, UWorld* WorldREF)
{
	TArray<FAutoSaveActor> AutoSaveActorArrayCheap;
	TArray<FAutoSaveActor> AutoSaveActorArrayExpensive;
	
	TArray<FSpawnedActorData> SavedSpawnedActorsAutoLoadCheap;
	TArray<FSpawnedActorData> SavedSpawnedActorsAutoLoadExpensive;

	
	for(FAutoSaveActor& AutoSaveActor : *AutoSaveActorArray)
	{
		if(IsValid(AutoSaveActor.Actor))
			(AutoSaveActor.AutoSaveComponent->AutoLoadGroup == EAutoSaveGroup::Cheap? AutoSaveActorArrayCheap : AutoSaveActorArrayExpensive).Add(AutoSaveActor);
	}

	for(FSpawnedActorData& SpawnedActorData : SavedSpawnedActorsAutoLoad)
	{
		(SpawnedActorData.AutoLoadGroup == EAutoSaveGroup::Cheap? SavedSpawnedActorsAutoLoadCheap : SavedSpawnedActorsAutoLoadExpensive).Add(SpawnedActorData);
	}

	AutoLoadAllAutoSaveActorsCheap(&AutoSaveActorArrayCheap, &SavedSpawnedActorsAutoLoadCheap, WorldREF);
	AutoLoadAllAutoSaveActorsExpensive(&AutoSaveActorArrayExpensive, &SavedSpawnedActorsAutoLoadExpensive, WorldREF);

	// After spawned all actors clear the TArray. So they won't be loaded again, if they destroyed before world destroys
	SavedSpawnedActorsAutoLoad.Empty();
}


// Separate AutoSaveActors based on their save group & save them
void UAutoSaveGame::AutoSaveAllAutoSaveActors(TArray<FAutoSaveActor>* AutoSaveActorArray, TArray<FAutoSaveActor>* SpawnedAutoSaveActorsArray)
{
	TArray<FAutoSaveActor> AutoSaveActorArrayCheap;
	TArray<FAutoSaveActor> AutoSaveActorArrayExpensive;
	
	TArray<FAutoSaveActor> SpawnedAutoSaveActorCheap;
	TArray<FAutoSaveActor> SpawnedAutoSaveActorExpensive;

	
	for(FAutoSaveActor& AutoSaveActor : *AutoSaveActorArray)
	{
		if(IsValid(AutoSaveActor.Actor))
			(AutoSaveActor.AutoSaveComponent->AutoLoadGroup == EAutoSaveGroup::Cheap? AutoSaveActorArrayCheap : AutoSaveActorArrayExpensive).Add(AutoSaveActor);
	}

	for(FAutoSaveActor& SpawnedAutoSaveActor : *SpawnedAutoSaveActorsArray)
	{
		if(IsValid(SpawnedAutoSaveActor.Actor))
			(SpawnedAutoSaveActor.AutoSaveComponent->AutoSaveGroup == EAutoSaveGroup::Cheap? SpawnedAutoSaveActorCheap : SpawnedAutoSaveActorExpensive).Add(SpawnedAutoSaveActor);
	}

	AutoSaveAllAutoSaveActorsCheap(&AutoSaveActorArrayCheap, &SpawnedAutoSaveActorCheap);
	AutoSaveAllAutoSaveActorsExpensive(&AutoSaveActorArrayExpensive, &SpawnedAutoSaveActorExpensive);
}





// Load all cheap AutoSaveActors
void UAutoSaveGame::AutoLoadAllAutoSaveActorsCheap(TArray<FAutoSaveActor>* AutoSaveActorArrayCheap, TArray<FSpawnedActorData>* SavedSpawnedActorsAutoLoadCheap, UWorld* WorldREF)
{
	// AutoSaveActors
	for(const FAutoSaveActor& AutoSaveActor : *AutoSaveActorArrayCheap)
	{
		// Notify PreLoad & load actor
		AutoSaveActor.AutoSaveComponent->OnActorPreLoad.Broadcast();
		LoadObjectInternal(AutoSaveActor.Actor, AutoSaveActor.AutoSaveComponent->MapKey, &AutoSavedObjects);

		
		// If requested we load all components. If it's not requested we only load AutoSaveComponent
		if(AutoSaveActor.AutoSaveComponent->bLoadAndSaveOtherComponents)
		{
			TInlineComponentArray<UActorComponent*> Components(AutoSaveActor.Actor);

			for(UActorComponent* Component : Components)
			{
				LoadObjectInternal(Component, AutoSaveActor.AutoSaveComponent->MapKey + DOT + Component->GetName(), &AutoSavedObjects);
			}
		}
		else
		{
			LoadObjectInternal(AutoSaveActor.AutoSaveComponent, AutoSaveActor.AutoSaveComponent->MapKey + DOT + AutoSaveActor.AutoSaveComponent->GetName(), &AutoSavedObjects);
		}

		// Notify PostLoad
		AutoSaveActor.AutoSaveComponent->bIsLoaded = true;
		AutoSaveActor.AutoSaveComponent->OnActorPostLoad.Broadcast();
	}





	// SpawnedAutoSaveActors
	for(FSpawnedActorData& SpawnedActorData : *SavedSpawnedActorsAutoLoadCheap)
	{
		// Spawn actor
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		AActor* Actor = WorldREF->SpawnActor<AActor>(SpawnedActorData.ActorClass, SpawnedActorData.ActorTransform, Params);



		// Get AutoSaveComponent
		UAutoSaveComponent* AutoSaveComponent = Actor->FindComponentByClass<UAutoSaveComponent>();


		// Component should be valid if we saved actor, but it's good to make sure
		check(IsValid(AutoSaveComponent))

		// Notify PreLoad & load actor
		AutoSaveComponent->OnActorPreLoad.Broadcast();
		SpawnedActorData.ActorData.LoadFromMemory(Actor);


		// If requested we load all components. If it's not requested we only load AutoSaveComponent
		if(SpawnedActorData.bLoadAndSaveOtherComponents)
		{
			TInlineComponentArray<UActorComponent*> Components(Actor);

			for(UActorComponent* Component : Components)
			{
				if(SpawnedActorData.ComponentData.Contains(Component->GetName()))
					SpawnedActorData.ComponentData.Find(Component->GetName())->LoadFromMemory(Component);
			}
		}
		else
		{
			if(SpawnedActorData.ComponentData.Contains(AutoSaveComponent->GetName()))
				SpawnedActorData.ComponentData.Find(AutoSaveComponent->GetName())->LoadFromMemory(AutoSaveComponent);
		}

		// Notify PostLoad
		AutoSaveComponent->bIsLoaded = true;
		AutoSaveComponent->OnActorPostLoad.Broadcast();
	}
}


// Save all cheap AutoSaveActors
void UAutoSaveGame::AutoSaveAllAutoSaveActorsCheap(TArray<FAutoSaveActor>* AutoSaveActorArrayCheap, TArray<FAutoSaveActor>* SavedSpawnedActorsAutoSaveCheap)
{
	// AutoSaveActors
	for(const FAutoSaveActor& AutoSaveActor : *AutoSaveActorArrayCheap)
	{
		// Notify PreSave & save actor
		AutoSaveActor.AutoSaveComponent->OnActorPreSave.Broadcast();
		SaveObjectInternal(AutoSaveActor.Actor, AutoSaveActor.AutoSaveComponent->MapKey, &AutoSavedObjects);

		
		// If requested we save all components. If it's not requested we only save AutoSaveComponent
		if(AutoSaveActor.AutoSaveComponent->bLoadAndSaveOtherComponents)
		{
			TInlineComponentArray<UActorComponent*> Components(AutoSaveActor.Actor);

			for(UActorComponent* Component : Components)
			{
				SaveObjectInternal(Component, AutoSaveActor.AutoSaveComponent->MapKey + DOT + Component->GetName(), &AutoSavedObjects);
			}
		}
		else
		{
			SaveObjectInternal(AutoSaveActor.AutoSaveComponent, AutoSaveActor.AutoSaveComponent->MapKey + DOT + AutoSaveActor.AutoSaveComponent->GetName(), &AutoSavedObjects);
		}

		// Notify PostSave
		AutoSaveActor.AutoSaveComponent->bIsSaved = true;
		AutoSaveActor.AutoSaveComponent->OnActorPostSave.Broadcast();
	}





	// SpawnedAutoSaveActors
	for(const FAutoSaveActor& SpawnedAutoSaveActor : *SavedSpawnedActorsAutoSaveCheap)
	{
		FSpawnedActorData SpawnedActorData;
		SpawnedActorData.ActorClass = SpawnedAutoSaveActor.Actor->GetClass();
		SpawnedActorData.ActorTransform = SpawnedAutoSaveActor.Actor->GetTransform();
		SpawnedActorData.bLoadAndSaveOtherComponents = SpawnedAutoSaveActor.AutoSaveComponent->bLoadAndSaveOtherComponents;
		SpawnedActorData.AutoLoadGroup = SpawnedAutoSaveActor.AutoSaveComponent->AutoLoadGroup;

		// Notify PreSave & save actor
		SpawnedAutoSaveActor.AutoSaveComponent->OnActorPreSave.Broadcast();
		SpawnedActorData.ActorData = FObjectSaveRecord::SaveToMemory(SpawnedAutoSaveActor.Actor);

		// If requested we save all components. If it's not requested we only save AutoSaveComponent
		if(SpawnedActorData.bLoadAndSaveOtherComponents)
		{
			TInlineComponentArray<UActorComponent*> Components(SpawnedAutoSaveActor.Actor);

			for(UActorComponent* Component : Components)
			{
				SpawnedActorData.ComponentData.Add(Component->GetName(), FObjectSaveRecord::SaveToMemory(Component));
			}
		}
		else
		{
			SpawnedActorData.ComponentData.Add(SpawnedAutoSaveActor.AutoSaveComponent->GetName(), FObjectSaveRecord::SaveToMemory(SpawnedAutoSaveActor.AutoSaveComponent));
		}

		// Notify PostSave
		SpawnedAutoSaveActor.AutoSaveComponent->bIsSaved = true;
		SpawnedAutoSaveActor.AutoSaveComponent->OnActorPostSave.Broadcast();

		// Save data to proper array based on ManualLoad flag
		if(!SpawnedAutoSaveActor.AutoSaveComponent->bManualLoad)
		{
			SavedSpawnedActorsAutoLoad.Add(SpawnedActorData);
			continue;
		}

		SavedSpawnedActorsManualLoad.Add(SpawnedAutoSaveActor.AutoSaveComponent->ManualLoadKey, SpawnedActorData);
	}
}





// Load all expensive AutoSaveActors
void UAutoSaveGame::AutoLoadAllAutoSaveActorsExpensive(TArray<FAutoSaveActor>* AutoSaveActorArrayExpensive, TArray<FSpawnedActorData>* SavedSpawnedActorsAutoLoadExpensive, UWorld* WorldREF)
{
	TArray<FAutoSaveActor> Actors;


	// AutoSaveActors
	for(const FAutoSaveActor& AutoSaveActor : *AutoSaveActorArrayExpensive)
	{
		// Notify PreLoad
		AutoSaveActor.AutoSaveComponent->OnActorPreLoad.Broadcast();
	}


	// SpawnedAutoSaveActors
	for(FSpawnedActorData& SpawnedActorData : *SavedSpawnedActorsAutoLoadExpensive)
	{
		// Spawn actor
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		AActor* Actor = WorldREF->SpawnActor<AActor>(SpawnedActorData.ActorClass, SpawnedActorData.ActorTransform, Params);



		// Get AutoSaveComponent
		UAutoSaveComponent* AutoSaveComponent = Actor->FindComponentByClass<UAutoSaveComponent>();


		// Component should be valid if we saved actor, but it's good to make sure
		check(IsValid(AutoSaveComponent))
		Actors.Add(FAutoSaveActor(Actor, AutoSaveComponent));

		// Notify PreLoad
		AutoSaveComponent->OnActorPreLoad.Broadcast();
	}





	// AutoSaveActors
	for(const FAutoSaveActor& AutoSaveActor : *AutoSaveActorArrayExpensive)
	{
		LoadObjectInternal(AutoSaveActor.Actor, AutoSaveActor.AutoSaveComponent->MapKey, &AutoSavedObjects);

		
		// If requested we load all components. If it's not requested we only load AutoSaveComponent
		if(AutoSaveActor.AutoSaveComponent->bLoadAndSaveOtherComponents)
		{
			TInlineComponentArray<UActorComponent*> Components(AutoSaveActor.Actor);

			for(UActorComponent* Component : Components)
			{
				LoadObjectInternal(Component, AutoSaveActor.AutoSaveComponent->MapKey + DOT + Component->GetName(), &AutoSavedObjects);
			}
		}
		else
		{
			LoadObjectInternal(AutoSaveActor.AutoSaveComponent, AutoSaveActor.AutoSaveComponent->MapKey + DOT + AutoSaveActor.AutoSaveComponent->GetName(), &AutoSavedObjects);
		}
	}


	// SpawnedAutoSaveActors
	for(int i = 0; i < SavedSpawnedActorsAutoLoadExpensive->Num(); i++)
	{
		(*SavedSpawnedActorsAutoLoadExpensive)[i].ActorData.LoadFromMemory(Actors[i].Actor);


		// If requested we load all components. If it's not requested we only load AutoSaveComponent
		if((*SavedSpawnedActorsAutoLoadExpensive)[i].bLoadAndSaveOtherComponents)
		{
			TInlineComponentArray<UActorComponent*> Components(Actors[i].Actor);

			for(UActorComponent* Component : Components)
			{
				if((*SavedSpawnedActorsAutoLoadExpensive)[i].ComponentData.Contains(Component->GetName()))
					(*SavedSpawnedActorsAutoLoadExpensive)[i].ComponentData.Find(Component->GetName())->LoadFromMemory(Component);
			}
		}
		else
		{
			if((*SavedSpawnedActorsAutoLoadExpensive)[i].ComponentData.Contains(Actors[i].AutoSaveComponent->GetName()))
				(*SavedSpawnedActorsAutoLoadExpensive)[i].ComponentData.Find(Actors[i].AutoSaveComponent->GetName())->LoadFromMemory(Actors[i].AutoSaveComponent);
		}
	}





	// AutoSaveActors
	for(const FAutoSaveActor& AutoSaveActor : *AutoSaveActorArrayExpensive)
	{
		// Notify PostLoad
		AutoSaveActor.AutoSaveComponent->bIsLoaded = true;
		AutoSaveActor.AutoSaveComponent->OnActorPostLoad.Broadcast();
	}
	

	// SpawnedAutoSaveActors
	for(int i = 0; i < SavedSpawnedActorsAutoLoadExpensive->Num(); i++)
	{
		// Notify PostLoad
		Actors[i].AutoSaveComponent->bIsLoaded = true;
		Actors[i].AutoSaveComponent->OnActorPostLoad.Broadcast();
	}
}


// Save all expensive AutoSaveActors
void UAutoSaveGame::AutoSaveAllAutoSaveActorsExpensive(TArray<FAutoSaveActor>* AutoSaveActorArrayExpensive, TArray<FAutoSaveActor>* SavedSpawnedActorsAutoSaveExpensive)
{
	// AutoSaveActors
	for(const FAutoSaveActor& AutoSaveActor : *AutoSaveActorArrayExpensive)
	{
		// Notify PreSave
		AutoSaveActor.AutoSaveComponent->OnActorPreSave.Broadcast();
	}


	// SpawnedAutoSaveActors
	for(const FAutoSaveActor& SpawnedAutoSaveActor : *SavedSpawnedActorsAutoSaveExpensive)
	{
		// Notify PreSave
		SpawnedAutoSaveActor.AutoSaveComponent->OnActorPreSave.Broadcast();
	}





	// AutoSaveActors
	for(const FAutoSaveActor& AutoSaveActor : *AutoSaveActorArrayExpensive)
	{
		// Save actor
		SaveObjectInternal(AutoSaveActor.Actor, AutoSaveActor.AutoSaveComponent->MapKey, &AutoSavedObjects);

		
		// If requested we save all components. If it's not requested we only save AutoSaveComponent
		if(AutoSaveActor.AutoSaveComponent->bLoadAndSaveOtherComponents)
		{
			TInlineComponentArray<UActorComponent*> Components(AutoSaveActor.Actor);

			for(UActorComponent* Component : Components)
			{
				SaveObjectInternal(Component, AutoSaveActor.AutoSaveComponent->MapKey + DOT + Component->GetName(), &AutoSavedObjects);
			}
		}
		else
		{
			SaveObjectInternal(AutoSaveActor.AutoSaveComponent, AutoSaveActor.AutoSaveComponent->MapKey + DOT + AutoSaveActor.AutoSaveComponent->GetName(), &AutoSavedObjects);
		}
	}


	// SpawnedAutoSaveActors
	for(const FAutoSaveActor& SpawnedAutoSaveActor : *SavedSpawnedActorsAutoSaveExpensive)
	{
		// Save actor
		SaveObjectInternal(SpawnedAutoSaveActor.Actor, SpawnedAutoSaveActor.AutoSaveComponent->MapKey, &AutoSavedObjects);

		
		// If requested we save all components. If it's not requested we only save AutoSaveComponent
		if(SpawnedAutoSaveActor.AutoSaveComponent->bLoadAndSaveOtherComponents)
		{
			TInlineComponentArray<UActorComponent*> Components(SpawnedAutoSaveActor.Actor);

			for(UActorComponent* Component : Components)
			{
				SaveObjectInternal(Component, SpawnedAutoSaveActor.AutoSaveComponent->MapKey + DOT + Component->GetName(), &AutoSavedObjects);
			}
		}
		else
		{
			SaveObjectInternal(SpawnedAutoSaveActor.AutoSaveComponent, SpawnedAutoSaveActor.AutoSaveComponent->MapKey + DOT + SpawnedAutoSaveActor.AutoSaveComponent->GetName(), &AutoSavedObjects);
		}
	}





	// AutoSaveActors
	for(const FAutoSaveActor& AutoSaveActor : *AutoSaveActorArrayExpensive)
	{
		// Notify PostSave
		AutoSaveActor.AutoSaveComponent->bIsSaved = true;
		AutoSaveActor.AutoSaveComponent->OnActorPostSave.Broadcast();
	}


	// SpawnedAutoSaveActors
	for(const FAutoSaveActor& SpawnedAutoSaveActor : *SavedSpawnedActorsAutoSaveExpensive)
	{
		// Notify PostSave
		SpawnedAutoSaveActor.AutoSaveComponent->bIsSaved = true;
		SpawnedAutoSaveActor.AutoSaveComponent->OnActorPostSave.Broadcast();

		
		FSpawnedActorData SpawnedActorData;
		SpawnedActorData.ActorClass = SpawnedAutoSaveActor.Actor->GetClass();
		SpawnedActorData.ActorTransform = SpawnedAutoSaveActor.Actor->GetTransform();
		SpawnedActorData.bLoadAndSaveOtherComponents = SpawnedAutoSaveActor.AutoSaveComponent->bLoadAndSaveOtherComponents;
		SpawnedActorData.AutoLoadGroup = SpawnedAutoSaveActor.AutoSaveComponent->AutoLoadGroup;

		
		// Save data to proper array based on ManualLoad flag
		if(!SpawnedAutoSaveActor.AutoSaveComponent->bManualLoad)
		{
			SavedSpawnedActorsAutoLoad.Add(SpawnedActorData);
			continue;
		}

		SavedSpawnedActorsManualLoad.Add(SpawnedAutoSaveActor.AutoSaveComponent->ManualLoadKey, SpawnedActorData);
	}
}



// Manual load SpawnedAutoSaveActor
AActor* UAutoSaveGame::ManualLoadSpawnedAutoSaveActor(UWorld* WorldREF, const FString ManualLoadKey, bool& bSuccess)
{
	// if world is not valid or data not found, we shouldn't try to load it
	if(!IsValid(WorldREF) || !SavedSpawnedActorsManualLoad.Contains(ManualLoadKey))
	{
		bSuccess = false;
		return nullptr;
	}


	// Get data
	FSpawnedActorData SpawnedActorData = *SavedSpawnedActorsManualLoad.Find(ManualLoadKey);


	// Spawn actor
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AActor* Actor = WorldREF->SpawnActor<AActor>(SpawnedActorData.ActorClass, SpawnedActorData.ActorTransform, Params);



	// Get component
	UAutoSaveComponent* AutoSaveComponent = Actor->FindComponentByClass<UAutoSaveComponent>();


	// Component should be valid if we saved actor, but it's good to make sure
	check(AutoSaveComponent)
		

	// Notify PreLoad & load actor
	AutoSaveComponent->OnActorPreLoad.Broadcast();
	SpawnedActorData.ActorData.LoadFromMemory(Actor);


	// If requested we load all components. If it's not requested we only load AutoSaveComponent
	if(SpawnedActorData.bLoadAndSaveOtherComponents)
	{
		TInlineComponentArray<UActorComponent*> Components(Actor);

		for(UActorComponent* Component : Components)
		{
			if(SpawnedActorData.ComponentData.Contains(Component->GetName()))
				SpawnedActorData.ComponentData.Find(Component->GetName())->LoadFromMemory(Component);
		}
	}
	else
	{
		if(SpawnedActorData.ComponentData.Contains(AutoSaveComponent->GetName()))
			SpawnedActorData.ComponentData.Find(AutoSaveComponent->GetName())->LoadFromMemory(AutoSaveComponent);
	}

	// Notify PostLoad
	AutoSaveComponent->bIsLoaded = true;
	AutoSaveComponent->OnActorPostLoad.Broadcast();


	// After spawned actors, remove it from TArray. So it won't be loaded again, if it destroyed before world destroys
	SavedSpawnedActorsManualLoad.Remove(ManualLoadKey);
	
	bSuccess = true;
	return Actor;
}