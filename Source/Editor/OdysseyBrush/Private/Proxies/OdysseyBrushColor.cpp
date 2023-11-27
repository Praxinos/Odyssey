// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
#include "Proxies/OdysseyBrushColor.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushColor"
/////////////////////////////////////////////////////
// Odyssey Brush Color

FOdysseyBrushColor::FOdysseyBrushColor()
    : m( ::ULIS::Format_RGBA8 )
{
}

FOdysseyBrushColor::FOdysseyBrushColor( const ::ULIS::FColor& iVal )
    : m( iVal )
{
}

FOdysseyBrushColor::FOdysseyBrushColor( ::ULIS::FColor&& iVal )
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
FOdysseyBrushColor::SetValue( const ::ULIS::FColor& iVal ) {
    m = iVal;
}

const ::ULIS::FColor&
FOdysseyBrushColor::GetValue() const {
    return  m;
}

//static
FOdysseyBrushColor
FOdysseyBrushColor::FromTemp( ::ULIS::FColor&& iVal ) {
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
    return  FOdysseyBrushColor( ::ULIS::FColor::FromRGBA8( 255, 0, 0, 255 ) );
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
    ::ULIS::FColor value = Color.GetValue();
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
    return  FOdysseyBrushColor(
        ::ULIS::FColor::MixFormat(
              Color1.GetValue()
            , Color2.GetValue()
            , ULISFormatFromModelAndDepth(
                  ColorModel
                , ChannelDepth
            )
            , Value
        )
    );
}

//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::ConvertToFormat(FOdysseyBrushColor Color, EOdysseyColorModel ColorModel, EOdysseyChannelDepth ChannelDepth)
{
    ::ULIS::eFormat format = ULISFormatFromModelAndDepth(ColorModel, ChannelDepth);
    ::ULIS::FColor result = Color.GetValue().ToFormat( format );
    return result;
}

//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromRGB( int R, int G, int B, int A )
{
    return  FOdysseyBrushColor::FromTemp( ::ULIS::FColor::FromRGBA8( R, G, B, A ) );
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromGrey( int Grey, int A )
{
    return  FOdysseyBrushColor::FromTemp( ::ULIS::FColor::FromGreyA8( Grey, A ) );
}
 

//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromHSV( int H, int S, int V, int A )
{
    return  FOdysseyBrushColor::FromTemp( ::ULIS::FColor::FromHSVA8( H, S, V, A ) );
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromHSL( int H, int S, int L, int A )
{
    return  FOdysseyBrushColor::FromTemp( ::ULIS::FColor::FromHSLA8( H, S, L, A ) );
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromCMYK( int C, int M, int Y, int K, int A )
{
    return  FOdysseyBrushColor::FromTemp( ::ULIS::FColor::FromCMYKA8( C, M, Y, K, A ) );
}

//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromLab(int L, int A, int B, int Alpha)
{
    return  FOdysseyBrushColor::FromTemp(::ULIS::FColor::FromLabA8(L, A, B, Alpha));
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Make F
//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromRGBF( float R, float G, float B, float A )
{
    return  FOdysseyBrushColor::FromTemp( ::ULIS::FColor::FromRGBAF( R, G, B, A ) );
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromGreyF( float Grey, float A )
{
    return  FOdysseyBrushColor::FromTemp( ::ULIS::FColor::FromGreyAF( Grey, A ) );
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromHSVF( float H, float S, float V, float A )
{
    return  FOdysseyBrushColor::FromTemp( ::ULIS::FColor::FromHSVAF( H, S, V, A ) );
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromHSLF( float H, float S, float L, float A )
{
    return  FOdysseyBrushColor::FromTemp( ::ULIS::FColor::FromHSLAF( H, S, L, A ) );
}

//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromCMYKF( float C, float M, float Y, float K, float A )
{
    return  FOdysseyBrushColor::FromTemp( ::ULIS::FColor::FromCMYKAF( C, M, Y, K, A ) );
}

//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromLabF( float L, float A, float B, float Alpha )
{
    return  FOdysseyBrushColor::FromTemp( ::ULIS::FColor::FromLabAF( L, A, B, Alpha ) );
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------- Break
//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoRGB( const  FOdysseyBrushColor& Color, int& R, int& G, int& B, int& A )
{
    ::ULIS::FColor conv = Color.GetValue().ToFormat( ::ULIS::Format_RGBA8 );
    R = conv.R8();
    G = conv.G8();
    B = conv.B8();
    A = conv.A8();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoGrey( const  FOdysseyBrushColor& Color, int& Grey, int& A )
{
    ::ULIS::FColor conv = Color.GetValue().ToFormat( ::ULIS::Format_GA8 );
    Grey = conv.Grey8();
    A = conv.A8();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoHSV( const  FOdysseyBrushColor& Color, int& H, int& S, int& V, int& A )
{
    ::ULIS::FColor conv = Color.GetValue().ToFormat( ::ULIS::Format_HSVA8 );
    H = conv.Hue8();
    S = conv.Saturation8();
    V = conv.Value8();
    A = conv.A8();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoHSL( const  FOdysseyBrushColor& Color, int& H, int& S, int& L, int& A )
{
    ::ULIS::FColor conv = Color.GetValue().ToFormat( ::ULIS::Format_HSLA8 );
    H = conv.Hue8();
    S = conv.Saturation8();
    L = conv.Lightness8();
    A = conv.A8();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoCMYK( const  FOdysseyBrushColor& Color, int& C, int& M, int& Y, int& K, int& A )
{
    ::ULIS::FColor conv = Color.GetValue().ToFormat( ::ULIS::Format_CMYKA8 );
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
    ::ULIS::FColor conv = Color.GetValue().ToFormat( ::ULIS::Format_LabA8 );
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
    ::ULIS::FColor conv = Color.GetValue().ToFormat( ::ULIS::Format_RGBAF );
    R = conv.RF();
    G = conv.GF();
    B = conv.BF();
    A = conv.AF();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoGreyF( const  FOdysseyBrushColor& Color, float& Grey, float& A )
{
    ::ULIS::FColor conv = Color.GetValue().ToFormat( ::ULIS::Format_GAF );
    Grey = conv.GreyF();
    A = conv.AF();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoHSVF( const  FOdysseyBrushColor& Color, float& H, float& S, float& V, float& A )
{
    ::ULIS::FColor conv = Color.GetValue().ToFormat( ::ULIS::Format_HSVAF );
    H = conv.HueF();
    S = conv.SaturationF();
    V = conv.ValueF();
    A = conv.AF();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoHSLF( const  FOdysseyBrushColor& Color, float& H, float& S, float& L, float& A )
{
    ::ULIS::FColor conv = Color.GetValue().ToFormat( ::ULIS::Format_HSLAF );
    H = conv.HueF();
    S = conv.SaturationF();
    L = conv.LightnessF();
    A = conv.AF();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoCMYKF( const  FOdysseyBrushColor& Color, float& C, float& M, float& Y, float& K, float& A )
{
    ::ULIS::FColor conv = Color.GetValue().ToFormat( ::ULIS::Format_CMYKAF );
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
    ::ULIS::FColor conv = Color.GetValue().ToFormat( ::ULIS::Format_LabAF );
    L = conv.LF();
    A = conv.aF();
    B = conv.bF();
    Alpha = conv.AF();
}


#undef LOCTEXT_NAMESPACE
