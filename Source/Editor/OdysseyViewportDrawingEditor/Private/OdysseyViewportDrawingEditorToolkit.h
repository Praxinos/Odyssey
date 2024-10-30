// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyModeToolkit.h"
#include "OdysseyPainterEditor.h"
#include "ViewportDrawingEditor/OdysseyViewportDrawingEditorExtension.h"

class FOdysseyViewportDrawingEditor;
class FEdMode;

class FOdysseyViewportDrawingEditorToolkit : public FOdysseyModeToolkit
{
public:
    FOdysseyViewportDrawingEditorToolkit(TSharedRef<FOdysseyPainterEditor> iEditor, FEdMode* iEdMode);

    virtual TSharedPtr<SWidget> GetInlineContent() const override;
    virtual FEdMode* GetEditorMode() const override;

    /** IToolkit interface */
    virtual FName GetToolkitFName() const override;
    virtual FText GetBaseToolkitName() const override;

    //If we fill the array with names, a mode toolbar will pop in our edMode
    virtual void GetToolPaletteNames( TArray<FName>& ioPaletteNames ) const override;
    virtual void BuildToolPalette( FName iPalette, class FToolBarBuilder& ioToolbarBuilder ) override;

    TSharedRef<FOdysseyViewportDrawingEditorExtension> GetViewportDrawingExtension() const;

private:
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> mViewportDrawingExtension;
    FEdMode* mEdMode;
};
