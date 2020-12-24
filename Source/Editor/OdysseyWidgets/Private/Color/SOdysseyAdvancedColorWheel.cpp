// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Color/SOdysseyAdvancedColorWheel.h"
#include "OdysseyStyleSet.h"
#include "OdysseySurfaceEditable.h"
#include "OdysseyBlock.h"
#include <ULIS3>

#define LOCTEXT_NAMESPACE "OdysseyAdvancedColorWheel"


/////////////////////////////////////////////////////
// Default Values Defines
#define BASE_WIDTH              1024.f
#define BASE_HEIGHT             1024.f
#define BASE_SIZE               1024.f
#define RATIO                   1
#define OUTER_RADIUS            405.f
#define MIDDLE_RADIUS           380.f
#define INNER_RADIUS            360.f
#define MARK_RADIUS             330.f
#define FINAL_RADIUS            255.f
#define HINT_COLOR_A_X          119.f
#define HINT_COLOR_A_Y          787.f
#define HINT_COLOR_B_X          297.f
#define HINT_COLOR_B_Y          888.f
#define TRIANGLE_X              330.f
#define TRIANGLE_Y              200.f
#define OUTER_RATIO             OUTER_RADIUS    /   BASE_SIZE
#define INNER_RATIO             INNER_RADIUS    /   BASE_SIZE
#define MARK_RATIO              MARK_RADIUS     /   BASE_SIZE
#define FINAL_RATION            FINAL_RADIUS    /   BASE_SIZE
#define HINT_COLOR_A_X_RATIO    HINT_COLOR_A_X  /   BASE_SIZE
#define HINT_COLOR_A_Y_RATIO    HINT_COLOR_A_Y  /   BASE_SIZE
#define HINT_COLOR_A_LOC_RATIO  FVector2D( HINT_COLOR_A_X_RATIO, HINT_COLOR_A_Y_RATIO )
#define HINT_COLOR_B_X_RATIO    HINT_COLOR_B_X  /   BASE_SIZE
#define HINT_COLOR_B_Y_RATIO    HINT_COLOR_B_Y  /   BASE_SIZE
#define HINT_COLOR_B_LOC_RATIO  FVector2D( HINT_COLOR_B_X_RATIO, HINT_COLOR_B_Y_RATIO )
#define TRIANGLE_X_RATIO        TRIANGLE_X  /   BASE_SIZE
#define TRIANGLE_Y_RATIO        TRIANGLE_Y  /   BASE_SIZE
#define TRIANGLE_LOC_RATIO      FVector2D( TRIANGLE_X_RATIO, TRIANGLE_Y_RATIO )
#define COS2PIs3                0.5f
#define SIN2PIs3                0.86602540378f


/////////////////////////////////////////////////////
// Utility Functions
FVector
GetBarycentricCoordinates( FVector2D A, FVector2D B, FVector2D C, FVector2D iPos )
{
    float d = ( ( B.Y - C.Y) * (A.X - C.X) + (C.X - B.X) * (A.Y - C.Y));
    float a = ( ( B.Y - C.Y) * (iPos.X - C.X) + (C.X - B.X) * (iPos.Y - C.Y)) / d;
    float b = ( ( C.Y - A.Y) * (iPos.X - C.X) + (A.X - C.X) * (iPos.Y - C.Y)) / d;
    float c = 1 - a -b;

    return FVector( a,b,c );
}


FVector
ClampBarycentricCoordinates( FVector iBaryPos )
{
    float x = iBaryPos.X;
    float y = iBaryPos.Y;
    float z = iBaryPos.Z;

    if(x<0)
    {
        y+=x/2;
        z+=x/2;
        x=0;
    }

    if(y<0)
    {
        x+=y/2;
        z+=y/2;
        y=0;
    }

    if(z<0)
    {
        x+=z/2;
        y+=z/2;
        z=0;
    }

    if(x>1)
    {
        x=1;
        y=0;
        z=0;
    }

    if(y>1)
    {
        x=0;
        y=1;
        z=0;
    }

    if(z>1)
    {
        x=0;
        y=0;
        z=1;
    }

    return FVector(x,y,z);
}

FVector2D
BarycentricToCartesianCoordinates( FVector2D A, FVector2D B, FVector2D C, FVector iBaryPos )
{
    double x = iBaryPos.X;
    double y = iBaryPos.Y;
    double z = iBaryPos.Z;

    return FVector2D( x*A.X + y*B.X + z*C.X,x*A.Y + y*B.Y+ z*C.Y );
}


