// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "EdMode.h"

class FModeToolkit;
class FOdysseyViewportDrawingEditor;
class FOdysseyViewportDrawingEditorToolkit;
class FOdysseyViewportDrawingEditorPainter;

/**
 * Odyssey paint on viewport editor mode
 */
class ODYSSEYVIEWPORTDRAWINGEDITOR_API FOdysseyViewportDrawingEditorEdMode : public FEdMode
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

    /** FGCObject interface */
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override
    {
        return "OdysseyViewportDrawingEditorEdMode";
    }

	// FEdMode interface.
    virtual bool UsesToolkits() const override { return true; }

    virtual void Render(const FSceneView* View,FViewport* Viewport,FPrimitiveDrawInterface* PDI) override;
    virtual bool Select(AActor* InActor, bool bInSelected) override;

    virtual bool MouseMove(FEditorViewportClient* iViewportClient,FViewport* iViewport,int32 iMouseX,int32 iMouseY) override;
	virtual bool InputKey(FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent) override;
    virtual bool CapturedMouseMove(FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iMouseX, int32 iMouseY) override;
    
    virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;

    virtual bool AllowWidgetMove() override { return false; }
    virtual bool ShouldDrawWidget() const override { return false; }
    virtual bool UsesTransformWidget() const override { return false; }

	bool IsEditingEnabled() const;

private:
    void CleanupTabsToolbar(const TSharedRef<SWindow>& Window);
    void OnResetViewMode();

private:
	TSharedPtr<FOdysseyViewportDrawingEditor> mEditor;
	//TSharedPtr<FOdysseyViewportDrawingEditorToolkit> mToolkit;
	FOdysseyViewportDrawingEditorPainter* mViewportDrawingEditorPainter;
};