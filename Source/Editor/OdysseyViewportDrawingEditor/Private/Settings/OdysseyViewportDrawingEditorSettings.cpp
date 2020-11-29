// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "OdysseyViewportDrawingEditorSettings.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/Package.h"

UOdysseyViewportDrawingEditorSettings::UOdysseyViewportDrawingEditorSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UOdysseyViewportDrawingEditorSettings* UOdysseyViewportDrawingEditorSettings::Get()
{
	static UOdysseyViewportDrawingEditorSettings* settings = nullptr;
	if (!settings)
	{
		settings = DuplicateObject<UOdysseyViewportDrawingEditorSettings>(GetMutableDefault<UOdysseyViewportDrawingEditorSettings>(), GetTransientPackage());
		settings->AddToRoot();
	}

	return settings;
}

