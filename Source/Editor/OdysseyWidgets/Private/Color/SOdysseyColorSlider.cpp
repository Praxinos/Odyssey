// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Color/SOdysseyColorSlider.h"
#include "Rendering/DrawElements.h"
#include "OdysseyBlock.h"
#include <ULIS_CORE>

#define LOCTEXT_NAMESPACE "OdysseyColorSlider"


/////////////////////////////////////////////////////
// IOdysseyChannelSlider
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
IOdysseyChannelSlider::Construct( const FArguments& InArgs )
{
    ODYSSEY_LEAF_WIDGET_FORWARD_CONSTRUCT_ARGS
    OnChannelChangedCallback = InArgs._OnChannelChanged;
    Init();
}


void
IOdysseyChannelSlider::Init()
{
    tSuperClass::Init();
    cursor_t = 0;
    cursor_pos = FVector2D( 0, 0 );
    cursor_size = FVector2D( 1, 1 );
    mColor = ::ULIS::CColor( 0, 0, 0 );
    bMarkedAsInvalid = false;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Public SWidget overrides
int32
IOdysseyChannelSlider::OnPaint( const FPaintArgs& Args
                              , const FGeometry& AllottedGeometry
                              , const FSlateRect& MyCullingRect
                              , FSlateWindowElementList& OutDrawElements
                              , int32 LayerId
                              , const FWidgetStyle& InWidgetStyle
                              , bool bParentEnabled ) const
{
    CheckResize( AllottedGeometry.GetLocalSize() );

    if( bMarkedAsInvalid )
    {
        PaintInternalBuffer();
        bMarkedAsInvalid = false;
    }

    FVector2D decal = ExternalSize / 2 - InternalSize / 2;
    cursor_pos = decal + FVector2D( ( InternalSize.X - cursor_size.X ) * cursor_t, 0 );

    FSlateDrawElement::MakeBox(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry( decal, InternalSize ), ItemBrush.Get(), ESlateDrawEffect::None, FLinearColor( 1, 1, 1, 1 ) );
    FSlateDrawElement::MakeBox(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry( cursor_pos, cursor_size ), cursor_brush.Get(), ESlateDrawEffect::None, FLinearColor( 1, 1, 1, 1 ) );
    return LayerId;
}


//--------------------------------------------------------------------------------------
//---------------------------------------- SOdysseyLeafWidget Buffer Utilities overrides
void
IOdysseyChannelSlider::InitInternalBuffers() const
{
    tSuperClass::InitInternalBuffers();
    cursor_size = FVector2D( 8, InternalSize.Y );
    cursor_surface = MakeUnique< FOdysseySurface >( cursor_size.X, cursor_size.Y );
    cursor_brush = MakeUnique< FSlateBrush >();
    cursor_brush->SetResourceObject( cursor_surface->Texture() );
    cursor_brush->ImageSize.X = surface->Width();
    cursor_brush->ImageSize.Y = surface->Height();
    cursor_brush->DrawAs = ESlateBrushDrawType::Image;
}

void
IOdysseyChannelSlider::PaintInternalBuffer( int iReason ) const
{
    ::ULIS::FClearFillContext::Fill( surface->Block()->GetIBlock(), ::ULIS::CColor::FromRGB( 50, 50, 50 ), ::ULIS::FPerformanceOptions(), false );

    int range = InternalSize.X - 2;
    for( int x = 1; x < InternalSize.X - 2; ++x )
    {
        float t = float( x ) / float( range );
        ::ULIS::CColor res = GetColorForProportion( t ).ToRGB();
        for( int y = 1; y < InternalSize.Y - 1; ++y )
        {
            surface->Block()->GetIBlock()->SetPixelColor( x, y, res );
        }
    }
    surface->Invalidate();

    ::ULIS::FPainterContext::DrawRectangle( cursor_surface->Block()->GetIBlock()
                                          , ::ULIS::FPoint( 0, 0 )
                                          , ::ULIS::FPoint( cursor_surface->Width(), cursor_surface->Height() )
                                          , ::ULIS::CColor( 5, 5, 5 )
                                          , false );

    ::ULIS::FPainterContext::DrawRectangle( cursor_surface->Block()->GetIBlock()
                                          , ::ULIS::FPoint( 1, 1 )
                                          , ::ULIS::FPoint( cursor_surface->Width() - 1, cursor_surface->Height() - 1 )
                                          , ::ULIS::CColor( 255, 255, 255 )
                                          , false );

    cursor_surface->Invalidate();
}


//--------------------------------------------------------------------------------------
//---------------------------------------------------- Public IOdysseyChannelSlider API
void
IOdysseyChannelSlider::SetColor( const ::ULIS::CColor& iColor )
{
    if( mColor == iColor )
        return;

    SetColor_Imp( iColor );
    cursor_t = GetProportionForColor_Imp( mColor );
    bMarkedAsInvalid = true;
    OnChannelChangedCallback.ExecuteIfBound( cursor_t );
}


void
IOdysseyChannelSlider::SetPosition( float iPos )
{
    if( iPos == cursor_t )
        return;

    cursor_t = iPos;
    mColor = GetColorForProportion( cursor_t );
    bMarkedAsInvalid = true;
    OnChannelChangedCallback.ExecuteIfBound( cursor_t );
}


float
IOdysseyChannelSlider::GetPosition() const
{
    return  cursor_t;
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------- Event
FReply
IOdysseyChannelSlider::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    ProcessMouseAction( MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ) );
    return FReply::Handled().CaptureMouse(SharedThis(this));
}


FReply
IOdysseyChannelSlider::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if( HasMouseCapture() )
        return FReply::Handled().ReleaseMouseCapture();

    return FReply::Unhandled();
}


FReply
IOdysseyChannelSlider::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if( !HasMouseCapture() )
        return FReply::Unhandled();

    ProcessMouseAction( MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ) );

    return FReply::Handled();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------- Private Event Handling
void
IOdysseyChannelSlider::ProcessMouseAction( FVector2D iPos )
{
    SetPosition( FMath::Clamp( ( iPos.X - cursor_size.X / 2 ) / ( InternalSize.X - cursor_size.X ), 0.f, 1.f ) );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------- Private IOdysseyChannelSlider API
void
IOdysseyChannelSlider::SetColor_Imp( const ::ULIS::CColor& iColor )
{
    mColor = iColor.ToRGB();
}


::ULIS::CColor
IOdysseyChannelSlider::GetColorForProportion( float t ) const
{
    ::ULIS::CColor result = mColor;
    SetColorForProportion_Imp( result, t );
    return result;
}


void
IOdysseyChannelSlider::SetColorForProportion_Imp( ::ULIS::CColor& color, float t ) const
{
    color.SetRedF( t );
}


#undef LOCTEXT_NAMESPACE

