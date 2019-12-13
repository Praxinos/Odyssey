// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "SOdysseyLeafWidget.h"
#include "Rendering/DrawElements.h"
#include "OdysseySurface.h"
#include "OdysseyBlock.h"
#include <ULIS_CORE>

#define LOCTEXT_NAMESPACE "OdysseyLeafWidget"


/////////////////////////////////////////////////////
// SOdysseyLeafWidget
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
SOdysseyLeafWidget::SOdysseyLeafWidget()  {}
SOdysseyLeafWidget::~SOdysseyLeafWidget() {}


void SOdysseyLeafWidget::Construct(const FArguments& InArgs)
{
    ODYSSEY_LEAF_WIDGET_FORWARD_CONSTRUCT_ARGS
    Init();
}


void
SOdysseyLeafWidget::Init()
{
    InternalSize = FVector2D( 1, 1 );
    ExternalSize = FVector2D( 1, 1 );
    InitInternalBuffers();
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Public SWidget overrides
int32
SOdysseyLeafWidget::OnPaint( const FPaintArgs& Args
                           , const FGeometry& AllottedGeometry
                           , const FSlateRect& MyCullingRect
                           , FSlateWindowElementList& OutDrawElements
                           , int32 LayerId
                           , const FWidgetStyle& InWidgetStyle
                           , bool bParentEnabled ) const
{
    CheckResize( AllottedGeometry.GetLocalSize() );
    FSlateDrawElement::MakeBox(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry( ExternalSize / 2 - InternalSize / 2, InternalSize ), ItemBrush.Get(), ESlateDrawEffect::None, FLinearColor( 1, 1, 1, 1 ) );
    return LayerId;
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------ Private sWidget overrides
FVector2D
SOdysseyLeafWidget::ComputeDesiredSize( float ) const
{
    FVector2D result = ExternalSize;
    const FOptionalSize CurrentDesiredWidth = DesiredWidth.Get();
    const FOptionalSize CurrentDesiredHeight = DesiredHeight.Get();
    result.X = CurrentDesiredWidth.IsSet() ? CurrentDesiredWidth.Get() : ExternalSize.X;
    result.Y = CurrentDesiredHeight.IsSet() ? CurrentDesiredHeight.Get() : ExternalSize.Y;
    return result;
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters
void
SOdysseyLeafWidget::SetDesiredWidth( TAttribute< FOptionalSize > InDesiredWidth )
{
    if( !DesiredWidth.IdenticalTo( InDesiredWidth ) )
    {
        DesiredWidth = InDesiredWidth;
        Invalidate( EInvalidateWidget::LayoutAndVolatility );
    }
}


void
SOdysseyLeafWidget::SetDesiredHeight( TAttribute< FOptionalSize > InDesiredHeight )
{
    if( !DesiredHeight.IdenticalTo( InDesiredHeight ) )
    {
        DesiredHeight = InDesiredHeight;
        Invalidate( EInvalidateWidget::LayoutAndVolatility );
    }
}


void
SOdysseyLeafWidget::SetMinDesiredWidth( TAttribute< FOptionalSize > InMinDesiredWidth )
{
    if( !MinDesiredWidth.IdenticalTo( InMinDesiredWidth ) )
    {
        MinDesiredWidth = InMinDesiredWidth;
        Invalidate( EInvalidateWidget::LayoutAndVolatility );
    }
}


void
SOdysseyLeafWidget::SetMinDesiredHeight( TAttribute< FOptionalSize > InMinDesiredHeight )
{
    if( !MinDesiredHeight.IdenticalTo( InMinDesiredHeight ) )
    {
        MinDesiredHeight = InMinDesiredHeight;
        Invalidate( EInvalidateWidget::LayoutAndVolatility );
    }
}


void
SOdysseyLeafWidget::SetMaxDesiredWidth( TAttribute< FOptionalSize > InMaxDesiredWidth )
{
    if( !MaxDesiredWidth.IdenticalTo( InMaxDesiredWidth ) )
    {
        MaxDesiredWidth = InMaxDesiredWidth;
        Invalidate( EInvalidateWidget::LayoutAndVolatility );
    }
}


void
SOdysseyLeafWidget::SetMaxDesiredHeight( TAttribute< FOptionalSize > InMaxDesiredHeight )
{
    if( !MaxDesiredHeight.IdenticalTo( InMaxDesiredHeight ) )
    {
        MaxDesiredHeight = InMaxDesiredHeight;
        Invalidate( EInvalidateWidget::LayoutAndVolatility );
    }
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Buffer Utilities
void
SOdysseyLeafWidget::CheckResize( const FVector2D& iNewSize ) const
{
    if( ExternalSize != iNewSize && iNewSize > FVector2D::ZeroVector )
    {
        ExternalSize = iNewSize;
        FVector2D previous_internal_size = InternalSize;
        OnResizeEvent( iNewSize );
        FVector2D new_internal_size = InternalSize;

        if( previous_internal_size != new_internal_size )
        {
            InitInternalBuffers();
            PaintInternalBuffer();
        }
    }
}


void
SOdysseyLeafWidget::OnResizeEvent( const FVector2D& iNewSize ) const
{
    const FOptionalSize CurrentDesiredWidth = DesiredWidth.Get();
    const FOptionalSize CurrentDesiredHeight = DesiredHeight.Get();
    InternalSize.X = CurrentDesiredWidth.IsSet() ? CurrentDesiredWidth.Get() : ExternalSize.X;
    InternalSize.Y = CurrentDesiredHeight.IsSet() ? CurrentDesiredHeight.Get() : ExternalSize.Y;

    const FOptionalSize CurrentMinDesiredWidth = MinDesiredWidth.Get();
    const FOptionalSize CurrentMaxDesiredWidth = MaxDesiredWidth.Get();
    const FOptionalSize CurrentMinDesiredHeight = MinDesiredHeight.Get();
    const FOptionalSize CurrentMaxDesiredHeight = MaxDesiredHeight.Get();

    if( CurrentMinDesiredWidth.IsSet() )
        InternalSize.X = FMath::Max( InternalSize.X, CurrentMinDesiredWidth.Get() );

    if( CurrentMaxDesiredWidth.IsSet() )
        InternalSize.X = FMath::Min( InternalSize.X, CurrentMaxDesiredWidth.Get() );

    if( CurrentMinDesiredHeight.IsSet() )
        InternalSize.Y = FMath::Max( InternalSize.Y, CurrentMinDesiredHeight.Get() );

    if( CurrentMaxDesiredHeight.IsSet() )
        InternalSize.Y = FMath::Min( InternalSize.Y, CurrentMaxDesiredHeight.Get() );
}


void
SOdysseyLeafWidget::InitInternalBuffers() const
{
    surface = MakeUnique< FOdysseySurface >( FMath::Max( 1.f, InternalSize.X ), FMath::Max( 1.f, InternalSize.Y ) );
    ItemBrush = MakeUnique< FSlateBrush >();
    ItemBrush->SetResourceObject( surface->Texture() );
    ItemBrush->ImageSize.X = surface->Width();
    ItemBrush->ImageSize.Y = surface->Height();
    ItemBrush->DrawAs = ESlateBrushDrawType::Image;
}


void
SOdysseyLeafWidget::PaintInternalBuffer( int iReason ) const
{
    ::ULIS::FClearFillContext::Fill( surface->Block()->GetIBlock(), ::ULIS::CColor::FromRGB( 220, 220, 220 ), ::ULIS::FPerformanceOptions(), false );
    surface->Invalidate();
}


#undef LOCTEXT_NAMESPACE

