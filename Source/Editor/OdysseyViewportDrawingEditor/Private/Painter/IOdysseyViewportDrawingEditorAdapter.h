// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyBrushAssetBase.h"
#include "IStylusState.h"
#include <chrono>
#include "Input/OdysseyRay.h"

#include <ULIS>

class UOdysseyPainterEditorRasterDrawingTool;
class FOdysseyViewportDrawingEditorExtension;
class UOdysseyPainterEditorTool;

/** Painting adapter for the painter. Describes the method of painting in the viewport*/
class IOdysseyViewportDrawingEditorAdapter
    : public IStylusMessageHandler
    
{
public:
    enum class eState
    {
        kIdle, //Idle, but preparations are not made yet
        kIdleReady, //Idle, but we're now ready to paint
        kDrawing,
    };

public:
    /** destructor */
    virtual ~IOdysseyViewportDrawingEditorAdapter();

    /** constructor */
    IOdysseyViewportDrawingEditorAdapter(FOdysseyViewportDrawingEditorExtension* iExtension);

public:
    virtual void Initialize();
    virtual void Finalize();

    virtual void SetTexture(UTexture2D* iTexture);
    UTexture2D* GetTexture() const;

    /** Painting Methods **/
    virtual void StartPainting();
    virtual void Paint();
    virtual void FinishPainting();

    virtual void RenderInteractorWidget(const FSceneView* iView, FViewport* iViewport, FPrimitiveDrawInterface* iPDI) = 0;

    FVector2D ViewportCoordinatesToTextureCoordinates( FVector2D iPositionInViewport, FEditorViewportClient* iViewportClient);

public:
    bool IsReadyToDraw() const;

public:
    /** Viewport Client methods */
    virtual bool MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y);

    virtual bool InputKey(FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent);
    virtual bool InputKeyWithStrokeRay(const FOdysseyRay& iRay, FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent);

    virtual bool CapturedMouseMove(FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iMouseX, int32 iMouseY);
    virtual bool CapturedMouseMoveWithStrokeRay(const FOdysseyRay& iRay, FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iMouseX, int32 iMouseY);

private:
    /** IStylusMessageHandler Overrides */
    virtual void OnStylusStateChanged(const TWeakPtr<SWidget> iWidget, const FStylusState& iState, int32 iIndex) override;

protected: 
    virtual ::ULIS::FEvent StampOverride(UOdysseyBrushAssetBase::FStampParams iStampParams) = 0;
    void OnSelectedToolChanged();
    virtual void SetTool(UOdysseyPainterEditorTool* iTool);
    UOdysseyPainterEditorRasterDrawingTool* GetDrawingTool();

    virtual void UnbindStampBrushInstance(UOdysseyBrushAssetBase* iUnbindBrush);
    virtual void BindStampBrushInstance(UOdysseyBrushAssetBase* iBindBrush);

protected:
    UTexture2D* mTexture;

    /** The editor we need help to paint with*/
    FOdysseyViewportDrawingEditorExtension* mExtension;

    /** The list of currently pressed keys */
    TArray<FKey> mKeysPressed;

    /** The state of the adapter: Are we currently drawing ? Are we idle ? Are the preparations to draw ready ?*/
    eState mState;

    /** The main information about the last ray. Useful for the begin stroke of the paint engine */
    FOdysseyRay mLastStrokeRay;

    /** The main information about the current ray. We can use it to draw at the right place on the mesh */
    FOdysseyRay mCurrentStrokeRay;

    /** The last known viewport we treated stylus input from. We need it because our stylus event handler doesn't know it, but need it to draw */
    FViewport* mLastKnownViewport;

    /** Patch (kinda): Stylus state and time of last event, so that we can have a little control for differentiating mouse and stylus events */
    std::chrono::steady_clock::time_point   mStylusLastEventTime;
    bool mIsCapturedByStylus;

    /** Current or previous selected tool which still has delegates on this adapter, we keep it here so that we can handle said delegates */
    UOdysseyPainterEditorTool* mTool;
    //UOdysseyPainterEditorRasterDrawingTool* mDrawingTool;
};
