// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Color/SOdysseyColorSlider.h"
#include "Rendering/DrawElements.h"
#include "OdysseyBlock.h"
#include <ULIS3>
#include "ULISLoaderModule.h"

#define LOCTEXT_NAMESPACE "OdysseyColorSlider"


/////////////////////////////////////////////////////
// IOdysseyChannelSlider
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
IOdysseyChannelSlider::Construct( const FArguments& InArgs )
{
    ODYSSEY_LEAF_WIDGET_FORWARD_CONSTRUCT_ARGS
    bMarkedAsInvalid = false;
    Init();
}

IOdysseyChannelSlider::IOdysseyChannelSlider( uint32 iFormat )
    : mFormat( iFormat ),
      cursor_brush( MakeUnique< FSlateBrush >() )
{
}

void
IOdysseyChannelSlider::Init()
{
    tSuperClass::Init();
    cursor_t = 0;
    cursor_pos = FVector2D( 0, 0 );
    cursor_size = FVector2D( 1, 1 );
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

    if (bMarkedAsInvalid || mDisplayedColor != mColor.Get())
    {
        PaintInternalBuffer();
        bMarkedAsInvalid = false;
        mDisplayedColor = mColor.Get();
    }

    FVector2D decal = ExternalSize / 2 - InternalSize / 2;
    float prop = GetProportionForColor(mColor.Get());
    cursor_pos = decal + FVector2D( ( InternalSize.X - cursor_size.X ) * prop, 0 );

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
    if (!cursor_surface || cursor_size.X != cursor_surface->Width() || cursor_size.Y != cursor_surface->Height())
    {
        cursor_surface = MakeUnique< FOdysseySurfaceEditable >(cursor_size.X, cursor_size.Y);
        cursor_brush->SetResourceObject(cursor_surface->Texture());
        cursor_brush->ImageSize.X = cursor_surface->Width();
        cursor_brush->ImageSize.Y = cursor_surface->Height();
        cursor_brush->DrawAs = ESlateBrushDrawType::Image;
    }
    //bMarkedAsInvalid = true;
}

void
IOdysseyChannelSlider::PaintInternalBuffer( int iReason ) const
{   
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 perfIntent = ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Fill( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, surface->Block()->GetBlock(), ::ul3::FPixelValue( ULIS3_FORMAT_RGB8, { 50, 50, 50 } ), surface->Block()->GetBlock()->Rect() );

    int range = InternalSize.X - 2;
    for( int x = 1; x < InternalSize.X - 2; ++x )
    {
        float t = float( x ) / float( range );
        ::ul3::FPixelValue res = ::ul3::Conv( GetColorForProportion( t ), surface->Block()->GetBlock()->Format() );
        for( int y = 1; y < InternalSize.Y - 1; ++y )
        {
            surface->Block()->GetBlock()->PixelProxy( x,y ).AssignMemoryUnsafe( res );
        }
    }
    surface->Invalidate();

    ::ul3::DrawRectOutlineNoAA( cursor_surface->Block()->GetBlock(), ::ul3::FPixelValue( ULIS3_FORMAT_RGBA8, { 5, 5, 5, 255 } ), ::ul3::FRect( 0, 0, cursor_surface->Width() - 1, cursor_surface->Height() - 1 ) );
    ::ul3::DrawRectOutlineNoAA( cursor_surface->Block()->GetBlock(), ::ul3::FPixelValue( ULIS3_FORMAT_RGBA8, { 255, 255, 255, 255 } ), ::ul3::FRect( 1, 1, cursor_surface->Width() - 2, cursor_surface->Height() - 2 ) );
    cursor_surface->Invalidate();
}


//--------------------------------------------------------------------------------------
//---------------------------------------------------- Public IOdysseyChannelSlider API
/* void
IOdysseyChannelSlider::SetColor( const ::ul3::FPixelValue& iColor )
{
    if( mColor == iColor )
        return;

    SetColor_Imp( iColor );
    cursor_t = GetProportionForColor_Imp( mColor );
    bMarkedAsInvalid = true;
}
*/

/* void
IOdysseyChannelSlider::SetPosition( float iPos )
{
    if( iPos == cursor_t )
        return;

    cursor_t = iPos;
    FPixelValue value = ::ul3::Conv( GetColorForProportion( cursor_t ), mFormat );
    bMarkedAsInvalid = true;
    //OnColorChangeCallback.ExecuteIfBound( cursor_t );
} */


/* float
IOdysseyChannelSlider::GetPosition() const
{
    return  cursor_t;
} */


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------- Event
FReply
IOdysseyChannelSlider::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    cursor_t = GetProportionForMousePosition(MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ));
    OnColorChangeCallback.ExecuteIfBound( eOdysseyEventState::kStart, GetColorForProportion(cursor_t) );
    return FReply::Handled().CaptureMouse(SharedThis(this));
}


FReply
IOdysseyChannelSlider::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if( !HasMouseCapture() )
        return FReply::Unhandled();
    
    cursor_t = GetProportionForMousePosition(MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ));
    OnColorChangeCallback.ExecuteIfBound(eOdysseyEventState::kSet, GetColorForProportion(cursor_t) );
    return FReply::Handled().ReleaseMouseCapture();
}


FReply
IOdysseyChannelSlider::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if( !HasMouseCapture() )
        return FReply::Unhandled();

    float pos = GetProportionForMousePosition(MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ));
    if (pos == cursor_t)
        return FReply::Handled();

    cursor_t = pos;
    OnColorChangeCallback.ExecuteIfBound(eOdysseyEventState::kAdjust, GetColorForProportion(cursor_t) );

    return FReply::Handled();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------- Private Event Handling

float
IOdysseyChannelSlider::GetProportionForMousePosition( FVector2D iPos ) const
{
    return FMath::Clamp( ( iPos.X - cursor_size.X / 2 ) / ( InternalSize.X - cursor_size.X ), 0.f, 1.f );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------- Private IOdysseyChannelSlider API

float
IOdysseyChannelSlider::GetProportionForColor( const ::ul3::FPixelValue& iColor ) const
{
    ::ul3::FPixelValue color = ::ul3::Conv( iColor, mFormat );
    return GetProportionForColor_Imp(color);
}

::ul3::FPixelValue
IOdysseyChannelSlider::GetColorForProportion( float t ) const
{
    ::ul3::FPixelValue result = ::ul3::Conv( mColor.Get(), mFormat );
    SetColorForProportion_Imp( result, t );
    result.SetAlphaF( 1.f );
    return result;
}


void
IOdysseyChannelSlider::SetColorForProportion_Imp( ::ul3::FPixelValue& color, float t ) const
{
    color.SetRedF( t );
}


#undef LOCTEXT_NAMESPACE

