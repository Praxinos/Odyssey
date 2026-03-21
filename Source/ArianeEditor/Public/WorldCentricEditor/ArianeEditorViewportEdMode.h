// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "EdMode.h"
#include "IStylusState.h"
// Ariane headers
#include "ArianePointerState.h"

class FModeToolkit;
class FArianeEditorViewportToolkit;


/**
 * Odyssey paint on viewport editor mode
 */
class ARIANEEDITOR_API FArianeEditorViewportEdMode : public FEdMode, public IStylusMessageHandler
{
public:
    const static FEditorModeID EM_ArianeEditorViewportEdModeId;
public:
    /** Constructor */
    FArianeEditorViewportEdMode();

    /** Destructor */
    virtual ~FArianeEditorViewportEdMode();

    /** Overriden from FGCObject */
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    /** Overriden from FGCObject */
    virtual FString GetReferencerName() const override
    {
        return "ArianeEditorViewportEdMode";
    }
    /** Overriden from FEdMode. */
    virtual bool UsesToolkits() const override { return true; }

    /** Overriden from FEdMode. */
    virtual void Initialize() override;

    /** Overriden from FEdMode. */
    virtual void Enter() override;

    /** Overriden from FEdMode. */
    virtual void Exit() override;

    /** Overriden from FEdMode. */
    virtual void Render(const FSceneView* View,FViewport* Viewport,FPrimitiveDrawInterface* PDI) override;

    /** Overriden from FEdMode. */
    virtual void DrawHUD(FEditorViewportClient* ViewportClient,FViewport* Viewport,const FSceneView* View,FCanvas* Canvas) override;

    /** Overriden from FEdMode. */
    virtual bool Select(AActor* InActor, bool bInSelected) override;

    /** Overriden from FEdMode. */
    virtual bool MouseMove( FEditorViewportClient* iViewportClient
                          , FViewport* iViewport
                          , int32 iMouseX
                          , int32 iMouseY ) override;

    // Overriden from FEdMode
    virtual bool InputKey( FEditorViewportClient* iViewportClient
                         , FViewport* iViewport
                         , FKey iKey
                         , EInputEvent iEvent) override;

    // Override from FEdMode
    virtual bool CapturedMouseMove( FEditorViewportClient* iViewportClient
                                  , FViewport* iViewport
                                  , int32 iMouseX
                                  , int32 iMouseY ) override;

    // Overriden from FEdMode
    virtual bool HandleClick( FEditorViewportClient* InViewportClient
                            , HHitProxy* HitProxy
                            , const FViewportClick& Click ) override;

    // Overriden from FEdMode
    virtual bool AllowWidgetMove() override { return false; }

    // Overriden from FEdMode
    virtual bool ShouldDrawWidget() const override { return false; }

    // Overriden from FEdMode
    virtual bool UsesTransformWidget() const override { return false; }

    // Overriden from FEdMode
    virtual bool GetCursor(EMouseCursor::Type& OutCursor) const override;


    bool IsEditingEnabled() const;

    TSharedPtr<FArianeEditorViewportToolkit> GetArianeEditorViewportToolkit() const;

private:
    bool MouseMove_Private( FEditorViewportClient* iViewportClient
                          , FViewport* iViewport
                          , const FArianePointerState& State );
    bool InputKey_Private( FEditorViewportClient* iViewportClient
                         , FViewport* iViewport
                         , FKey iKey
                         , const FArianePointerState& State
                         , EInputEvent iEvent );
    bool CapturedMouseMove_Private( FEditorViewportClient* iViewportClient
                                  , FViewport* iViewport
                                  , const FArianePointerState& State );
    bool HandleClick_Private( FEditorViewportClient* InViewportClient
                            , HHitProxy* HitProxy
                            , const FViewportClick& Click
                            , const FArianePointerState& State );

    /** Force retrieval of the last events from the stylus input */
    void FlushStylusInput();
    void ListenStylusInput();
    void IgnoreStylusInput();

private:
    void OnEditorClose();
    // Implements IStylusMessageHandler::OnStylusStateChanged
    void OnStylusStateChanged( const TWeakPtr<SWidget> iWidget
                             , const TArray<FStylusState>& iStates
                             , int32 iIndex );

private:
    bool bStylusInUse;
/* Gary
    FArianeViewportDrawingEditorPainter* mViewportDrawingEditorPainter;
    TSharedPtr<FArianeViewportDrawingEditorExtension> mViewportDrawingEditorExtension;
*/
};
