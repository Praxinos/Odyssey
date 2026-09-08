// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "EdMode.h"

class FModeToolkit;
class FOdysseyViewportDrawingEditor;
class FOdysseyViewportDrawingEditorToolkit;
class FOdysseyViewportDrawingEditorPainter;
class FOdysseyPainterEditor;
class FOdysseyViewportDrawingEditorExtension;

/**
 * Odyssey paint on viewport editor mode
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyViewportDrawingEditorEdMode : public FEdMode
{
public:
    const static FEditorModeID EM_OdysseyViewportDrawingEditorEdModeId;
public:
    /** Constructor */
    FOdysseyViewportDrawingEditorEdMode();

    /** Destructor */
    virtual ~FOdysseyViewportDrawingEditorEdMode();

    virtual void Initialize() override;
    virtual void Enter() override;
    virtual void Exit() override;

    // FEdMode interface.
    virtual bool UsesToolkits() const override { return true; }

    virtual void Render(const FSceneView* View,FViewport* Viewport,FPrimitiveDrawInterface* PDI) override;
    virtual void DrawHUD(FEditorViewportClient* ViewportClient,FViewport* Viewport,const FSceneView* View,FCanvas* Canvas) override;
    virtual bool Select(AActor* InActor, bool bInSelected) override;

    virtual bool MouseEnter(FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iMouseX, int32 iMouseY) override;
    virtual bool MouseLeave(FEditorViewportClient* iViewportClient, FViewport* iViewport) override;
    virtual bool MouseMove(FEditorViewportClient* iViewportClient,FViewport* iViewport,int32 iMouseX,int32 iMouseY) override;
    virtual bool InputKey(FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent) override;
    virtual bool CapturedMouseMove(FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iMouseX, int32 iMouseY) override;
    virtual bool HandleClick(FEditorViewportClient* iViewportClient, HHitProxy* iHitProxy, const FViewportClick& iClick) override;

    virtual bool AllowWidgetMove() override { return false; }
    virtual bool ShouldDrawWidget() const override { return false; }
    virtual bool UsesTransformWidget() const override { return false; }

    bool IsEditingEnabled() const;

    virtual bool GetCursor(EMouseCursor::Type& OutCursor) const override;

    FOdysseyPainterEditor* GetEditor() const;
    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> GetViewportDrawingEditorToolkit() const;

private:
    void OnEditorClose();

private:
    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> mViewportDrawingEditorToolkit;
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> mViewportDrawingEditorExtension;

private:
    // Telemetry
    FDateTime SessionStartTime;
};
