// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "IStylusState.h"
#include "RawIndexBuffer.h"
#include "Rendering/StaticMeshVertexBuffer.h"
#include "UnrealClient.h"
#include "UObject/GCObject.h"
#include "ViewportClient.h"

#include "Input/OdysseyPoint.h"
#include "OdysseyEventState.h"

#include <chrono>
#include <ULIS>
#include "StylusInputHandler.h"

class FCanvas;
class UTexture2D;

class FOdysseyMeshSelector;
class FOdysseyPainterEditorData;
class IStylusInputInterfaceInternal;
class SOdysseyViewport;
class FOdysseyPainterEditor;
class FOdysseyHUDElement;

/////////////////////////////////////////////////////
// FOdysseyPainterEditorViewportClient
class FOdysseyPainterEditorViewportClient
    : public FViewportClient
    , public FGCObject
    , public FOdysseyStylusInputHandler
    , public FTickableEditorObject
{
public:
    DECLARE_DELEGATE_TwoParams(FOnPickColor, eOdysseyEventState::Type, const FVector2D&)
    DECLARE_DELEGATE_RetVal_TwoParams(bool, FOnMouseDown, const FOdysseyPoint&, const FKey&)
    DECLARE_DELEGATE_RetVal_TwoParams(bool, FOnMouseUp, const FOdysseyPoint&, const FKey&)
    DECLARE_DELEGATE_RetVal_TwoParams(bool, FOnMouseClick, const FOdysseyPoint&, const FKey&)
    DECLARE_DELEGATE_RetVal_TwoParams(bool, FOnMouseDoubleClick, const FOdysseyPoint&, const FKey&)
    DECLARE_DELEGATE_OneParam(FOnMouseHover, const FOdysseyPoint&)
    DECLARE_DELEGATE_OneParam(FOnMouseDrag, const FOdysseyPoint&)
    DECLARE_DELEGATE_RetVal_OneParam(bool, FOnKeyDown, const FKey&)
    DECLARE_DELEGATE_RetVal_OneParam(bool, FOnKeyUp, const FKey&)

public:
    enum class eState
    {
        kIdle,
        kRotate,
        kPanZoom,
        kPan,
        kZoom,
    };

public:
    enum class eStylusEventFence
    {
        kNone,
        kStylusUp,
        kStylusDown,
    };

public:
    // Construction / Destruction
    FOdysseyPainterEditorViewportClient(FOdysseyPainterEditor* iEditor, TWeakPtr< SOdysseyViewport > iTextureEditorViewport, FOdysseyMeshSelector* iMeshSelector);
    ~FOdysseyPainterEditorViewportClient();

public:
    // FViewportClient API
    virtual void Draw( FViewport* iViewport, FCanvas* ioCanvas ) override;

    virtual bool InputKey( const FInputKeyEventArgs& iEventArgs ) override;
    virtual void CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY ) override;
    virtual void MouseEnter( FViewport* iViewport, int32 iX, int32 iY ) override;
    virtual void MouseLeave( FViewport* iViewport ) override;
    virtual void MouseMove(FViewport* iViewport, int32 iX, int32 iY) override;

    virtual EMouseCursor::Type                  GetCursor( FViewport* iViewport, int32 iX, int32 iY ) override;
    virtual TOptional< TSharedRef< SWidget > >  MapCursor( FViewport* iViewport, const FCursorReply& iCursorReply ) override;

    virtual void OnPacket(const UE::StylusInput::FStylusInputPacket& Packet, UE::StylusInput::IStylusInputInstance* Instance) override;

    void StartStylusInputRecord();
    void StopStylusInputRecord();
    FOdysseyPoint StylusPacketToPoint(const UE::StylusInput::FStylusInputPacket& iPacket);

    void ReadStylusInput(eStylusEventFence iUntilEventType = eStylusEventFence::kNone);

    virtual EMouseCaptureMode GetMouseCaptureMode() const override;

public:
    // FGCObject API
    virtual void AddReferencedObjects( FReferenceCollector& ioCollector ) override;
    virtual FString GetReferencerName() const override;

