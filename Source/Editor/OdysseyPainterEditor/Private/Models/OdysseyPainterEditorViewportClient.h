// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "IStylusState.h"
#include "RawIndexBuffer.h"
#include "Rendering/StaticMeshVertexBuffer.h"
#include "UnrealClient.h"
#include "UObject/GCObject.h"

#include "Input/OdysseyPoint.h"
#include "OdysseyEventState.h"

#include <chrono>
#include <ULIS>

class UOdysseyStylusInputSubsystem;
class FCanvas;
class UTexture2D;

class FOdysseyMeshSelector;
class FOdysseyPainterEditorData;
class IStylusInputInterfaceInternal;
class SOdysseyViewport;
class FOdysseyPainterEditor;
class FOdysseyHUDElement;

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
	DECLARE_DELEGATE_TwoParams(FOnPickColor, eOdysseyEventState::Type, const FVector2D&)
    DECLARE_DELEGATE_RetVal_TwoParams(bool, FOnMouseDown, const FOdysseyPoint&, const FKey&)
    DECLARE_DELEGATE_RetVal_TwoParams(bool, FOnMouseUp, const FOdysseyPoint&, const FKey&)
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
        kPick
    };

public:
    // Construction / Destruction
    FOdysseyPainterEditorViewportClient(FOdysseyPainterEditor* iEditor, TWeakPtr< SOdysseyViewport > iTextureEditorViewport, FOdysseyMeshSelector* iMeshSelector);
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

    virtual void OnStylusStateChanged( const TWeakPtr<SWidget> iWidget, const TArray<FStylusState>& iStates, int32 iIndex ) override;
    void StartStylusInputRecord();
    void StopStylusInputRecord();
    FOdysseyPoint StylusStateToPoint(const FStylusState& iState);
    void ReadStylusInput();

    virtual EMouseCaptureMode GetMouseCaptureMode() const override;
    
public:
    // FGCObject API
    virtual void AddReferencedObjects( FReferenceCollector& ioCollector ) override;
    virtual FString GetReferencerName() const override;

public:
    // Public API
	FOnPickColor&	OnPickColor() { return mOnPickColor; }
    FOnMouseDown&   OnMouseDown()   { return mOnMouseDown; }
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
    UOdysseyStylusInputSubsystem*           InputSubsystem;
    FOdysseyPainterEditor*		            mOdysseyPainterEditor;
    TWeakPtr<SOdysseyViewport>              mOdysseyPainterEditorViewportPtr;
    FOdysseyMeshSelector*                   mMeshSelector;
    TObjectPtr<UTexture2D>                  mCheckerboardTexture;
    EMouseCursor::Type                      mCurrentMouseCursor;
    float                                   mRotationReference; // The reference from which we determine the new rotation
    FVector2D                               mPanReference; //Where did we begin the pan ?
    float                                   mZoomReference;
    FVector2D                               mZoomViewportPointReference; //Where did we begin the zoom ?


    eState                                  mCurrentToolState;

	FOnPickColor							mOnPickColor;
    FOnMouseDown                            mOnMouseDown;
    FOnMouseUp                              mOnMouseUp;
    FOnMouseDoubleClick                     mOnMouseDoubleClick;
    FOnMouseHover                           mOnMouseHover;
    FOnMouseDrag                            mOnMouseDrag;
    FOnKeyDown                              mOnKeyDown;
    FOnKeyUp                                mOnKeyUp;

	FOdysseyPoint						    mCurrentPointInViewport;
    FOdysseyPoint						    mCurrentPointInTexture;
    std::chrono::steady_clock::time_point   mStylusLastEventTime;

    TArray<FKey>                            mKeysPressed;

    FTexture                                mNearestNeighbourTexture;
    FTexture                                mBilinearTexture;

    bool                                    mIsMouseDown = false;
    FKey                                    mMouseButton;

    
    TArray<FStylusState> mStylusStates;
    bool mIsRecordingStylus = false;
    int mLastStylusEventIndex = 0;
    bool mStylusIsDown = false;

    FOdysseyPoint mCurrentHUDPoint;
    TSharedPtr<FOdysseyHUDElement> mCurrentHUDElement;
    TSharedPtr<FOdysseyHUDElement> mHoveredHUDElement;
};
