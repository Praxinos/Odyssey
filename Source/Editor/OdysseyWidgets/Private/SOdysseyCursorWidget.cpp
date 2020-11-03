// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "SOdysseyCursorWidget.h"
#include "Rendering/DrawElements.h"

#define LOCTEXT_NAMESPACE "SOdysseyCursorWidget"


/////////////////////////////////////////////////////
// SOdysseyCursorWidget
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
SOdysseyCursorWidget::SOdysseyCursorWidget()
    : Icon( NULL )
{}


SOdysseyCursorWidget::~SOdysseyCursorWidget()
{}


void SOdysseyCursorWidget::Construct(const FArguments& InArgs)
{
    DesiredWidth    = InArgs._DesiredWidth;
    DesiredHeight   = InArgs._DesiredHeight;
    Icon            = InArgs._Icon;
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Public SWidget overrides
int32
SOdysseyCursorWidget::OnPaint( const FPaintArgs& Args
                           , const FGeometry& AllottedGeometry
                           , const FSlateRect& MyCullingRect
                           , FSlateWindowElementList& OutDrawElements
                           , int32 LayerId
                           , const FWidgetStyle& InWidgetStyle
                           , bool bParentEnabled ) const
{
    FSlateDrawElement::MakeBox( OutDrawElements,
                                LayerId,
                                AllottedGeometry.ToPaintGeometry( FVector2D(0,0), FVector2D( DesiredWidth, DesiredHeight ) ),
                                Icon,
                                ESlateDrawEffect::None,
                                FLinearColor( 1, 1, 1, 1 ) );
    return LayerId;
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------ Private sWidget overrides
FVector2D
SOdysseyCursorWidget::ComputeDesiredSize( float ) const
{
    return FVector2D( DesiredWidth, DesiredHeight );
}


#undef LOCTEXT_NAMESPACE

