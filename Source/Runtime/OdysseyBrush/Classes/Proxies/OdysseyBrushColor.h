// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
#pragma once
#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include <ULIS_CCOLOR>
#include "OdysseyBrushColor.generated.h"

/////////////////////////////////////////////////////
// Odyssey Brush Color
USTRUCT(BlueprintType)
struct ODYSSEYBRUSH_API FOdysseyBrushColor
{
    GENERATED_BODY()

    FOdysseyBrushColor() : m( ::ULIS::CColor() ) {}

    ::ULIS::CColor  m;
};

/////////////////////////////////////////////////////
// UOdysseyBrushColorFunctionLibrary
UCLASS(meta=(ScriptName="OdysseyBrushColorLibrary"))
class ODYSSEYBRUSH_API UOdysseyBrushColorFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()

    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeDebugColor();

    /* Make */
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromRGB( int R, int G, int B, int A = 255 );

    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromHSV( int H, int S, int V, int A = 255 );

    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromHSL( int H, int S, int L, int A = 255 );

    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromCMYK( int C, int M, int Y, int K, int A = 255 );

    /* Make F */
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromRGBF( float R, float G, float B, float A = 1.f );

    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromHSVF( float H, float S, float V, float A = 1.f );

    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromHSLF( float H, float S, float L, float A = 1.f );

    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromCMYKF( float C, float M, float Y, float K, float A = 1.f );

    /* Break */
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoRGB( const  FOdysseyBrushColor& Color, int& R, int& G, int& B, int& A );

    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoHSV( const  FOdysseyBrushColor& Color, int& H, int& S, int& V, int& A );

    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoHSL( const  FOdysseyBrushColor& Color, int& H, int& S, int& L, int& A );

    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoCMYK( const  FOdysseyBrushColor& Color, int& C, int& M, int& Y, int& K, int& A );

    /* Break F */
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoRGBF( const  FOdysseyBrushColor& Color, float& R, float& G, float& B, float& A );

    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoHSVF( const  FOdysseyBrushColor& Color, float& H, float& S, float& V, float& A );

    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoHSLF( const  FOdysseyBrushColor& Color, float& H, float& S, float& L, float& A );

    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoCMYKF( const  FOdysseyBrushColor& Color, float& C, float& M, float& Y, float& K, float& A );
};

