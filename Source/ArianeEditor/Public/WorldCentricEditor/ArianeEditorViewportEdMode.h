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

        virtual void Initialize() override;
        virtual void Enter() override;
        virtual void Exit() override;

        /** FGCObject interface */
        virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
        virtual FString GetReferencerName() const override
        {
            return "ArianeEditorViewportEdMode";
        }

        // FEdMode interface.
        virtual bool UsesToolkits() const override { return true; }

        virtual void Render(const FSceneView* View,FViewport* Viewport,FPrimitiveDrawInterface* PDI) override;
        virtual void DrawHUD(FEditorViewportClient* ViewportClient,FViewport* Viewport,const FSceneView* View,FCanvas* Canvas) override;
        virtual bool Select(AActor* InActor, bool bInSelected) override;

        virtual bool MouseMove( FEditorViewportClient* iViewportClient
                              , FViewport* iViewport
                              , int32 iMouseX
                              , int32 iMouseY ) override;
        virtual bool InputKey( FEditorViewportClient* iViewportClient
                             , FViewport* iViewport
                             , FKey iKey
                             , EInputEvent iEvent) override;
        virtual bool CapturedMouseMove( FEditorViewportClient* iViewportClient
                                      , FViewport* iViewport
                                      , int32 iMouseX
                                      , int32 iMouseY ) override;
        virtual bool HandleClick( FEditorViewportClient* InViewportClient
                                , HHitProxy* HitProxy
                                , const FViewportClick& Click ) override;

        virtual bool AllowWidgetMove() override { return false; }
        virtual bool ShouldDrawWidget() const override { return false; }
        virtual bool UsesTransformWidget() const override { return false; }

        bool IsEditingEnabled() const;

        virtual bool GetCursor(EMouseCursor::Type& OutCursor) const override;

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
        bool CapturedMouseMove_Private(FEditorViewportClient* iViewportClient
                             , FViewport* iViewport
                             , const FArianePointerState& State );
        bool HandleClick_Private( FEditorViewportClient* InViewportClient
                                , HHitProxy* HitProxy
                                , const FViewportClick& Click
                                , const FArianePointerState& State );

        FArianePointerState StylusStateToPointerState( const FStylusState& StylusState );
        void FlushStylusInput();

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
