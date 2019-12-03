// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "UObject/GCObject.h"
#include "UnrealClient.h"
#include "OdysseyStrokePoint.h"
#include "Rendering/StaticMeshVertexBuffer.h"
#include "RawIndexBuffer.h"


class FCanvas;
class IOdysseyPainterEditorToolkit;
class SOdysseySurfaceViewport;
class UTexture2D;
class FOdysseyMeshSelector;

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
{
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
    FOdysseyPainterEditorViewportClient( TWeakPtr< IOdysseyPainterEditorToolkit > InTextureEditor, TWeakPtr< SOdysseySurfaceViewport > InTextureEditorViewport, FOdysseyMeshSelector* InMeshSelector );
    ~FOdysseyPainterEditorViewportClient();

public:
    // FViewportClient API
    virtual void  Draw( FViewport* Viewport, FCanvas* Canvas )  override;

    virtual bool InputKey( FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed = 1.0f, bool bGamepad = false ) override;
    virtual void CapturedMouseMove( FViewport* Viewport, int32 X, int32 Y ) override;
    virtual void MouseEnter( FViewport* Viewport, int32 X, int32 Y ) override;
    virtual void MouseLeave( FViewport* Viewport ) override;

    virtual EMouseCursor::Type                  GetCursor( FViewport* Viewport, int32 X, int32 Y ) override;
    virtual TOptional< TSharedRef< SWidget > >  MapCursor( FViewport* Viewport, const FCursorReply& CursorReply ) override;
    virtual EMouseCaptureMode                   CaptureMouseOnClick() override;
    
public:
    // FGCObject API
    virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;

public:
    // Public API
    void        ModifyCheckerboardTextureColors();
    FText       GetDisplayedResolution() const;

private:
    // Private API
    void        UpdateScrollBars();
    FVector2D   GetViewportScrollBarPositions() const;
    void        DestroyCheckerboardTexture();
    void        ZoomInInViewport( const FVector2D& iPositionInViewport );
    void        ZoomOutInViewport( const FVector2D& iPositionInViewport );
    double      GetZoom() const;
    FVector2D   GetLocalMousePosition( const FVector2D& iMouseInViewport, const bool iWithRotation = true )  const;
    FOdysseyStrokePoint   GetLocalMousePosition( const FOdysseyStrokePoint& iPointInViewport )  const;
    void DrawUVsOntoViewport(FViewport* InViewport, FCanvas* InCanvas, int32 UVChannel, FStaticMeshVertexBuffer& VertexBuffer, FIndexArrayView& Indices );

    bool        InputKeyWithStrokePoint( const FOdysseyStrokePoint& iPointInViewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed = 1.0f, bool bGamepad = false );
    void        CapturedMouseMoveWithStrokePoint( const FOdysseyStrokePoint& iPointInViewport ) ;

private:
    // Private Data Members
    EMouseCaptureMode                       mMouseCaptureMode;
    TWeakPtr<IOdysseyPainterEditorToolkit>  OdysseyPainterEditorPtr;
    TWeakPtr<SOdysseySurfaceViewport>       OdysseyPainterEditorViewportPtr;
    FOdysseyMeshSelector*                   MeshSelector;
    UTexture2D*                             CheckerboardTexture;
    EMouseCursor::Type                      CurrentMouseCursor;
    float                                   RotationReference; // The reference from which we determine the new rotation
    FVector2D                               PanReference; //Where did we begin the pan ?
    FVector2D                               PivotPointRatio; //Where is the center of the viewport from the center of the texture as a ratio, rotation independant

    eState                                  CurrentToolState;
};