/////////////////////////////////////////////////////
// SOdysseyAdvancedColorWheel
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
SOdysseyAdvancedColorWheel::~SOdysseyAdvancedColorWheel() {
    //delete colorA;
}

void SOdysseyAdvancedColorWheel::Construct(const FArguments& InArgs)
{
    ODYSSEY_LEAF_WIDGET_FORWARD_CONSTRUCT_ARGS

    WheelBG                 = FOdysseyStyle::GetBrush("AdvancedColorWheel.WheelBG");
    InnerWheelBG            = FOdysseyStyle::GetBrush("AdvancedColorWheel.InnerWheelBG");
    InnerWheelHue           = FOdysseyStyle::GetBrush("AdvancedColorWheel.InnerWheelHue");
    InnerWheelDropShadow    = FOdysseyStyle::GetBrush("AdvancedColorWheel.InnerWheelDropShadow");
    TriangleOverlay         = FOdysseyStyle::GetBrush("AdvancedColorWheel.TriangleOverlay");
    HintColorA              = FOdysseyStyle::GetBrush("AdvancedColorWheel.HintColorA");
    HintColorB              = FOdysseyStyle::GetBrush("AdvancedColorWheel.HintColorB");
    HueCursor               = FOdysseyStyle::GetBrush("AdvancedColorWheel.HueCursor");
    HueCursorBG             = FOdysseyStyle::GetBrush("AdvancedColorWheel.HueCursorBG");
    TriangleCursor          = FOdysseyStyle::GetBrush("AdvancedColorWheel.TriangleCursor");
    TriangleCursorBG        = FOdysseyStyle::GetBrush("AdvancedColorWheel.TriangleCursorBG");
    CursorOverlay           = FOdysseyStyle::GetBrush("AdvancedColorWheel.CursorOverlay");

    mColor = InArgs._Color;
    OnColorChangeCallback = InArgs._OnColorChange;

    triangle_buffer_size = FVector2D( 1, 1 );
    mEditMode = eEditMode::kNone;
    bMarkedAsInvalid = false;
    Init();

    // colorA = new ::ul3::FPixelValue( ULIS3_FORMAT_RGBA8 );
    // ::ul3::FPixelValue start_color = ::ul3::FPixelValue::FromRGBA8( 0, 0, 0, 255 );
    // SetColor( start_color );
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Public Callback API

void
SOdysseyAdvancedColorWheel::UpdateColor() const
{
    float u, v, w;
    ::ul3::FPixelValue hsv_color = ::ul3::Conv(mColor.Get(), ULIS3_FORMAT_HSVAF );
    float hue = hsv_color.HueF();
    float sat = hsv_color.SaturationF();
    float value = hsv_color.ValueF();

    u = value * (1.0f - sat);
    v = sat * value;

    w = 1 - ( u + v ); //Value min

    float sum = u + v + w;
    checkf( sum == 1.f, TEXT("Bad Conversion Occured") );

    hue_deg = hue * 360;
    hue_rad = -hue_deg * PI / 180.f;
    triangle_cursor_barycentric_position = FVector( u, v, w );

    UpdateGeometry();
    UpdateTint();
}


FVector2D
SOdysseyAdvancedColorWheel::GetInternalSize() const
{
    return  InternalSize;
}

FVector2D
SOdysseyAdvancedColorWheel::GetInternalPadding() const
{
    return decal;
}

bool
SOdysseyAdvancedColorWheel::IsFullyVisible() const
{
    return  clamp_shift.Y == 0;
}


float
SOdysseyAdvancedColorWheel::GetDrawRatio() const
{
    return  draw_ratio;
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Public SWidget overrides
int32
SOdysseyAdvancedColorWheel::OnPaint( const FPaintArgs& Args
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
        UpdateColor();
        PaintInternalBuffer();
        bMarkedAsInvalid = false;
        mDisplayedColor = mColor.Get();
    }

    // WheelBG
    FSlateDrawElement::MakeBox( OutDrawElements,
                                LayerId,
                                AllottedGeometry.ToPaintGeometry( decal, draw_size ),
                                WheelBG,
                                ESlateDrawEffect::NoPixelSnapping,
                                FLinearColor( 1, 1, 1, 1 ) );

    // InnerWheelBG
    FSlateDrawElement::MakeBox( OutDrawElements,
                                LayerId,
                                AllottedGeometry.ToPaintGeometry( decal, draw_size ),
                                InnerWheelBG,
                                ESlateDrawEffect::NoPixelSnapping,
                                lum_tint );

    // InnerWheelHue
    FSlateDrawElement::MakeBox( OutDrawElements,
                                LayerId,
                                AllottedGeometry.ToPaintGeometry( decal, draw_size ),
                                InnerWheelHue,
                                ESlateDrawEffect::NoPixelSnapping,
                                sat_tint );

    // InnerWheelDropShadow
    FSlateDrawElement::MakeBox( OutDrawElements,
                                LayerId,
                                AllottedGeometry.ToPaintGeometry( decal, draw_size ),
                                InnerWheelDropShadow,
                                ESlateDrawEffect::NoPixelSnapping,
                                FLinearColor( 1, 1, 1, 1 ) );

    // HintColorA
    FSlateDrawElement::MakeBox( OutDrawElements,
                                LayerId,
                                AllottedGeometry.ToPaintGeometry( decal + HINT_COLOR_A_LOC_RATIO * draw_size, HintColorA->GetImageSize() * draw_ratio ),
                                HintColorA,
                                ESlateDrawEffect::NoPixelSnapping,
                                result_tint );

    // HintColorB
    FSlateDrawElement::MakeBox( OutDrawElements,
                                LayerId,
                                AllottedGeometry.ToPaintGeometry( decal + HINT_COLOR_B_LOC_RATIO * draw_size, HintColorB->GetImageSize() * draw_ratio ),
                                HintColorB,
                                ESlateDrawEffect::NoPixelSnapping,
                                FLinearColor( 1, 1, 1, 1 ) );

    // CursorOverlay
    FSlateDrawElement::MakeBox( OutDrawElements,
                                LayerId,
                                AllottedGeometry.ToPaintGeometry(   CursorOverlay->GetImageSize() * draw_ratio,
                                                                    FSlateLayoutTransform(),
                                                                    FSlateRenderTransform( FQuat2D( hue_rad ), cursor_overlay_position ),
                                                                    FVector2D( 0.f, 0.5f ) ),
                                CursorOverlay,
                                ESlateDrawEffect::NoPixelSnapping,
                                hue_tint );

    // ItemBrush Triangle
    FSlateDrawElement::MakeBox( OutDrawElements,
                                LayerId +1,
                                AllottedGeometry.ToPaintGeometry( decal + TRIANGLE_LOC_RATIO * draw_size, triangle_buffer_size ),
                                ItemBrush.Get(),
                                ESlateDrawEffect::NoPixelSnapping,
                                FLinearColor( 1, 1, 1, 1 ) );
    // TriangleOverlay
    FSlateDrawElement::MakeBox( OutDrawElements,
                                LayerId +1,
                                AllottedGeometry.ToPaintGeometry( decal, TriangleOverlay->GetImageSize() * draw_ratio ),
                                TriangleOverlay,
                                ESlateDrawEffect::NoPixelSnapping,
                                FLinearColor( 1, 1, 1, 1 ) );

    // HueCursor
    FSlateDrawElement::MakeBox( OutDrawElements,
                                LayerId +1,
                                AllottedGeometry.ToPaintGeometry( hue_cursor_position, HueCursor->GetImageSize() * draw_ratio ),
                                HueCursorBG,
                                ESlateDrawEffect::NoPixelSnapping,
                                hue_tint );

    FSlateDrawElement::MakeBox( OutDrawElements,
                                LayerId +1,
                                AllottedGeometry.ToPaintGeometry( hue_cursor_position, HueCursor->GetImageSize() * draw_ratio ),
                                HueCursor,
                                ESlateDrawEffect::NoPixelSnapping,
                                FLinearColor( 1, 1, 1, 1 ) );

    // TriangleCursor
    FSlateDrawElement::MakeBox( OutDrawElements,
                                LayerId +1,
                                AllottedGeometry.ToPaintGeometry( triangle_cursor_cartesian_position, TriangleCursor->GetImageSize() * draw_ratio ),
                                TriangleCursorBG,
                                ESlateDrawEffect::NoPixelSnapping,
                                result_tint );

    FSlateDrawElement::MakeBox( OutDrawElements,
                                LayerId +1,
                                AllottedGeometry.ToPaintGeometry( triangle_cursor_cartesian_position, TriangleCursor->GetImageSize() * draw_ratio ),
                                TriangleCursor,
                                ESlateDrawEffect::NoPixelSnapping,
                                FLinearColor( 1, 1, 1, 1 ) );

    return LayerId;
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Buffer Utilities
void
SOdysseyAdvancedColorWheel::OnResizeEvent( const FVector2D& iNewSize ) const
{
    tSuperClass::OnResizeEvent( iNewSize );
    // Ensure aspect ratio on minimal size
    float minsize = FMath::Min( InternalSize.X, InternalSize.Y );
    InternalSize.X = minsize;
    InternalSize.Y = minsize;
    UpdateGeometry();
}

void
SOdysseyAdvancedColorWheel::InitInternalBuffers() const
{
    surface = MakeUnique< FOdysseySurfaceEditable >( triangle_buffer_size.X, triangle_buffer_size.Y );
    ItemBrush = MakeUnique< FSlateBrush >();
    ItemBrush->SetResourceObject( surface->Texture() );
    ItemBrush->ImageSize.X = surface->Width();
    ItemBrush->ImageSize.Y = surface->Height();
    ItemBrush->DrawAs = ESlateBrushDrawType::Image;
    bMarkedAsInvalid = true;
}

void
SOdysseyAdvancedColorWheel::PaintInternalBuffer( int iReason ) const
{
    PaintTriangle();
    surface->Invalidate();
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------- Painting Utilities
void
SOdysseyAdvancedColorWheel::PaintTriangle() const
{
    FVector2D Point1( 0, 0 );
    FVector2D Point2( triangle_buffer_size.X, triangle_buffer_size.Y / 2 );
    FVector2D Point3( 0, triangle_buffer_size.Y );

    // Global Triangle Area
    float triangleArea = triangle_buffer_size.X * triangle_buffer_size.Y * 0.5;

    // Bake base colors
    ::ul3::FPixelValue Color1 = ::ul3::FPixelValue::FromRGBA8( 255, 255, 255 );             // pure white
    ::ul3::FPixelValue Color2 = ::ul3::Conv( ::ul3::FPixelValue::FromHSVA8( static_cast< int >( hue_deg / 360.f * 255), 255, 255 ), ULIS3_FORMAT_RGBA8 ); // pure hue, max sat
    ::ul3::FPixelValue Color3 = ::ul3::FPixelValue::FromRGBA8( 0, 0, 0 );                   // pure black

    // Optimisation Constants
    float optconst1 = -triangle_buffer_size.Y / 2; // ( iPt2.Y - iPt3.Y )
    float optconst2 = triangle_buffer_size.Y; // ( iPt3.Y - iPt1.Y )
    float optconst3 = -triangle_buffer_size.X; // ( iPt3.X - iPt2.X )
    float optconst4 = -triangle_buffer_size.Y; // ( iPt1.Y - iPt3.Y )
    float optconst5 = optconst3 * optconst4; // ( iPt1.Y - iPt3.Y )

    // Funks
    auto InTriangle = [&]( const FVector2D& iPt1, const FVector2D& iPt2, const FVector2D& iPt3, const FVector2D& iPtX ) {
        float var0 = iPtX.X - iPt3.X;
        float var1 = iPtX.Y - iPt3.Y;
        float d = optconst5;
        float a = ( optconst1 * var0 + optconst3 * var1 ) / optconst5;
        float b = ( optconst2 * var0 ) / optconst5;
        float c = 1 - a -b;
        return 0 <= a && a <= 1 && 0 <= b && b <= 1 && 0 <= c && c <= 1;
    };

    auto ComputeArea = []( const FVector2D& iPt1, const FVector2D& iPt2, const FVector2D& iPt3 ) {
        return 0.5f * FMath::Abs( iPt1.X * ( iPt2.Y - iPt3.Y ) + iPt2.X * ( iPt3.Y - iPt1.Y ) + iPt3.X * ( iPt1.Y - iPt2.Y ) );
    };

    FVector2D current;
    const float maxy = static_cast< int >( Point3.Y );
    const float maxx = static_cast< int >( Point2.X );
    for( current.Y = Point1.Y; current.Y < maxy; ++current.Y ) {
        for( current.X = Point1.X; current.X < maxx; ++current.X ) {
            if( !InTriangle( Point1, Point2, Point3, current ) )
                break;

            float Area1 = ComputeArea( Point2, Point3, current );
            float Area2 = ComputeArea( Point3, Point1, current );
            float Area3 = ComputeArea( Point1, Point2, current );
            int r  = ( Area1 * Color1.Red8()   + Area2 * Color2.Red8()   + Area3 * Color3.Red8()   ) / triangleArea;
            int g  = ( Area1 * Color1.Green8() + Area2 * Color2.Green8() + Area3 * Color3.Green8() ) / triangleArea;
            int b  = ( Area1 * Color1.Blue8()  + Area2 * Color2.Blue8()  + Area3 * Color3.Blue8()  ) / triangleArea;
            uint8* pixel = surface->Block()->GetBlock()->PixelPtr( current.X, current.Y );
            pixel[2] = r;
            pixel[1] = g;
            pixel[0] = b;
            pixel[3] = 255;
        }
    }
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------- Event
FReply
SOdysseyAdvancedColorWheel::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    mEditMode = eEditMode::kNone;
    StartProcessMouseAction( MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ) - clamp_shift );
    OnColorChangeCallback.ExecuteIfBound( eOdysseyEventState::kStart, GetColorResult() );
    return FReply::Handled().CaptureMouse(SharedThis(this));
}


FReply
SOdysseyAdvancedColorWheel::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    mEditMode = eEditMode::kNone;
    if( HasMouseCapture() )
    {
        OnColorChangeCallback.ExecuteIfBound( eOdysseyEventState::kSet, GetColorResult() );
        return FReply::Handled().ReleaseMouseCapture();
    }

    return FReply::Unhandled();
}


FReply
SOdysseyAdvancedColorWheel::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if( !HasMouseCapture() )
        return FReply::Unhandled();

    ProcessMouseAction( MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ) - clamp_shift );
    OnColorChangeCallback.ExecuteIfBound( eOdysseyEventState::kAdjust, GetColorResult() );

    return FReply::Handled();
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------- Private Event Handling
void
SOdysseyAdvancedColorWheel::StartProcessMouseAction( FVector2D iPos )
{
    if( IsInHue( iPos ) )       mEditMode = eEditMode::kEditHue;
    if( IsInTriangle( iPos ) )  mEditMode = eEditMode::kEditTriangle;
    ProcessMouseAction( iPos );
}


