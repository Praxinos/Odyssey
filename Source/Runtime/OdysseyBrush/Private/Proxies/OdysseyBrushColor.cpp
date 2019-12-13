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
    col.m = ::ULIS::CColor( 255, 0, 0 );
    return  col;
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Make
//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromRGB( int R, int G, int B, int A )
{
    FOdysseyBrushColor col;
    col.m = ::ULIS::CColor::FromRGB( R, G, B, A );
    return  col;
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromHSV( int H, int S, int V, int A )
{
    FOdysseyBrushColor col;
    col.m = ::ULIS::CColor::FromHSV( H, S, V, A );
    return  col;
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromHSL( int H, int S, int L, int A )
{
    FOdysseyBrushColor col;
    col.m = ::ULIS::CColor::FromHSL( H, S, L, A );
    return  col;
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromCMYK( int C, int M, int Y, int K, int A )
{
    FOdysseyBrushColor col;
    col.m = ::ULIS::CColor::FromCMYK( C, M, Y, K );
    return  col;
}



//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Make F
//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromRGBF( float R, float G, float B, float A )
{
    FOdysseyBrushColor col;
    col.m = ::ULIS::CColor::FromRGBF( R, G, B, A );
    return  col;
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromHSVF( float H, float S, float V, float A )
{
    FOdysseyBrushColor col;
    col.m = ::ULIS::CColor::FromHSVF( H, S, V, A );
    return  col;
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromHSLF( float H, float S, float L, float A )
{
    FOdysseyBrushColor col;
    col.m = ::ULIS::CColor::FromHSLF( H, S, L, A );
    return  col;
}


//static
FOdysseyBrushColor
UOdysseyBrushColorFunctionLibrary::MakeOdysseyBrushColorFromCMYKF( float C, float M, float Y, float K, float A )
{
    FOdysseyBrushColor col;
    col.m = ::ULIS::CColor::FromCMYKF( C, M, Y, K );
    return  col;
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------- Break
//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoRGB( const  FOdysseyBrushColor& Color, int& R, int& G, int& B, int& A )
{
    FOdysseyBrushColor col;
    col.m = Color.m.ToRGB();
    R = col.m.Red();
    G = col.m.Green();
    B = col.m.Blue();
    A = col.m.Alpha();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoHSV( const  FOdysseyBrushColor& Color, int& H, int& S, int& V, int& A )
{
    FOdysseyBrushColor col;
    col.m = Color.m.ToHSV();
    H = col.m.HSVHue();
    S = col.m.HSVSaturation();
    V = col.m.Value();
    A = col.m.Alpha();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoHSL( const  FOdysseyBrushColor& Color, int& H, int& S, int& L, int& A )
{
    FOdysseyBrushColor col;
    col.m = Color.m.ToHSL();
    H = col.m.HSLHue();
    S = col.m.HSLSaturation();
    L = col.m.Lightness();
    A = col.m.Alpha();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoCMYK( const  FOdysseyBrushColor& Color, int& C, int& M, int& Y, int& K, int& A )
{
    FOdysseyBrushColor col;
    col.m = Color.m.ToCMYK();
    C = col.m.Cyan();
    M = col.m.Magenta();
    Y = col.m.Yellow();
    K = col.m.Key();
    A = col.m.Alpha();
}



//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Break F
//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoRGBF( const  FOdysseyBrushColor& Color, float& R, float& G, float& B, float& A )
{
    FOdysseyBrushColor col;
    col.m = Color.m.ToRGB();
    R = col.m.RedF();
    G = col.m.GreenF();
    B = col.m.BlueF();
    A = col.m.AlphaF();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoHSVF( const  FOdysseyBrushColor& Color, float& H, float& S, float& V, float& A )
{
    FOdysseyBrushColor col;
    col.m = Color.m.ToHSV();
    H = col.m.HSVHueF();
    S = col.m.HSVSaturationF();
    V = col.m.ValueF();
    A = col.m.AlphaF();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoHSLF( const  FOdysseyBrushColor& Color, float& H, float& S, float& L, float& A )
{
    FOdysseyBrushColor col;
    col.m = Color.m.ToHSL();
    H = col.m.HSLHueF();
    S = col.m.HSLSaturationF();
    L = col.m.LightnessF();
    A = col.m.AlphaF();
}


//static
void
UOdysseyBrushColorFunctionLibrary::BreakOdysseyBrushColorIntoCMYKF( const  FOdysseyBrushColor& Color, float& C, float& M, float& Y, float& K, float& A )
{
    FOdysseyBrushColor col;
    col.m = Color.m.ToCMYK();
    C = col.m.CyanF();
    M = col.m.MagentaF();
    Y = col.m.YellowF();
    K = col.m.KeyF();
    A = col.m.AlphaF();
}


#undef LOCTEXT_NAMESPACE
