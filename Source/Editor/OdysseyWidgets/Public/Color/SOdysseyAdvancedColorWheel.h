// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Types/SlateStructs.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Layout/Visibility.h"
#include "Widgets/SWidget.h"
#include "Layout/Margin.h"
#include "Layout/Children.h"
#include "Widgets/SPanel.h"
#include "SOdysseyLeafWidget.h"
#include "OdysseyEventState.h"
#include <ULIS>

//DECLARE_DELEGATE_OneParam( FOnColorChanged, const ::ULIS::FColor& );
DECLARE_DELEGATE_TwoParams( FOnColorChange, eOdysseyEventState::Type, const ::ULIS::FColor& );


/////////////////////////////////////////////////////
// SOdysseyAdvancedColorWheel
class ODYSSEYWIDGETS_API SOdysseyAdvancedColorWheel : public SOdysseyLeafWidget
{
private:
    typedef SOdysseyLeafWidget tSuperClass;

enum eEditMode
{
    kNone,
    kEditHue,
    kEditTriangle
};

public:
    SLATE_BEGIN_ARGS( SOdysseyAdvancedColorWheel )
        ODYSSEY_LEAF_WIDGET_CONSTRUCT_ARGS
        {}
        ODYSSEY_LEAF_WIDGET_CONSTRUCT_ATTRIBUTES
        SLATE_ATTRIBUTE( ::ULIS::FColor, Color )
        SLATE_EVENT( FOnColorChange, OnColorChange )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyAdvancedColorWheel();
    void Construct(const FArguments& InArgs);

public:
    // Public Callback API
    void SetColor( const ::ULIS::FColor& iColor );
    FVector2D GetInternalSize() const;
    FVector2D GetInternalPadding() const;
    bool IsFullyVisible() const;
    float GetDrawRatio() const;

private:
    // Public SWidget overrides
    virtual int32 OnPaint( const FPaintArgs& Args
                         , const FGeometry& AllottedGeometry
                         , const FSlateRect& MyCullingRect
                         , FSlateWindowElementList& OutDrawElements
                         , int32 LayerId
                         , const FWidgetStyle& InWidgetStyle
                         , bool bParentEnabled ) const override;

protected:
    // Buffer Utilities
    virtual void OnResizeEvent( const FVector2D& iNewSize ) const override;
    virtual void InitInternalBuffers() const override;
    virtual void PaintInternalBuffer( int iReason = 0 ) const override;

private:
    // Painting Utilities
    void PaintTriangle() const;

public:
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
    ::ULIS::FColor GetColorResult() const;

private:
    // Painting Utilities
    const FSlateBrush* WheelBG;
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
    const FSlateBrush* CursorOverlay;

    mutable FVector2D external_center;
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
    mutable ::ULIS::FColor mDisplayedColor;

    FOnColorChange OnColorChangeCallback;
    TAttribute<::ULIS::FColor> mColor;

};
