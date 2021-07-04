// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyModeToolkit.h"

class FOdysseyViewportDrawingEditor;

class FOdysseyViewportDrawingEditorToolkit : public FOdysseyModeToolkit
{
public:
	FOdysseyViewportDrawingEditorToolkit(TSharedPtr<FOdysseyViewportDrawingEditor> iEditor, class FEdMode* iEditorMode);

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;

    //If we fill the array with names, a mode toolbar will pop in our edMode
    virtual void GetToolPaletteNames( TArray<FName>& ioPaletteNames ) const override;
    virtual void BuildToolPalette( FName iPalette, class FToolBarBuilder& ioToolbarBuilder ) override;
};
