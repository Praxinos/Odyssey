// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "IStylusState.h"
#include "RawIndexBuffer.h"
#include "Rendering/StaticMeshVertexBuffer.h"
#include "UnrealClient.h"
#include "UObject/GCObject.h"

#include "OdysseyStrokePoint.h"
#include "OdysseyEventState.h"

#include <ULIS3>

class UOdysseyStylusInputSubsystem;
class FCanvas;
class UTexture2D;

class FOdysseyMeshSelector;
class FOdysseyPainterEditorData;
class IStylusInputInterfaceInternal;
class SOdysseySurfaceViewport;

//
//                                                                                                         MouseWheelUp/Down
//                                                                                                            ┌────►────┐
//                                                                                                            │         │
//                                  ╔═════════════════════════════════════════════════════════════════════════╧═════════╧══════════════════╗
//              ┌───────────────────╢                                             kIdle                                                    ╟───────┐
//              │                   ╚═╤═══════════════════╤═══════════════════╤══════════════════╤═══════════════════╤════════════════╤════╝       │
//              │                     │                   │                   │                  │                   │                │            │
//  'P' pressed ▼        'P' released ▲       'R' pressed ▼      'R' released ▲    'Alt' pressed ▼    'Alt' released ▲    LMB pressed ▼            ▲ LMB released
//              │                     │                   │                   │                  │                   │                │            │
//      ╔═══════╧═══════════╗         │           ╔═══════╧═══════════╗       │          ╔═══════╧═══════════╗       │             ╔══╧════════════╧═══╗
//      ║       kPan        ╟─────────┤           ║     kRotate       ╟───────┤          ║      kPick        ╟───────┤             ║     kDrawing      ║
//      ╚═══════╤══╤════════╝         │           ╚═══════╤══╤════════╝       │          ╚═══════╤══╤════════╝       │             ╚════╤═════════╤════╝
//              │  │                  │                   │  │                │                  │  │                │                  │         │
//  LMB pressed ▼  ▲ LMB released     │       LMB pressed ▼  ▲ LMB released   │      LMB pressed ▼  ▲ LMB released   │                  └────►────┘
//              │  │                  │                   │  │                │                  │  │                │                   LMB move  
//      ╔═══════╧══╧════════╗         │           ╔═══════╧══╧════════╗       │          ╔═══════╧══╧════════╗       │
//      ║     kPanning      ╟─────────┘           ║     kRotating     ╟───────┘          ║     kPicking      ╟───────┘
//      ╚════╤═════════╤════╝                     ╚════╤═════════╤════╝                  ╚════╤═════════╤════╝
//           │         │                               │         │                            │         │
//           └────►────┘                               └────►────┘                            └────►────┘
//            LMB move                                  LMB move                               LMB move
//  
//  
//  https://unicode-table.com/fr/#2500 ──◄──
//                         

/////////////////////////////////////////////////////
// FOdysseyPainterEditorViewportClient
class FOdysseyPainterEditorViewportClient
    : public FViewportClient
    , public FGCObject
    , public IStylusMessageHandler
{
public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPickColor, eOdysseyEventState::Type, const FVector2D&)

public:
    enum class eState
    {
        kIdle,
        kDrawing,
        kRotate,
        kRotating,
        kPan,
        kPanning,
        kPick,
        kPicking,
    };

public:
    // Construction / Destruction
    FOdysseyPainterEditorViewportClient(TWeakPtr<FOdysseyPainterEditorData>iData, TWeakPtr< SOdysseySurfaceViewport > iTextureEditorViewport, FOdysseyMeshSelector* iMeshSelector);
    ~FOdysseyPainterEditorViewportClient();

public:
    // FViewportClient API
    virtual void Draw( FViewport* iViewport, FCanvas* ioCanvas ) override;

    virtual bool InputKey( FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed = 1.0f, bool iGamepad = false ) override;
    virtual void CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY ) override;
    virtual void MouseEnter( FViewport* iViewport, int32 iX, int32 iY ) override;
    virtual void MouseLeave( FViewport* iViewport ) override;
    virtual void MouseMove(FViewport* iViewport, int32 iX, int32 iY) override;

    virtual EMouseCursor::Type                  GetCursor( FViewport* iViewport, int32 iX, int32 iY ) override;
    virtual TOptional< TSharedRef< SWidget > >  MapCursor( FViewport* iViewport, const FCursorReply& iCursorReply ) override;

    virtual void OnStylusStateChanged( const TWeakPtr<SWidget> iWidget, const FStylusState& iState, int32 iIndex ) override;
	
    virtual EMouseCaptureMode GetMouseCaptureMode() const override;
    
public:
    // FGCObject API
    virtual void AddReferencedObjects( FReferenceCollector& ioCollector ) override;

public:
    // Public API
    void        ModifyCheckerboardTextureColors();
    FText       GetDisplayedResolution() const;
	FOnPickColor&	OnPickColor() { return mOnPickColor; }

private:
    // Private API
    void        UpdateScrollBars();
    FVector2D   GetViewportScrollBarPositions() const;
    void        DestroyCheckerboardTexture();
    void        ZoomInInViewport( const FVector2D& iPositionInViewport );
    void        ZoomOutInViewport( const FVector2D& iPositionInViewport );
    double      GetZoom() const;
    FVector2D   GetLocalMousePosition( const FVector2D& iMouseInViewport, const bool iWithRotation = true ) const;
    FOdysseyStrokePoint   GetLocalMousePosition( const FOdysseyStrokePoint& iPointInViewport ) const;
    void        DrawUVsOntoViewport( const FViewport* iViewport, FCanvas* ioCanvas, int32 iUVChannel, const FStaticMeshVertexBuffer& iVertexBuffer, const FIndexArrayView& iIndices );

    bool        InputKeyWithStrokePoint( const FOdysseyStrokePoint& iPointInViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed = 1.0f, bool iGamepad = false );
    void        CapturedMouseMoveWithStrokePoint( const FOdysseyStrokePoint& iPointInViewport ) ;

    void        OnStylusInputChanged( TSharedPtr<IStylusInputInterfaceInternal> iStylusInput );

private:
    // Private Data Members
    UOdysseyStylusInputSubsystem*           InputSubsystem;
    FKey                                    mLastKey;
    EInputEvent                             mLastEvent;
    TWeakPtr<FOdysseyPainterEditorData>		mOdysseyPainterEditorDataPtr;
    TWeakPtr<SOdysseySurfaceViewport>       mOdysseyPainterEditorViewportPtr;
    FOdysseyMeshSelector*                   mMeshSelector;
    UTexture2D*                             mCheckerboardTexture;
    EMouseCursor::Type                      mCurrentMouseCursor;
    float                                   mRotationReference; // The reference from which we determine the new rotation
    FVector2D                               mPanReference; //Where did we begin the pan ?
    FVector2D                               mPivotPointRatio; //Where is the center of the viewport from the center of the texture as a ratio, rotation independant

    eState                                  mCurrentToolState;

	FOnPickColor							mOnPickColor;
    bool                                    mIsCapturedByStylus;
	FOdysseyStrokePoint						mCurrentPointInTexture;
    std::chrono::steady_clock::time_point   mStylusLastEventTime;

    TArray<FKey>                            mKeysPressed;
};
