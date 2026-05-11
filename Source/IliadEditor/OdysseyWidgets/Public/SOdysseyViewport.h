// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SViewport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SSpinBox.h"

class FExtender;
class FOdysseySceneViewport;
class SScrollBar;
class SViewport;
class UTexture2D;
class UTexture;


/////////////////////////////////////////////////////
// SOdysseyViewport
class ODYSSEYWIDGETS_API SOdysseyViewport : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyViewport)
        {}
        SLATE_ATTRIBUTE(UTexture*, Texture)
        SLATE_ARGUMENT(TSharedPtr<FExtender>, OptionExtender)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct( const FArguments& InArgs );

public:
    // Public API

    /* Ge the texture the viewport is displaying */
    UTexture* GetTexture() const;

    /* Get the scene viewport object */
    TSharedPtr<FOdysseySceneViewport>   GetViewport()              const;

    /* Get the viewport widget */
    TSharedPtr<SViewport>               GetViewportWidget()        const;

    /* Get the current Zoom value */
    double          GetZoom() const;

    /* Get the rotation value in Radians */
    double          GetRotation() const;

    /* Return the Rotation Value to display (in degrees) */
    int             GetGuiRotationValue() const;

    /* Get the Pan value */
    FVector2D       GetPan() const;

    /* Get the flip state */
    FVector2D       GetFlip() const;

    /* Returns the center point of the viewport widget */
    FVector2D       GetViewportCenter() const;

    /* Get the Fit To Viewport option state */
    bool            GetFitToViewport() const;

    /* Set the viewport client to use */
    void            SetViewportClient(TSharedPtr<class FViewportClient> InViewportClient);

    /* Add PanValue to current pan, panning in the widget coordinates system **/
    void            AddPan(FVector2D PanValue);

    /* Resets the Pan valule to its defalut value */
    void            ResetPan();

    /* Sets the new zoom value by zooming according to iZoomPosition
        iZoomPosition is the offset of the point to zoom from the center of the viewport
    */
    void            SetZoom(double ZoomValue, const FVector2D& iZoomPosition = FVector2D(0.0f, 0.0f));

    /* Set the rotation value in Radians, rotating according to the given PivotPoint */
    void            SetRotation(double RotationValue);

    /* Set the Fit To Viewport option state */
    void            SetFitToViewport(bool bFitToViewport);

    /* Toggle the Fit To Viewport option state */
    void            ToggleFitToViewport();

    /* Toggle the option to have the center of the viewport be the reference point while flipping instead of the center of the texture */
    void            ToggleAlignWithViewport();

    /* Returns the dimensions of the texture when displayed in the viewport (this is not a AABB) */
    void            ComputeTextureDisplayDimensions(uint32& Width, uint32& Height) const;

    /* Retrieve the transform of the displayed texture in the viewport */
    FTransform2D    GetTransformToDisplayedTexture();

    /* Retrieve the transform of the source texture in the viewport */
    FTransform2D    GetTransformToSourceTexture();

    /* Converts the given point from WorldCoodinates to LocalCoordinates (texture Coordinates, (0,0) being the center of the texture)*/
    FVector2D       ToLocal(const FVector2D& iPoint) const;

    /* Converts the given point from LocalCoordinates (texture Coordinates, (0,0) being the center of the texture) to WorldCoodinates */
    FVector2D       ToWorld(const FVector2D& iPoint) const;

public:
    // Public Shortcuts Methods

    /* Zoom in by adding iSliderOffsetToAdd to the zoom slider position */
    void            ZoomExponential(float iBaseZoom, float iSliderOffsetToAdd, const FVector2D& iZoomPosition = FVector2D(0.0f, 0.0f));

    /* Rotate the canvas to the Left, the Pivot point for the Rotation being in the middle of the viewport */
    void            RotateLeft();

    /* Rotate the canvas to the Right, the Pivot point for the Rotation being in the middle of the viewport */
    void            RotateRight();

    /* Flip the canvas Horizontally */
    void            FlipHorizontal();

    /* Flip the canvas Vertically */
    void            FlipVertical();

    /* Returns the tranformation matrix with the eventual flip */
    const FMatrix2x2            GetFlipMatrix() const;

