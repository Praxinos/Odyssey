// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
#include "Proxies/OdysseyBrushColor.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushColor"
/////////////////////////////////////////////////////
// Odyssey Brush Color

FOdysseyBrushColor::FOdysseyBrushColor()
    : m( ULIS3_FORMAT_RGBA8 )
{
}

FOdysseyBrushColor::FOdysseyBrushColor( const ::ul3::FPixelValue& iVal )
    : m( iVal )
{
}

FOdysseyBrushColor::FOdysseyBrushColor( ::ul3::FPixelValue&& iVal )
    : m( std::move( iVal ) )
{
}

FOdysseyBrushColor::FOdysseyBrushColor( const FOdysseyBrushColor& iOther )
    : m( iOther.m )
{
}

FOdysseyBrushColor::FOdysseyBrushColor( FOdysseyBrushColor&& iOther )
    : m( std::move( iOther.m ) )
{
}

FOdysseyBrushColor&
FOdysseyBrushColor::operator=( const FOdysseyBrushColor& iOther ) {
    m = iOther.m;
    return  *this;
}

void
FOdysseyBrushColor::SetValue( const ::ul3::FPixelValue& iVal ) {
    m = iVal;
}

const ::ul3::FPixelValue&
FOdysseyBrushColor::GetValue() const {
    return  m;
}

//static
FOdysseyBrushColor
FOdysseyBrushColor::FromTemp( ::ul3::FPixelValue&& iVal ) {
    return  FOdysseyBrushColor( std::move( iVal ) );
}

