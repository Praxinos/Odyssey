// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
#include "Proxies/OdysseyBrushColor.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushColor"
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
    FOdysseyBrushColor col;
    col.SetValue( new ::ul3::FPixelValue( ULIS3_FORMAT_RGBA8, { 255, 0, 0, 0 } ) );
    return  col;
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Make
//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromRGB( int R, int G, int B, int A )
{
    FOdysseyBrushColor col;
    col.SetValue( new ::ul3::FPixelValue( ULIS3_FORMAT_RGBA8, { R, G, B, A } ) );
    return  col;
}
 

//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromHSV( int H, int S, int V, int A )
{
    FOdysseyBrushColor col;
    col.SetValue( new ::ul3::FPixelValue( ULIS3_FORMAT_HSVA8, { H, S, V, A } ) );
    return  col;
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromHSL( int H, int S, int L, int A )
{
    FOdysseyBrushColor col;
    col.SetValue( new ::ul3::FPixelValue( ULIS3_FORMAT_HSLA8, { H, S, L, A } ) );
    return  col;
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromCMYK( int C, int M, int Y, int K, int A )
{
    FOdysseyBrushColor col;
    col.SetValue( new ::ul3::FPixelValue( ULIS3_FORMAT_CMYKA8, { C, M, Y, K, A } ) );
    return  col;
}



//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Make F
//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromRGBF( float R, float G, float B, float A )
{
    FOdysseyBrushColor col;
    col.SetValue( new ::ul3::FPixelValue( ULIS3_FORMAT_RGBAF, { R, G, B, A } ) );
    return  col;
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromHSVF( float H, float S, float V, float A )
{
    FOdysseyBrushColor col;
    col.SetValue( new ::ul3::FPixelValue( ULIS3_FORMAT_HSVF, { H, S, V, A } ) );
    return  col;
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromHSLF( float H, float S, float L, float A )
{
    FOdysseyBrushColor col;
    col.SetValue( new ::ul3::FPixelValue( ULIS3_FORMAT_HSLF, { H, S, L, A } ) );
    return  col;
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromCMYKF( float C, float M, float Y, float K, float A )
{
    FOdysseyBrushColor col;
    col.SetValue( new ::ul3::FPixelValue( ULIS3_FORMAT_CMYKF, { C, M, Y, K, A } ) );
    return  col;
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


#undef LOCTEXT_NAMESPACE
