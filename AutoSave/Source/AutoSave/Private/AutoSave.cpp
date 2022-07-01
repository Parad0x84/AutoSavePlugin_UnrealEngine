// Copyright Epic Games, Inc. All Rights Reserved.

#include "AutoSave.h"

#define LOCTEXT_NAMESPACE "FAutoSaveModule"


void FAutoSaveModule::StartupModule()
{
	IModuleInterface::StartupModule();
}

void FAutoSaveModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAutoSaveModule, AutoSave)
