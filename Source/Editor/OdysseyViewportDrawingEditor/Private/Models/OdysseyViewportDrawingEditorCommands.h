// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "OdysseyStyleSet.h"

class FOdysseyViewportDrawingEditorCommands : public TCommands<FOdysseyViewportDrawingEditorCommands>
{
public:
	FOdysseyViewportDrawingEditorCommands() : TCommands<FOdysseyViewportDrawingEditorCommands> ( "OdysseyViewportDrawingEditMode", NSLOCTEXT("Contexts", "OdysseyViewportDrawingEditMode", "Odyssey Viewport Drawing Edit Mode"), NAME_None, FOdysseyStyle::GetStyleSetName()) {}

	/**
	* Initialize commands
	*/
	virtual void RegisterCommands() override;

public:
    TSharedPtr<FUICommandInfo> NextTexture;
    TSharedPtr<FUICommandInfo> PreviousTexture;
	TSharedPtr<FUICommandInfo> SaveTexturePaint;

	/** Commands for the tools toolbar. */
	TSharedPtr< FUICommandInfo > SetOdysseyBrushSettingsView;
	TSharedPtr< FUICommandInfo > SetOdysseyStrokeOptionsView;
	TSharedPtr< FUICommandInfo > SetOdysseyLayerStackView;
    TSharedPtr< FUICommandInfo > SetOdysseyToolsView;


	TArray<TSharedPtr<FUICommandInfo>> Commands;
};