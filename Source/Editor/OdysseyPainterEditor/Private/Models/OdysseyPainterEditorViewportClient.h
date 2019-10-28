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



enum class eControlType
{
    kDrawing,
    kRotating,
    kZooming,
    kPanning,
    kPicking,
    kNothing
};


enum class eActivationType
{
    kToggle, //We pressed a key and when we'll release it, and the control type is active until we release it
    kOn, //We selected a controlType from a button in the tool selector. Left click or penButton1 triggers the control type
    kOff
};


/////////////////////////////////////////////////////
// FOdysseyPainterEditorViewportClient
class FOdysseyPainterEditorViewportClient
    : public FViewportClient
    , public FGCObject
{
public:
    // Construction / Destruction
    FOdysseyPainterEditorViewportClient( TWeakPtr< IOdysseyPainterEditorToolkit > InTextureEditor, TWeakPtr< SOdysseySurfaceViewport > InTextureEditorViewport, FOdysseyMeshSelector* InMeshSelector );
    ~FOdysseyPainterEditorViewportClient();

public:
    // FViewportClient API
    virtual void  Draw( FViewport* Viewport, FCanvas* Canvas )  override;
    virtual bool  InputKey( FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed = 1.0f, bool bGamepad = false )  override;
    virtual void  CapturedMouseMove( FViewport* Viewport, int32 X, int32 Y )  override;
    virtual EMouseCursor::Type  GetCursor( FViewport* Viewport,int32 X,int32 Y )  override;
    virtual TOptional< TSharedRef< SWidget > >  MapCursor( FViewport* Viewport, const FCursorReply& CursorReply ) override;
    // Tablet API
    //ODYSSEY: TABLET HANDLE BEGIN
    virtual void  TabletPenDataReceived( FViewport* Viewport, const FTabletEvent& TabletEvent )  override;
    //ODYSSEY: TABLET HANDLE END

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
    void        ZoomInInViewport( FViewport* Viewport );
    void        ZoomOutInViewport( FViewport* Viewport );
    double      GetZoom() const;
    FVector2D   GetLocalMousePosition( FViewport* Viewport )  const;
    void DrawUVsOntoViewport(FViewport* InViewport, FCanvas* InCanvas, int32 UVChannel, FStaticMeshVertexBuffer& VertexBuffer, FIndexArrayView& Indices );

private:
    // Private Data Members
    TWeakPtr<IOdysseyPainterEditorToolkit>  OdysseyPainterEditorPtr;
    TWeakPtr<SOdysseySurfaceViewport>       OdysseyPainterEditorViewportPtr;
    FOdysseyMeshSelector*                   MeshSelector;
    UTexture2D*                             CheckerboardTexture;
    FOdysseyStrokePoint                     RefEventStrokePoint;
    EMouseCursor::Type                      CurrentMouseCursor;
    float                                   RotationReference; // The reference from which we determine the new rotation
    FVector2D                               PanReference; //Where did we begin the pan ?
    bool                                    bLeftMouseDown;

    TPair< eControlType, eActivationType >  CurrentToolState;
};