private:
    // Private API

    /* Pans to the given PanValue **/
    void            Pan(FVector2D PanValue);

    /* Zooms at the given zoom value
        iZoomPosition is the offset of the point to zoom from the center of the viewport
    */
    void            Zoom(double ZoomValue, const FVector2D& iZoomPosition = FVector2D(0.0f, 0.0f));

    /* Rotate to the given Rotation value */
    void            Rotate(double RotationValue);

    /* Fits the displayed texture to the viewport */
    void            FitToViewport();

    /* Get the Vertical Scrollbar Widget */
    TSharedPtr<SScrollBar>              GetVerticalScrollBar()     const;

    /* Get the Horizontal Scrolbar Widget */
    TSharedPtr<SScrollBar>              GetHorizontalScrollBar()   const;

    /* Returns the expected translation from the given scrollbars offsets */
    FVector2D                   GetTranslationFromSlidersOffsets(float InScrollOffsetFractionX, float InScrollOffsetFractionY);

    /* Update the scrollbars according to the texture transform */
    void                        UpdateScrollBars();

    /* Update the transform matrix after an action (pan, rotation, flip...) from an user */
    void                        UpdateTransform();

private:
    // SWidget overrides
    virtual void    Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

private:
    // Private Slate Callbacks

    /* Return the Zoom Value to display */
    float           GetGuiZoomValue() const;

    /* Return the zoom menu Fit To Viewport option state */
    bool            IsZoomMenuFitChecked() const;

    /* Return the align with viewport option state */
    bool            IsAlignWithViewportChecked() const;

    /* Return the Texture Infos to display */
    FText           GetTextureInfosValue() const;

    /* Handles the horizontal scrollbar scroll event */
    void            HandleHorizontalScrollBarScrolled( float InScrollOffsetFraction );

    /* Handles the vertical scrollbar scroll event */
    void            HandleVerticalScrollBarScrolled(float InScrollOffsetFraction);

    /* Handles the zoom menu option (X %) clicked event */
    void            HandleZoomMenuEntryClicked( double ZoomValue );

    /* Handles the zoom menu Fit To Viewport option clicked event */
    void            HandleZoomMenuFitClicked();

    /* Handles the align with viewport option when flipping */
    void            HandleAlignWithViewportClicked();

    /* Handles the zoom slider changed event */
    void            HandleZoomSliderChanged( float NewValue );

    /* Handles Rotate Left button clicked event */
    void            HandleRotationLeft();

    /* Handles Rotate Right button clicked event */
    void            HandleRotationRight();

    /* Handles Flip X clicked event */
    void            HandleFlipHorizontal(ECheckBoxState iState);

    /* Handles Flip Y clicked event */
    void            HandleFlipVertical(ECheckBoxState iState);

    /* Handles Viewport Reset button clicked event */
    void            HandleViewportReset();

    /* Handles Rotation Spinbox value changed event */
    void            HandleRotationChanged( int newRotation );

private:
    // Private Member Data
    TAttribute<UTexture*>               mTexture;
    TWeakPtr<FViewportClient>           mViewportClient;
    TSharedPtr<FOdysseySceneViewport>   mViewport;
    TSharedPtr<SViewport>               mViewportWidget;
    TSharedPtr<SScrollBar>              mVerticalScrollBar;
    TSharedPtr<SScrollBar>              mHorizontalScrollBar;
    TSharedPtr<SSpinBox<float>>         mZoomSpinBox;
    FTransform2D                        mTransform;
    bool                                mIsFitToViewport;
    bool                                mAlignWithViewport; // Keep the center of the viewport at the same position when flipping it when on.

    /* The scale applied to the viewport */
    double                              mZoom;

    /* The rotation (in radians) applied to the viewport */
    double                              mRotation;

    /* The translation applied to the viewport */
    FVector2D                           mPan;

    /* (0,0) if no flip, (1,0) if flip X, (0,1) if flip Y, (1,1) if both axis are flipped */
    FVector2D                           mFlipStateUV;
};