void
SOdysseyAdvancedColorWheel::ProcessMouseAction( FVector2D iPos )
{
    switch( mEditMode )
    {
        case eEditMode::kEditHue:
            ProcessEditHueAction( iPos );
            break;

        case eEditMode::kEditTriangle:
            ProcessEditTriangleAction( iPos );
            break;
    }
}


void
SOdysseyAdvancedColorWheel::ProcessEditHueAction( FVector2D iPos )
{
    FVector2D delta = iPos - external_center;
    hue_rad = FMath::Atan2( delta.Y, delta.X );
    float shifted_hue = -hue_rad;
    if( shifted_hue < 0 ) shifted_hue+= 2 * PI;
    hue_deg = shifted_hue * 180 / PI;
}


void
SOdysseyAdvancedColorWheel::ProcessEditTriangleAction( FVector2D iPos )
{
    FVector barycentric = GetBarycentricCoordinates( triangle_Point1, triangle_Point2, triangle_Point3, iPos );
    triangle_cursor_barycentric_position = ClampBarycentricCoordinates(barycentric);
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------- Internal Geometry & Color Computing
bool
SOdysseyAdvancedColorWheel::IsInHue( FVector2D iPos )
{
    float dist = FVector2D::Distance( external_center, iPos );
    return dist < draw_ratio * OUTER_RADIUS && dist > draw_ratio * FINAL_RADIUS;
}


bool
SOdysseyAdvancedColorWheel::IsInTriangle( FVector2D iPos )
{
    float d = ( ( triangle_Point2.Y - triangle_Point3.Y ) * ( triangle_Point1.X - triangle_Point3.X ) + ( triangle_Point3.X - triangle_Point2.X ) * ( triangle_Point1.Y - triangle_Point3.Y ) );
    float a = ( ( triangle_Point2.Y - triangle_Point3.Y ) * ( iPos.X - triangle_Point3.X ) + ( triangle_Point3.X - triangle_Point2.X ) * ( iPos.Y - triangle_Point3.Y ) ) / d;
    float b = ( ( triangle_Point3.Y - triangle_Point1.Y ) * ( iPos.X - triangle_Point3.X ) + ( triangle_Point1.X - triangle_Point3.X ) * ( iPos.Y - triangle_Point3.Y ) ) / d;
    float c = 1 - a -b;
    bool in_triangle = 0 <= a && a <= 1 && 0 <= b && b <= 1 && 0 <= c && c <= 1;
    float dist = FVector2D::Distance( external_center, iPos );
    bool in_final_radius = dist < draw_ratio * FINAL_RADIUS;
    return  in_triangle || in_final_radius;
}


void
SOdysseyAdvancedColorWheel::UpdateGeometry() const
{
    external_center = ExternalSize / 2;
    internal_center = InternalSize / 2;
    decal = external_center - internal_center;

    draw_size = InternalSize;
    draw_ratio = draw_size.X / BASE_SIZE;

    triangle_Point1 = external_center + FVector2D( -COS2PIs3, -SIN2PIs3 ) * INNER_RADIUS * draw_ratio;
    triangle_Point2 = external_center + FVector2D( 1, 0 ) * INNER_RADIUS * draw_ratio;
    triangle_Point3 = external_center + FVector2D( -COS2PIs3, SIN2PIs3 ) * INNER_RADIUS * draw_ratio;
    triangle_buffer_size = FVector2D( triangle_Point2.X - triangle_Point1.X, triangle_Point3.Y - triangle_Point1.Y );

    hue_cursor_direction = FVector2D( FMath::Cos( hue_rad ), FMath::Sin( hue_rad ) );
    hue_cursor_position = external_center + ( hue_cursor_direction * MIDDLE_RADIUS - HueCursor->GetImageSize() / 2 ) * draw_ratio;
    triangle_cursor_cartesian_position = BarycentricToCartesianCoordinates( triangle_Point1, triangle_Point2, triangle_Point3, triangle_cursor_barycentric_position ) - ( TriangleCursor->GetImageSize() / 2 ) * draw_ratio;
    cursor_overlay_position = external_center - FVector2D( 0, CursorOverlay->GetImageSize().Y * 0.5f ) * draw_ratio;

    clamp_shift = -decal;
    clamp_shift.X = 0;
    clamp_shift.Y = FMath::Max( 0.f, clamp_shift.Y );
    decal += clamp_shift;
    hue_cursor_position += clamp_shift;
    triangle_cursor_cartesian_position += clamp_shift;
    cursor_overlay_position += clamp_shift;
}

void
SOdysseyAdvancedColorWheel::UpdateTint() const
{
    ::ul3::FPixelValue color = ::ul3::Conv( mColor.Get(), ULIS3_FORMAT_RGBA8 );
    result_tint = FLinearColor( FColor( color.Red8(), color.Green8(), color.Blue8() ) );
    ::ul3::FPixelValue hsv_tint = ::ul3::Conv( ::ul3::FPixelValue::FromHSVA8( static_cast< int >( hue_deg / 360.f * 255), 255, 255 ), ULIS3_FORMAT_RGBA8 );
    hue_tint = FLinearColor( FColor( hsv_tint.Red8(), hsv_tint.Green8(), hsv_tint.Blue8() ) );
    ::ul3::FPixelValue HSVColor = ::ul3::Conv( color, ULIS3_FORMAT_HSVA8 );
    sat_tint = FLinearColor( FColor( HSVColor.Value8(), HSVColor.Value8(), HSVColor.Value8(), HSVColor.Saturation8() ) );
    lum_tint = FLinearColor( FColor( HSVColor.Value8(), HSVColor.Value8(), HSVColor.Value8(), 255 ) );
}


::ul3::FPixelValue
SOdysseyAdvancedColorWheel::GetColorResult() const
{
    // Bake tints
    float u = triangle_cursor_barycentric_position.X;
    float v = triangle_cursor_barycentric_position.Y;
    float w = triangle_cursor_barycentric_position.Z;

    float hue = hue_deg / 360.f;
    float value = 1 - w;
    float sat = value != 0 ? v / value : 0.0f;

    return ::ul3::FPixelValue::FromHSVAF( hue, sat, value, 1.0f );
}

#undef LOCTEXT_NAMESPACE

