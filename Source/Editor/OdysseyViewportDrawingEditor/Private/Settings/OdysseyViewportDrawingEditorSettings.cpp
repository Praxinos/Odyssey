// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

