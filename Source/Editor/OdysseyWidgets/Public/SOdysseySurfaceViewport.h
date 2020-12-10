// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"


class FOdysseySceneViewport;
class SScrollBar;
class SViewport;
class IOdysseySurface;
class UTexture2D;


/////////////////////////////////////////////////////
// SOdysseySurfaceViewport
class ODYSSEYWIDGETS_API SOdysseySurfaceViewport : public SCompoundWidget
{
public:
    DECLARE_MULTICAST_DELEGATE( FOnParameterChanged );

public:

    SLATE_BEGIN_ARGS(SOdysseySurfaceViewport)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct( const FArguments& InArgs );

public:
    // Public API
    void                        SetSurface( IOdysseySurface* iValue );
    void                        SetViewportClient(TSharedPtr<class FViewportClient> InViewportClient);

    TSharedPtr<FOdysseySceneViewport>   GetViewport()              const;
    TSharedPtr<SViewport>               GetViewportWidget()        const;
    TSharedPtr<SScrollBar>              GetVerticalScrollBar()     const;
    TSharedPtr<SScrollBar>              GetHorizontalScrollBar()   const;
    IOdysseySurface*                    GetSurface() const;

    float                       GetViewportVerticalScrollBarRatio() const;
    float                       GetViewportHorizontalScrollBarRatio() const;

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
    void            HandleViewportReset();
    void            HandleRotationChanged( int newRotation );
    int             HandleRotationValue() const;
    FText           HandleSurfaceInfosTextValue() const;



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
    
    FOnParameterChanged& OnParameterChanged() { return mOnParameterChanged; }

private:
    // Private Member Data
    IOdysseySurface*            Surface;
    TSharedPtr<FViewportClient> ViewportClient;
    TSharedPtr<FOdysseySceneViewport>  Viewport;
    TSharedPtr<SViewport>       ViewportWidget;
    TSharedPtr<SScrollBar>      TextureViewportVerticalScrollBar;
    TSharedPtr<SScrollBar>      TextureViewportHorizontalScrollBar;
    double                      Zoom;
    double                      Rotation;
    FVector2D                   Pan;
    bool                        IsFitToViewport;
    uint32                      PreviewEffectiveTextureWidth;
    uint32                      PreviewEffectiveTextureHeight;

    FOnParameterChanged         mOnParameterChanged;
};
