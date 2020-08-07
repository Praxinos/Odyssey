// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"
#include "Toolkits/BaseToolkit.h"

class FOdysseyViewportDrawingEditorEdModeToolkit : public FModeToolkit
{
public:
	FOdysseyViewportDrawingEditorEdModeToolkit(class FOdysseyViewportDrawingEditorEdMode* iOwningMode);
	
	/** Initializes the geometry mode toolkit */
	virtual void Init(const TSharedPtr< class IToolkitHost >& iInitToolkitHost) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override;

private:
	/** Owning editor mode */
	class FOdysseyViewportDrawingEditorEdMode* mMeshPaintEdMode;
};