public:
    // FTickableEditorObject
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(OdysseyPainterEditorViewportClient, STATGROUP_Tickables); }

public:
    // Public API
    FOnMouseDown&   OnMouseDown()   { return mOnMouseDown; }
    FOnMouseClick& OnMouseClick() { return mOnMouseClick; }
    FOnMouseDoubleClick&   OnMouseDoubleClick()   { return mOnMouseDoubleClick; }
    FOnMouseUp&     OnMouseUp()     { return mOnMouseUp; }
    FOnMouseHover&  OnMouseHover()  { return mOnMouseHover; }
    FOnMouseDrag&   OnMouseDrag()   { return mOnMouseDrag; }
    FOnKeyDown&     OnKeyDown()     { return mOnKeyDown; }
    FOnKeyUp&       OnKeyUp()       { return mOnKeyUp; }

private:
    // Private API
    void        CreateCheckerboardTexture();
    void        DestroyCheckerboardTexture();
    void        ZoomInInViewport( const FVector2D& iPositionInViewport );
    void        ZoomOutInViewport( const FVector2D& iPositionInViewport );
    double      GetZoom() const;
    FVector2D   GetLocalMousePosition( const FVector2D& iMouseInViewport ) const;
    FOdysseyPoint   GetLocalMousePosition( const FOdysseyPoint& iPointInViewport ) const;
    void        DrawUVsOntoViewport( const FViewport* iViewport, FCanvas* ioCanvas, int32 iUVChannel, const FStaticMeshVertexBuffer& iVertexBuffer, const FIndexArrayView& iIndices );
    eState      InputChordToState();

    void        OnViewportPropertyWillChange();
    void        OnViewportPropertyChanged();

private:
    //New API to manage events
    void MouseDown(const FOdysseyPoint& iPoint);
    void MouseUp(const FOdysseyPoint& iPoint);
    void MouseDrag(const FOdysseyPoint& iPoint);
    bool KeyDown(FKey iKey);
    bool KeyUp(FKey iKey);

    TSharedPtr<FOdysseyHUDElement> GetHUDElement(FViewport* iViewport, int32 iX, int32 iY);

private:
    // Private Data Members
    FOdysseyPainterEditor*                    mOdysseyPainterEditor;
    TWeakPtr<SOdysseyViewport>              mOdysseyPainterEditorViewportPtr;
    FOdysseyMeshSelector*                   mMeshSelector;
    TObjectPtr<UTexture2D>                  mCheckerboardTexture;
    EMouseCursor::Type                      mCurrentMouseCursor;
    float                                   mRotationReference; // The reference from which we determine the new rotation
    FVector2D                               mPanReference; //Where did we begin the pan ?
    float                                   mZoomReference;
    FVector2D                               mZoomViewportPointReference; //Where did we begin the zoom ?


    eState                                  mCurrentToolState;

    FOnMouseDown                            mOnMouseDown;
    FOnMouseUp                              mOnMouseUp;
    FOnMouseClick                           mOnMouseClick;
    FOnMouseDoubleClick                     mOnMouseDoubleClick;
    FOnMouseHover                           mOnMouseHover;
    FOnMouseDrag                            mOnMouseDrag;
    FOnKeyDown                              mOnKeyDown;
    FOnKeyUp                                mOnKeyUp;

    FOdysseyPoint                            mCurrentPointInViewport;
    FOdysseyPoint                            mCurrentPointInTexture;
    std::chrono::steady_clock::time_point   mStylusLastEventTime;

    TArray<FKey>                            mKeysPressed;

    FTexture                                mNearestNeighbourTexture;
    FTexture                                mBilinearTexture;

    bool                                    mIsMouseDown = false;
    FKey                                    mMouseButton;
    FOdysseyPoint                           mMouseDownReference;
    FVector2D                               mHUDMouseDownReference;

    bool mIsRecordingStylus = false;
    bool mStylusIsDown = false;
    bool mIsFocused = false;

    FOdysseyPoint mCurrentHUDPoint;
    TSharedPtr<FOdysseyHUDElement> mCurrentHUDElement;
    TSharedPtr<FOdysseyHUDElement> mHoveredHUDElement;
};
