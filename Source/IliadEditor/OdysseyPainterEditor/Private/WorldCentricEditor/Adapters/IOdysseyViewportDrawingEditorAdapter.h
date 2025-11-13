// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyBrushAssetBase.h"
#include "IStylusState.h"
#include <chrono>
#include "Input/OdysseyRay.h"
#include "OdysseyHUDElement.h"

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
        kCapturedByEditor
    };

public:
    /** destructor */
    virtual ~IOdysseyViewportDrawingEditorAdapter();

    /** constructor */
    IOdysseyViewportDrawingEditorAdapter(FOdysseyViewportDrawingEditorExtension* iExtension);

public:
    virtual void Initialize();
    virtual void Finalize();

    virtual void SetTexture(UTexture* iTexture);
    UTexture* GetTexture() const;

    /** Painting Methods **/
    virtual void StartPainting();
    virtual void Paint();
    virtual void FinishPainting();

    virtual void RenderInteractorWidget(const FSceneView* iView, FViewport* iViewport, FPrimitiveDrawInterface* iPDI) = 0;

    bool ViewportCoordinatesToTextureCoordinates( FVector2D iPositionInViewport, FEditorViewportClient* iViewportClient, FVector2D* oPositionInTexture);

public:
    bool IsReadyToDraw();

public:
    /** Viewport Client methods */
    virtual bool MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y);
    virtual bool InputKey(FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent);
    virtual bool CapturedMouseMove(FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iMouseX, int32 iMouseY);
    virtual bool GetCursor(EMouseCursor::Type& OutCursor) const;
    virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click);

private:
    /** IStylusMessageHandler Overrides */
    virtual void OnStylusStateChanged(const TWeakPtr<SWidget> iWidget, const TArray<FStylusState>& iStates, int32 iIndex) override;
    void StartStylusInputRecord(const FKey& iMouseButton);
    void StopStylusInputRecord();
    FOdysseyRay StylusStateToRay(const FStylusState& iState);
    void ReadStylusInput();
    void GetRayParamsFromViewportPosition(FEditorViewportClient* iViewportClient, float iX, float iY, FVector* oOrigin, FVector* oDirection);

protected:
    virtual ::ULIS::FEvent StampOverride(UOdysseyBrushAssetBase::FStampParams iStampParams) = 0;
    void OnCurrentToolChanged();
    virtual void SetTool(UOdysseyPainterEditorTool* iTool);
    UOdysseyPainterEditorRasterDrawingTool* GetDrawingTool();

    virtual void UnbindStampBrushInstance(UOdysseyBrushAssetBase* iUnbindBrush);
    virtual void BindStampBrushInstance(UOdysseyBrushAssetBase* iBindBrush);

private:
    //New API to manage drawing events
    void MouseDown(const FOdysseyRay& iRay, const FKey& iMouseButton);
    void MouseUp(const FOdysseyRay& iRay, const FKey& iMouseButton);
    void MouseDrag(const FOdysseyRay& iRay);
    bool KeyDown(FKey iKey);
    bool KeyUp(FKey iKey);
    bool ShouldEditorCaptureMouse() const;

    TSharedPtr<FOdysseyHUDElement> GetHUDElement(FViewport* iViewport, int32 iX, int32 iY);

protected:
    UTexture* mTexture;

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

    /** Current or previous selected tool which still has delegates on this adapter, we keep it here so that we can handle said delegates */
    TStrongObjectPtr<UOdysseyPainterEditorTool> mTool;

    /** Contains the MouseButton considered as the one currently used*/
    FKey mMouseButton;

    /** Contains all the last styluses states */
    TArray<FStylusState> mStylusStates;

    /** Are we using the stylus or not */
    bool mIsRecordingStylus = false;

    /** Indicates the next stylus state to read in mStylusStates */
    int mLastStylusEventIndex = 0;

    /** Indicates if the stylus is considered as touching the tablet or not */
    bool mStylusIsDown = false;
    FKey mStylusButton;

    bool mOverrideMouseCursor = false;
    EMouseCursor::Type mMouseCursor = EMouseCursor::Default;
    bool mIsPickingColor = false;

    FOdysseyPoint mCurrentHUDPoint;
    FOdysseyPoint mLastHUDPoint;
    FVector2D     mHUDMouseDownReference;
    TSharedPtr<FOdysseyHUDElement> mCurrentHUDElement;
    TSharedPtr<FOdysseyHUDElement> mHoveredHUDElement;
};