//////////////////////////////////////////////////////////////////////////
// UOdysseyBrushFunctionLibrary
UOdysseyBrushColorFunctionLibrary::UOdysseyBrushColorFunctionLibrary( const  FObjectInitializer&  ObjectInitializer )
    : Super( ObjectInitializer )
{
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeDebugColor()
{
    return  FOdysseyBrushColor( ::ul3::FPixelValue::FromRGBA8( 255, 0, 0, 255 ) );
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Make

//static
EOdysseyColorModel
UOdysseyBrushColorFunctionLibrary::GetColorModel(FOdysseyBrushColor Color)
{
    return OdysseyColorModelFromULISFormat(Color.GetValue().Format());
}

//static
EOdysseyChannelDepth
UOdysseyBrushColorFunctionLibrary::GetChannelDepth(FOdysseyBrushColor Color)
{
    return OdysseyChannelDepthFromULISFormat(Color.GetValue().Format());
}

//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::SetAlpha(FOdysseyBrushColor Color, float Alpha)
{
	::ul3::FPixelValue value = Color.GetValue();
	value.SetAlphaF(Alpha);
    return value;
}

//static
float
UOdysseyBrushColorFunctionLibrary::GetAlpha(FOdysseyBrushColor Color)
{
    return Color.GetValue().AlphaF();
}

//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::Lerp(FOdysseyBrushColor Color1, FOdysseyBrushColor Color2, float Value, EOdysseyColorModel ColorModel, EOdysseyChannelDepth ChannelDepth)
{
	::ul3::tFormat format = ULISFormatFromModelAndDepth(ColorModel, ChannelDepth);

	FOdysseyBrushColor result;
	::ul3::FPixelValue v1 = ::ul3::Conv(Color1.GetValue(), format);
	::ul3::FPixelValue v2 = ::ul3::Conv(Color2.GetValue(), format);
	::ul3::FPixelValue r = ::ul3::MixNative(Value, &v1, &v2);
	result.SetValue(r);
	return result;
}

//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::ConvertToFormat(FOdysseyBrushColor Color, EOdysseyColorModel ColorModel, EOdysseyChannelDepth ChannelDepth)
{
	::ul3::tFormat format = ULISFormatFromModelAndDepth(ColorModel, ChannelDepth);
    ::ul3::FPixelValue result = ::ul3::Conv( Color.GetValue(), format );
    return result;
}

//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromRGB( int R, int G, int B, int A )
{
    return  FOdysseyBrushColor::FromTemp( ::ul3::FPixelValue::FromRGBA8( R, G, B, A ) );
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromGrey( int Grey, int A )
{
    return  FOdysseyBrushColor::FromTemp( ::ul3::FPixelValue::FromGreyA8( Grey, A ) );
}
 

//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromHSV( int H, int S, int V, int A )
{
    return  FOdysseyBrushColor::FromTemp( ::ul3::FPixelValue::FromHSVA8( H, S, V, A ) );
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromHSL( int H, int S, int L, int A )
{
    return  FOdysseyBrushColor::FromTemp( ::ul3::FPixelValue::FromHSLA8( H, S, L, A ) );
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromCMYK( int C, int M, int Y, int K, int A )
{
    return  FOdysseyBrushColor::FromTemp( ::ul3::FPixelValue::FromCMYKA8( C, M, Y, K, A ) );
}

//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromLab(int L, int A, int B, int Alpha)
{
	return  FOdysseyBrushColor::FromTemp(::ul3::FPixelValue::FromLabA8(L, A, B, Alpha));
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Make F
//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromRGBF( float R, float G, float B, float A )
{
    return  FOdysseyBrushColor::FromTemp( ::ul3::FPixelValue::FromRGBAF( R, G, B, A ) );
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromGreyF( float Grey, float A )
{
    return  FOdysseyBrushColor::FromTemp( ::ul3::FPixelValue::FromGreyAF( Grey, A ) );
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromHSVF( float H, float S, float V, float A )
{
    return  FOdysseyBrushColor::FromTemp( ::ul3::FPixelValue::FromHSVAF( H, S, V, A ) );
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromHSLF( float H, float S, float L, float A )
{
    return  FOdysseyBrushColor::FromTemp( ::ul3::FPixelValue::FromHSLAF( H, S, L, A ) );
}

//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromCMYKF( float C, float M, float Y, float K, float A )
{
    return  FOdysseyBrushColor::FromTemp( ::ul3::FPixelValue::FromCMYKAF( C, M, Y, K, A ) );
}

//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromLabF( float L, float A, float B, float Alpha )
{
    return  FOdysseyBrushColor::FromTemp( ::ul3::FPixelValue::FromLabAF( L, A, B, Alpha ) );
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------- Break
//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoRGB( const  FOdysseyBrushColor& Color, int& R, int& G, int& B, int& A )
{
    ::ul3::FPixelValue conv = ::ul3::Conv( Color.GetValue(), ULIS3_FORMAT_RGBA8 );
    R = conv.R8();
    G = conv.G8();
    B = conv.B8();
    A = conv.A8();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoGrey( const  FOdysseyBrushColor& Color, int& Grey, int& A )
{
    ::ul3::FPixelValue conv = ::ul3::Conv( Color.GetValue(), ULIS3_FORMAT_GA8 );
    Grey = conv.Grey8();
    A = conv.A8();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoHSV( const  FOdysseyBrushColor& Color, int& H, int& S, int& V, int& A )
{
    ::ul3::FPixelValue conv = ::ul3::Conv( Color.GetValue(), ULIS3_FORMAT_HSVA8 );
    H = conv.Hue8();
    S = conv.Saturation8();
    V = conv.Value8();
    A = conv.A8();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoHSL( const  FOdysseyBrushColor& Color, int& H, int& S, int& L, int& A )
{
    ::ul3::FPixelValue conv = ::ul3::Conv( Color.GetValue(), ULIS3_FORMAT_HSLA8 );
    H = conv.Hue8();
    S = conv.Saturation8();
    L = conv.Lightness8();
    A = conv.A8();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoCMYK( const  FOdysseyBrushColor& Color, int& C, int& M, int& Y, int& K, int& A )
{
    ::ul3::FPixelValue conv = ::ul3::Conv( Color.GetValue(), ULIS3_FORMAT_CMYKA8 );
    C = conv.Cyan8();
    M = conv.Magenta8();
    Y = conv.Yellow8();
    K = conv.Key8();
    A = conv.A8();
}

//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoLabA(const  FOdysseyBrushColor& Color, int& L, int& A, int& B, int& Alpha)
{
	::ul3::FPixelValue conv = ::ul3::Conv(Color.GetValue(), ULIS3_FORMAT_LabA8);
	L = conv.L8();
	A = conv.a8();
	B = conv.b8();
	Alpha = conv.A8();
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Break F
//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoRGBF( const  FOdysseyBrushColor& Color, float& R, float& G, float& B, float& A )
{
    ::ul3::FPixelValue conv = ::ul3::Conv( Color.GetValue(), ULIS3_FORMAT_RGBAF );
    R = conv.RF();
    G = conv.GF();
    B = conv.BF();
    A = conv.AF();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoGreyF( const  FOdysseyBrushColor& Color, float& Grey, float& A )
{
    ::ul3::FPixelValue conv = ::ul3::Conv( Color.GetValue(), ULIS3_FORMAT_GAF );
    Grey = conv.GreyF();
    A = conv.AF();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoHSVF( const  FOdysseyBrushColor& Color, float& H, float& S, float& V, float& A )
{
    ::ul3::FPixelValue conv = ::ul3::Conv( Color.GetValue(), ULIS3_FORMAT_HSVAF );
    H = conv.HueF();
    S = conv.SaturationF();
    V = conv.ValueF();
    A = conv.AF();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoHSLF( const  FOdysseyBrushColor& Color, float& H, float& S, float& L, float& A )
{
    ::ul3::FPixelValue conv = ::ul3::Conv( Color.GetValue(), ULIS3_FORMAT_HSLAF );
    H = conv.HueF();
    S = conv.SaturationF();
    L = conv.LightnessF();
    A = conv.AF();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoCMYKF( const  FOdysseyBrushColor& Color, float& C, float& M, float& Y, float& K, float& A )
{
    ::ul3::FPixelValue conv = ::ul3::Conv( Color.GetValue(), ULIS3_FORMAT_CMYKAF );
    C = conv.CyanF();
    M = conv.MagentaF();
    Y = conv.YellowF();
    K = conv.KeyF();
    A = conv.AF();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoLabF( const  FOdysseyBrushColor& Color, float& L, float& A, float& B, float& Alpha )
{
    ::ul3::FPixelValue conv = ::ul3::Conv( Color.GetValue(), ULIS3_FORMAT_LabAF );
    L = conv.LF();
    A = conv.aF();
    B = conv.bF();
    Alpha = conv.AF();
}


#undef LOCTEXT_NAMESPACE
