// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
	virtual void Enter() override;
	virtual void Exit() override;
	virtual TSharedPtr< FModeToolkit> GetToolkit() override;

	// IMeshPaintEdMode interface.
	virtual bool InputKey(FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent) override;
    virtual void Render(const FSceneView* View,FViewport* Viewport,FPrimitiveDrawInterface* PDI) override;
    virtual bool MouseMove(FEditorViewportClient* ViewportClient,FViewport* Viewport,int32 x,int32 y);
    virtual bool ProcessCapturedMouseMoves(FEditorViewportClient* InViewportClient,FViewport* InViewport,const TArrayView<FIntPoint>& CapturedMouseMoves) override;

	bool IsEditingEnabled() const;
};