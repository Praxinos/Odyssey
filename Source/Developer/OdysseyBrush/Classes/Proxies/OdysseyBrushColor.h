// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
#pragma once
#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include <ULIS3>

#include "OdysseyBrushFormat.h"

#include "OdysseyBrushColor.generated.h"

/////////////////////////////////////////////////////
// Odyssey Brush Color
USTRUCT(BlueprintType, meta = (DisplayName = "Odyssey Color"))
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
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static FOdysseyBrushColor MakeDebugColor();

    /* Format Management */

    //Get the Odyssey Brush Color Color Model
    UFUNCTION(BlueprintPure, Category="Odyssey|Color", meta = ( DisplayName="Get OdysseyBrushColor Color Model" ))
    static EOdysseyColorModel GetColorModel(FOdysseyBrushColor Color);

    //Get the Odyssey Brush Color Channel Depth
    UFUNCTION(BlueprintPure, Category="Odyssey|Color", meta = ( DisplayName="Get OdysseyBrushColor Channel Depth" ))
    static EOdysseyChannelDepth GetChannelDepth(FOdysseyBrushColor Color);
    
    //Set the Odyssey Brush Color Alpha Component
    UFUNCTION(BlueprintPure, Category="Odyssey|Color", meta = ( DisplayName="Set OdysseyBrushColor Alpha" ))
    static FOdysseyBrushColor SetAlpha(FOdysseyBrushColor Color, float Alpha);

    //Get the Odyssey Brush Color Alpha Component
    UFUNCTION(BlueprintPure, Category="Odyssey|Color", meta = ( DisplayName="Get OdysseyBrushColor Alpha" ))
    static float GetAlpha(FOdysseyBrushColor Color);

	//Interpolates an OdysseyBrushColor between 2 colors
	UFUNCTION(BlueprintPure, Category = "Odyssey|Color", meta = (DisplayName = "Lerp (OdysseyBrushColor)"))
	static FOdysseyBrushColor Lerp(FOdysseyBrushColor Color1, FOdysseyBrushColor Color2, float Value, EOdysseyColorModel ColorModel = EOdysseyColorModel::kRGBA, EOdysseyChannelDepth ChannelDepth = EOdysseyChannelDepth::k8);

    //Convert the Odyssey Brush Color to the specified Color Model and Channel Depth
    UFUNCTION(BlueprintPure, Category="Odyssey|Color", meta = ( DisplayName="Convert OdysseyBrushColor to Format" ))
    static FOdysseyBrushColor ConvertToFormat(FOdysseyBrushColor Color, EOdysseyColorModel ColorModel = EOdysseyColorModel::kRGBA, EOdysseyChannelDepth ChannelDepth = EOdysseyChannelDepth::k8);

    /* Make */

    //Make Odyssey Brush color from RGBA values (Integers).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromRGB( int R, int G, int B, int A = 255 );

    //Make Odyssey Brush color from GreyA values (Integers).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromGrey( int Grey, int A = 255 );

    //Make Odyssey Brush color from HSVA values (Integers).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromHSV( int H, int S, int V, int A = 255 );

    //Make Odyssey Brush color from HSLA values (Integers).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromHSL( int H, int S, int L, int A = 255 );

    //Make Odyssey Brush color from CMYKA values (Integers).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromCMYK( int C, int M, int Y, int K, int A = 255 );

	//Make Odyssey Brush color from LabA values (Integers).
	UFUNCTION(BlueprintPure, Category = "Odyssey|Color")
	static FOdysseyBrushColor MakeOdysseyBrushColorFromLab(int L, int A, int B, int Alpha = 255);

    /* Make F */
    
    //Make Odyssey Brush color from RGBA values (Floats).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromRGBF( float R, float G, float B, float A = 1.f );

    //Make Odyssey Brush color from GreyA values (Floats).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromGreyF( float Grey, float A = 1.f );

    //Make Odyssey Brush color from HSVA values (Floats).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromHSVF( float H, float S, float V, float A = 1.f );

    //Make Odyssey Brush color from HSLA values (Floats).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromHSLF( float H, float S, float L, float A = 1.f );

    //Make Odyssey Brush color from CMYKA values (Floats).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromCMYKF( float C, float M, float Y, float K, float A = 1.f );

    //Make Odyssey Brush color from LAB Alpha values (Floats).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static FOdysseyBrushColor MakeOdysseyBrushColorFromLabF( float L, float A, float B, float Alpha = 255 );

    /* Break */
    
    //Breaks Odyssey Brush Color input into RGBA (Integers).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static void BreakOdysseyBrushColorIntoRGB( const  FOdysseyBrushColor& Color, int& R, int& G, int& B, int& A );

    //Breaks Odyssey Brush Color input into GreyA (Integers).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static void BreakOdysseyBrushColorIntoGrey( const  FOdysseyBrushColor& Color, int& Grey, int& A );

    //Breaks Odyssey Brush Color input into HSVA (Integers).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static void BreakOdysseyBrushColorIntoHSV( const  FOdysseyBrushColor& Color, int& H, int& S, int& V, int& A );

    //Breaks Odyssey Brush Color input into HSLA (Integers).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static void BreakOdysseyBrushColorIntoHSL( const  FOdysseyBrushColor& Color, int& H, int& S, int& L, int& A );

    //Breaks Odyssey Brush Color input into CMYKA (Integers).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static void BreakOdysseyBrushColorIntoCMYK( const  FOdysseyBrushColor& Color, int& C, int& M, int& Y, int& K, int& A );

	//Breaks Odyssey Brush Color input into LABA (Integers).
	UFUNCTION(BlueprintPure, Category = "Odyssey|Color")
	static void BreakOdysseyBrushColorIntoLabA(const  FOdysseyBrushColor& Color, int& L, int& A, int& B, int& Alpha);

    /* Break F */
    
    //Breaks Odyssey Brush Color input into RGBA (Floats).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static void BreakOdysseyBrushColorIntoRGBF( const  FOdysseyBrushColor& Color, float& R, float& G, float& B, float& A );
    
    //Breaks Odyssey Brush Color input into GreyA (Floats).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static void BreakOdysseyBrushColorIntoGreyF( const  FOdysseyBrushColor& Color, float& Grey, float& A );

    //Breaks Odyssey Brush Color input into HSVA (Floats).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static void BreakOdysseyBrushColorIntoHSVF( const  FOdysseyBrushColor& Color, float& H, float& S, float& V, float& A );

    //Breaks Odyssey Brush Color input into HSLA (Floats).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static void BreakOdysseyBrushColorIntoHSLF( const  FOdysseyBrushColor& Color, float& H, float& S, float& L, float& A );

    //Breaks Odyssey Brush Color input into CMYKA (Floats).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static void BreakOdysseyBrushColorIntoCMYKF( const FOdysseyBrushColor& Color, float& C, float& M, float& Y, float& K, float& A );

    //Breaks Odyssey Brush Color input into LAB Alpha (Floats).
    UFUNCTION(BlueprintPure, Category="Odyssey|Color")
    static void BreakOdysseyBrushColorIntoLabF( const  FOdysseyBrushColor& Color, float& L, float& A, float& B, float& Alpha );
};

