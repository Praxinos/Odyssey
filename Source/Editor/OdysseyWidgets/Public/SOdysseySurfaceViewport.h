// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"


class FSceneViewport;
class SScrollBar;
class SViewport;
class FOdysseySurface;
class UTexture2D;


/////////////////////////////////////////////////////
// SOdysseySurfaceViewport
class ODYSSEYWIDGETS_API SOdysseySurfaceViewport : public SCompoundWidget
{
public:

    SLATE_BEGIN_ARGS(SOdysseySurfaceViewport)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct( const FArguments& InArgs );

public:
    // Public API
    void                        SetSurface( FOdysseySurface* iValue );
    TSharedPtr<FSceneViewport>  GetViewport()              const;
    TSharedPtr<SViewport>       GetViewportWidget()        const;
    TSharedPtr<SScrollBar>      GetVerticalScrollBar()     const;
    TSharedPtr<SScrollBar>      GetHorizontalScrollBar()   const;
    FOdysseySurface*            GetSurface() const;
    float                       GetViewportVerticalScrollBarRatio() const;
    float                       GetViewportHorizontalScrollBarRatio() const;
    void                        SetViewportClient(TSharedPtr<class FViewportClient> InViewportClient);

public:
    // SWidget overrides
    virtual void    Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

private:
    // Private Callbacks
    void            HandleHorizontalScrollBarScrolled( float InScrollOffsetFraction );
    EVisibility     HandleHorizontalScrollBarVisibility( ) const;
    void            HandleVerticalScrollBarScrolled( float InScrollOffsetFraction );
    EVisibility     HandleVerticalScrollBarVisibility( ) const;
    void            HandleZoomMenuEntryClicked( double ZoomValue );
    void            HandleZoomMenuFitClicked();
    bool            IsZoomMenuFitChecked() const;
    FText           HandleZoomPercentageText( ) const;
    void            HandleZoomSliderChanged( float NewValue );
    float           HandleZoomSliderValue( ) const;
    void            HandleRotationLeft();
    void            HandleRotationRight();
    void            HandleRotationReset();
    void            HandleRotationChanged( int newRotation );
    int             HandleRotationValue() const;
    FText           HandleSurfaceSizeTextValue() const;



public:
    // Navigation API
    double          GetZoom() const;
    void            SetZoom(double ZoomValue);
    void            ZoomIn();
    void            ZoomOut();
    bool            GetFitToViewport() const;
    void            SetFitToViewport( bool bFitToViewport );
    void            ToggleFitToViewport();
    double          GetRotationInDegrees() const;
    void            SetRotationInDegrees(double RotationValue);
    FVector2D       GetPan() const;
    void            SetPan( FVector2D PanValue );

    /* Add Pan in parameter to current pan **/
    void            AddPan( FVector2D PanValue );

    void            RotateLeft();
    void            RotateRight();
    void            CalculateTextureDisplayDimensions(uint32& Width, uint32& Height) const;

private:
    // Private Member Data
    FOdysseySurface*            Surface;
    TSharedPtr<FViewportClient> ViewportClient;
    TSharedPtr<FSceneViewport>  Viewport;
    TSharedPtr<SViewport>       ViewportWidget;
    TSharedPtr<SScrollBar>      TextureViewportVerticalScrollBar;
    TSharedPtr<SScrollBar>      TextureViewportHorizontalScrollBar;
    double                      Zoom;
    double                      Rotation;
    FVector2D                   Pan;
    bool                        IsFitToViewport;
    uint32                      PreviewEffectiveTextureWidth;
    uint32                      PreviewEffectiveTextureHeight;
};
