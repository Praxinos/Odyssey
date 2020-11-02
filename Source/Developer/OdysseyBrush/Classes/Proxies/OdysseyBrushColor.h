// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
#pragma once
#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include <ULIS3>

#include "OdysseyBrushFormat.h"

#include "OdysseyBrushColor.generated.h"

/////////////////////////////////////////////////////
// Odyssey Brush Color
USTRUCT(BlueprintType)
struct ODYSSEYBRUSH_API FOdysseyBrushColor
{
public:
    GENERATED_BODY()

    FOdysseyBrushColor();
    FOdysseyBrushColor( const ::ul3::FPixelValue& iVal );
    FOdysseyBrushColor( ::ul3::FPixelValue&& iVal );
    FOdysseyBrushColor( const FOdysseyBrushColor& iOther );
    FOdysseyBrushColor( FOdysseyBrushColor&& iOther );
    FOdysseyBrushColor& operator=( const FOdysseyBrushColor& iOther );
    void SetValue( const ::ul3::FPixelValue& iVal );
    const ::ul3::FPixelValue& GetValue() const;
    static  FOdysseyBrushColor FromTemp( ::ul3::FPixelValue&& iVal );

private:
    ::ul3::FPixelValue  m;
};

/////////////////////////////////////////////////////
// UOdysseyBrushColorFunctionLibrary
UCLASS(meta=(ScriptName="OdysseyBrushColorLibrary"))
class ODYSSEYBRUSH_API UOdysseyBrushColorFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()

    //Debug node to generate a pure red color (RGB 255, 0, 0).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeDebugColor();

    /* Format Management */

    //Get the Odyssey Brush Color Pixel Format
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor", meta = ( DisplayName="Get OdysseyBrushColor Format" ))
    static EOdysseyPixelFormat GetFormat(FOdysseyBrushColor Color);

    //Get the Odyssey Brush Color Pixel Precision
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor", meta = ( DisplayName="Get OdysseyBrushColor Precision" ))
    static EOdysseyPixelFormatPrecision GetPrecision(FOdysseyBrushColor Color);

    /* Make */

    //Make Odyssey Brush color from RGBA values (Integers).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromRGB( int R, int G, int B, int A = 255 );

    //Make Odyssey Brush color from HSVA values (Integers).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromHSV( int H, int S, int V, int A = 255 );

    //Make Odyssey Brush color from HSLA values (Integers).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromHSL( int H, int S, int L, int A = 255 );

    //Make Odyssey Brush color from CMYKA values (Integers).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromCMYK( int C, int M, int Y, int K, int A = 255 );

    /* Make F */
    
    //Make Odyssey Brush color from RGBA values (Floats).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromRGBF( float R, float G, float B, float A = 1.f );

    //Make Odyssey Brush color from HSVA values (Floats).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromHSVF( float H, float S, float V, float A = 1.f );

    //Make Odyssey Brush color from HSLA values (Floats).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromHSLF( float H, float S, float L, float A = 1.f );

    //Make Odyssey Brush color from CMYKA values (Floats).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromCMYKF( float C, float M, float Y, float K, float A = 1.f );

    //Make Odyssey Brush color from LAB Alpha values (Floats).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromLabF( float L, float A, float B, float Alpha = 255 );

    /* Break */
    
    //Breaks Odyssey Brush Color input into RGBA (Integers).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoRGB( const  FOdysseyBrushColor& Color, int& R, int& G, int& B, int& A );

    //Breaks Odyssey Brush Color input into HSVA (Integers).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoHSV( const  FOdysseyBrushColor& Color, int& H, int& S, int& V, int& A );

    //Breaks Odyssey Brush Color input into HSLA (Integers).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoHSL( const  FOdysseyBrushColor& Color, int& H, int& S, int& L, int& A );

    //Breaks Odyssey Brush Color input into CMYKA (Integers).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoCMYK( const  FOdysseyBrushColor& Color, int& C, int& M, int& Y, int& K, int& A );

    /* Break F */
    
    //Breaks Odyssey Brush Color input into RGBA (Floats).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoRGBF( const  FOdysseyBrushColor& Color, float& R, float& G, float& B, float& A );

    //Breaks Odyssey Brush Color input into HSVA (Floats).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoHSVF( const  FOdysseyBrushColor& Color, float& H, float& S, float& V, float& A );

    //Breaks Odyssey Brush Color input into HSLA (Floats).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoHSLF( const  FOdysseyBrushColor& Color, float& H, float& S, float& L, float& A );

    //Breaks Odyssey Brush Color input into CMYKA (Floats).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoCMYKF( const FOdysseyBrushColor& Color, float& C, float& M, float& Y, float& K, float& A );

    //Breaks Odyssey Brush Color input into LAB Alpha (Floats).
    UFUNCTION(BlueprintPure, Category="OdysseyBrushColor")
    static void BreakOdysseyBrushColorIntoLabF( const  FOdysseyBrushColor& Color, float& L, float& A, float& B, float& Alpha );
};

