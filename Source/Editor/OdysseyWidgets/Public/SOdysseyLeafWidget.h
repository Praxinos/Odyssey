// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Types/SlateStructs.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SLeafWidget.h"

#include "Layout/Visibility.h"
#include "Widgets/SWidget.h"
#include "Layout/Margin.h"
#include "Layout/Children.h"
#include "Widgets/SPanel.h"

class FOdysseySurfaceEditable;

#define ODYSSEY_LEAF_WIDGET_CONSTRUCT_ARGS              \
    : _DesiredWidth(FOptionalSize())                    \
    , _DesiredHeight(FOptionalSize())                   \
    , _MinDesiredWidth(FOptionalSize())                 \
    , _MinDesiredHeight(FOptionalSize())                \
    , _MaxDesiredWidth(FOptionalSize())                 \
    , _MaxDesiredHeight(FOptionalSize())                \

#define ODYSSEY_LEAF_WIDGET_CONSTRUCT_ATTRIBUTES        \
    SLATE_ATTRIBUTE( FOptionalSize, DesiredWidth )      \
    SLATE_ATTRIBUTE( FOptionalSize, DesiredHeight )     \
    SLATE_ATTRIBUTE( FOptionalSize, MinDesiredWidth )   \
    SLATE_ATTRIBUTE( FOptionalSize, MinDesiredHeight )  \
    SLATE_ATTRIBUTE( FOptionalSize, MaxDesiredWidth )   \
    SLATE_ATTRIBUTE( FOptionalSize, MaxDesiredHeight )

#define ODYSSEY_LEAF_WIDGET_FORWARD_CONSTRUCT_ARGS      \
    DesiredWidth = InArgs._DesiredWidth;                \
    DesiredHeight = InArgs._DesiredHeight;              \
    MinDesiredWidth = InArgs._MinDesiredWidth;          \
    MinDesiredHeight = InArgs._MinDesiredHeight;        \
    MaxDesiredWidth = InArgs._MaxDesiredWidth;          \
    MaxDesiredHeight = InArgs._MaxDesiredHeight;

#define ODYSSEY_LEAF_WIDGET_MEMBERS                     \
    TAttribute< FOptionalSize > DesiredWidth;           \
    TAttribute< FOptionalSize > DesiredHeight;          \
    TAttribute< FOptionalSize > MinDesiredWidth;        \
    TAttribute< FOptionalSize > MinDesiredHeight;       \
    TAttribute< FOptionalSize > MaxDesiredWidth;        \
    TAttribute< FOptionalSize > MaxDesiredHeight;

class ODYSSEYWIDGETS_API SOdysseyLeafWidget : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS( SOdysseyLeafWidget )
        ODYSSEY_LEAF_WIDGET_CONSTRUCT_ARGS
        {}
        ODYSSEY_LEAF_WIDGET_CONSTRUCT_ATTRIBUTES
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    SOdysseyLeafWidget();
    virtual ~SOdysseyLeafWidget();
    void Construct(const FArguments& InArgs);

protected:
    virtual void Init();

private:
    // Public SWidget overrides
    virtual int32 OnPaint( const FPaintArgs& Args
                         , const FGeometry& AllottedGeometry
                         , const FSlateRect& MyCullingRect
                         , FSlateWindowElementList& OutDrawElements
                         , int32 LayerId
                         , const FWidgetStyle& InWidgetStyle
                         , bool bParentEnabled ) const override;

private:
    // Private sWidget overrides
    virtual FVector2D ComputeDesiredSize(float) const override;

public:
    // Setters
    void SetDesiredWidth(       TAttribute< FOptionalSize > InDesiredWidth );
    void SetDesiredHeight(      TAttribute< FOptionalSize > InDesiredHeight );
    void SetMinDesiredWidth(    TAttribute< FOptionalSize > InMinDesiredWidth );
    void SetMinDesiredHeight(   TAttribute< FOptionalSize > InMinDesiredHeight );
    void SetMaxDesiredWidth(    TAttribute< FOptionalSize > InMaxDesiredWidth );
    void SetMaxDesiredHeight(   TAttribute< FOptionalSize > InMaxDesiredHeight );

protected:
    // Buffer Utilities
    void CheckResize( const FVector2D& iNewSize ) const;
    virtual void OnResizeEvent( const FVector2D& iNewSize ) const;
    virtual void InitInternalBuffers() const;
    virtual void PaintInternalBuffer( int iReason = 0 ) const;

protected:
    //@hack: since OnPaint is const we have to mess with mutable stuff
    // to reallocate the internal buffer or paint on it
    mutable FVector2D InternalSize;
    mutable FVector2D ExternalSize;
    mutable TUniquePtr< FOdysseySurfaceEditable > surface;
    mutable TUniquePtr< FSlateBrush > ItemBrush;

    ODYSSEY_LEAF_WIDGET_MEMBERS
};
