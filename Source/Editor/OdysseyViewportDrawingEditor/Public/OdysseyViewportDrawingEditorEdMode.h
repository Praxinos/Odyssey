// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IMeshPaintMode.h"

class FModeToolkit;

/**
 * Odyssey paint on viewport editor mode
 */
class FOdysseyViewportDrawingEditorEdMode : public IMeshPaintEdMode
{
public:
	const static FEditorModeID EM_OdysseyViewportDrawingEditorEdModeId;
public:
	/** Constructor */
	FOdysseyViewportDrawingEditorEdMode() {}

	/** Destructor */
	virtual ~FOdysseyViewportDrawingEditorEdMode() {}
	virtual void Initialize() override;
	virtual TSharedPtr< FModeToolkit> GetToolkit() override;

	// IMeshPaintEdMode interface.
	virtual bool InputKey(FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent) override;

	bool IsEditingEnabled() const;
};