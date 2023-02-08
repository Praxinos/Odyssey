// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "IOdysseyViewportDrawingEditorAdapter.h"

/** Painting adapter for the painter. Describes the method of texture based painting in the viewport*/
class FOdysseyViewportDrawingEditorTextureBasedAdapter : public IOdysseyViewportDrawingEditorAdapter
{
public:
    /** destructor */
    ~FOdysseyViewportDrawingEditorTextureBasedAdapter();

    /** constructor */
    FOdysseyViewportDrawingEditorTextureBasedAdapter(TSharedPtr<FOdysseyViewportDrawingEditor> iEditor);

public:
    virtual void PrepareAdapterForPainting() override;
    virtual void StartPainting() override;
    virtual void Paint() override;
    virtual void FinishPainting() override;

    virtual void Tick(float iDelta) override;

    virtual void RenderInteractorWidget(const FSceneView* iView, FViewport* iViewport, FPrimitiveDrawInterface* iPDI) override;

private:
    void OnToolChange(UOdysseyPainterEditorTool* iNewTool);
    virtual ::ULIS::FEvent StampOverride(UOdysseyBrushAssetBase::FStampParams iStampParams) override;

};
