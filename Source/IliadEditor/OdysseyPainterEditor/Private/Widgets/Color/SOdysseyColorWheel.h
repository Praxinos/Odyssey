// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyEventState.h"
#include "Engine/TextureRenderTarget2D.h"

/////////////////////////////////////////////////////
// SOdysseyColorWheel
class SOdysseyColorWheel : public SLeafWidget
{
public:
    DECLARE_DELEGATE_TwoParams( FOnColorChanged, eOdysseyEventState::Type, const FLinearColor& );

enum eEditMode
{
    kNone,
    kEditHue,
    kEditTriangle
};

public:
    SLATE_BEGIN_ARGS( SOdysseyColorWheel )
        {}
        SLATE_ATTRIBUTE(UTexture2D*, TextureModel)
        SLATE_ATTRIBUTE( FLinearColor, Color )
        SLATE_EVENT( FOnColorChanged, OnColorChanged )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyColorWheel();
    void Construct(const FArguments& InArgs);

public:
    // Public Callback API
    /* void SetColor( const ::ULIS::FLinearColor& iColor );
    FVector2D GetInternalSize() const;
    FVector2D GetInternalPadding() const;
    bool IsFullyVisible() const;
    float GetDrawRatio() const; */

private:
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    // Public SWidget overrides
    virtual int32 OnPaint( const FPaintArgs& Args
                         , const FGeometry& AllottedGeometry
                         , const FSlateRect& MyCullingRect
                         , FSlateWindowElementList& OutDrawElements
                         , int32 LayerId
                         , const FWidgetStyle& InWidgetStyle
                         , bool bParentEnabled ) const override;

    virtual FVector2D ComputeDesiredSize(float) const override;

/*protected:
    // Buffer Utilities
    virtual void OnResizeEvent( const FVector2D& iNewSize ) const override;
    virtual void InitInternalBuffers() const override;
    virtual void PaintInternalBuffer( int iReason = 0 ) const override;*/

private:
    void UpdateTriangle();
    void OnObjectPropertyChanged(UObject* iObject, FPropertyChangedEvent& PropertyChangedEvent);
    // Painting Utilities
    //void PaintTriangle() const;

/*public:
    // Event
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    // Private Event Handling
    void  StartProcessMouseAction( FVector2D iPos );
    void  ProcessMouseAction( FVector2D iPos );
    void  ProcessEditHueAction( FVector2D iPos );
    void  ProcessEditTriangleAction( FVector2D iPos );

private:
    // Internal Geometry & Color Computing
    bool  IsInHue( FVector2D iPos );
    bool  IsInTriangle( FVector2D iPos );
    void  UpdateGeometry() const;
    void  UpdateTint() const;
    void  UpdateColor() const;
    ::ULIS::FLinearColor GetColorResult() const;*/

private:
    TAttribute<UTexture2D*> mTextureModel;
    TAttribute<FLinearColor> mColor;
    FOnColorChanged OnColorChanged;

    TStrongObjectPtr<UTextureRenderTarget2D> mTriangleRenderTarget;
    TStrongObjectPtr<UTexture2D> mTriangleTexture;
    TUniquePtr<FSlateBrush> mTriangleBrush;

    bool mNeedUpdateTriangle = false;

    // Painting Utilities
    /* const FSlateBrush* WheelBG;
    const FSlateBrush* InnerWheelBG;
    const FSlateBrush* InnerWheelHue;
    const FSlateBrush* InnerWheelDropShadow;
    const FSlateBrush* TriangleOverlay;

    const FSlateBrush* HintColorA;
    const FSlateBrush* HintColorB;

    const FSlateBrush* HueCursor;
    const FSlateBrush* HueCursorBG;
    const FSlateBrush* TriangleCursor;
    const FSlateBrush* TriangleCursorBG;
    const FSlateBrush* CursorOverlay; */

    /* mutable FVector2D external_center;
    mutable FVector2D internal_center;
    mutable FVector2D decal;
    mutable FVector2D draw_size;
    mutable FVector2D clamp_shift;
    mutable float draw_ratio;

    mutable FVector2D triangle_Point1;
    mutable FVector2D triangle_Point2;
    mutable FVector2D triangle_Point3;
    mutable FVector2D triangle_buffer_size;

    eEditMode mEditMode;

    mutable float hue_rad;
    mutable int hue_deg;

    mutable FVector triangle_cursor_barycentric_position;
    mutable FVector2D hue_cursor_direction;
    mutable FVector2D hue_cursor_position;
    mutable FVector2D triangle_cursor_cartesian_position;
    mutable FVector2D cursor_overlay_position;

    mutable FLinearColor hue_tint;
    mutable FLinearColor result_tint;
    mutable FLinearColor sat_tint;
    mutable FLinearColor lum_tint;

    mutable bool bMarkedAsInvalid;
    mutable ::ULIS::FLinearColor mDisplayedColor; */

};
