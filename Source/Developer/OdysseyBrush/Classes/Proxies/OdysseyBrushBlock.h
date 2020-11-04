// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include "Curves/CurveFloat.h"

#include "Proxies/OdysseyBrushColor.h"
#include "Proxies/OdysseyBrushFormat.h"
#include "Proxies/OdysseyBrushBlending.h"
#include "Proxies/OdysseyBrushRect.h"

#include "OdysseyBrushBlock.generated.h"

class  FOdysseyBlock;
class  UFont;
class  UOdysseyBrushAssetBase;

/////////////////////////////////////////////////////
// Odyssey Block Reference
USTRUCT(BlueprintType, meta = (DisplayName = "Odyssey Block Reference"))
struct ODYSSEYBRUSH_API FOdysseyBlockProxy
{
    GENERATED_BODY()

    FOdysseyBlockProxy()
        : m(        0       )
    {}

    FOdysseyBlockProxy( TSharedPtr<FOdysseyBlock> iBlock )
        : m(        iBlock  )
    {}

    static
    FOdysseyBlockProxy
    MakeNullProxy()
    {
        return  FOdysseyBlockProxy();
    }

    TSharedPtr<FOdysseyBlock>   m;
};


// Duplicate FFontCharacter (in Font.h#29) to be able to expose values in BP (as FFontCharacter is not tagged BlueprintType)
USTRUCT(BlueprintType)
struct FOdysseyFontCharacter
{
    GENERATED_BODY()

    UPROPERTY( BlueprintReadWrite )
    int32 StartU;

    UPROPERTY( BlueprintReadWrite )
    int32 StartV;

    UPROPERTY( BlueprintReadWrite )
    int32 USize;

    UPROPERTY( BlueprintReadWrite )
    int32 VSize;

    UPROPERTY( BlueprintReadWrite )
    uint8 TextureIndex;

    UPROPERTY( BlueprintReadWrite )
    int32 VerticalOffset;

    FOdysseyFontCharacter()
        : StartU( 0 )
        , StartV( 0 )
        , USize( 0 )
        , VSize( 0 )
        , TextureIndex( 0 )
        , VerticalOffset( 0 )
    {
    }
};

/////////////////////////////////////////////////////
// UOdysseyBlockProxyFunctionLibrary
UCLASS(meta=(ScriptName="OdysseyBlockProxyLibrary"))
class ODYSSEYBRUSH_API UOdysseyBlockProxyFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    //Converts Texture 2D to Odyssey Block Reference.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockReference"
             , meta = ( DefaultToSelf="BrushContext", DisplayName = "To Odyssey Block Reference (Texture2D)", BlueprintAutocast ) )
    static FOdysseyBlockProxy Conv_TextureToOdysseyBlockProxy( UTexture2D* Texture, EOdysseyPixelFormat Format, EOdysseyPixelFormatPrecision Precision, UOdysseyBrushAssetBase* BrushContext);

    //Convert the Odyssey Block Reference to the specified Format and Precision
    UFUNCTION(BlueprintPure, Category="OdysseyBlockReference", meta = ( DisplayName="Convert Block to Format" ))
    static FOdysseyBlockProxy ConvertToFormat(UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Block, EOdysseyPixelFormat Format, EOdysseyPixelFormatPrecision Precision);

    //Applies a color on sample's alpha channel. Requires an Odyssey Brush Color input.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockReference"
             , meta = ( DefaultToSelf="BrushContext", DeprecatedFunction, DeprecationMessage = "Use Fill" ) )
    static FOdysseyBlockProxy FillPreserveAlpha(  UOdysseyBrushAssetBase* BrushContext
                                                , FOdysseyBlockProxy Sample
                                                , FOdysseyBrushColor Color );

    //Blends a color on the whole given Sample. Requires an Odyssey Brush Color input.
	UFUNCTION(BlueprintPure
		, Category = "OdysseyBlockReference"
		, meta = (DefaultToSelf = "BrushContext", DisplayName = "Blend Block With Color"))
		static FOdysseyBlockProxy BlendColor(UOdysseyBrushAssetBase* BrushContext
			, FOdysseyBrushColor Color
			, FOdysseyBlockProxy Back
			, FOdysseyBrushRect Area
			, float Opacity = 1.f
			, EOdysseyPixelFormat Format = EOdysseyPixelFormat::kCanvasFormat
			, EOdysseyPixelFormatPrecision Precision = EOdysseyPixelFormatPrecision::kCanvasPrecision
			, EOdysseyBlendingMode BlendingMode = EOdysseyBlendingMode::kNormal
			, EOdysseyAlphaMode AlphaMode = EOdysseyAlphaMode::kNormal);

    //Fills the given Sample with thge given Color. Requires an Odyssey Brush Color input.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockReference"
             , meta = ( DefaultToSelf="BrushContext", DisplayName = "Fill Block With Color" ) )
    static FOdysseyBlockProxy Fill( UOdysseyBrushAssetBase* BrushContext
                                    , FOdysseyBlockProxy Block
                                    , FOdysseyBrushColor Color
                                    , FOdysseyBrushRect Area
                                    , bool PreserveAlpha = false
                                    , EOdysseyPixelFormat Format = EOdysseyPixelFormat::kCanvasFormat
                                    , EOdysseyPixelFormatPrecision Precision = EOdysseyPixelFormatPrecision::kCanvasPrecision);

    //Creates an empty Odyssey Block Reference.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockReference"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="InitializeData" ) )
    static FOdysseyBlockProxy CreateBlock( UOdysseyBrushAssetBase* BrushContext
                                         , int Width = 256
                                         , int Height = 256
                                         , EOdysseyPixelFormat Format = EOdysseyPixelFormat::kCanvasFormat
										 , EOdysseyPixelFormatPrecision Precision = EOdysseyPixelFormatPrecision::kCanvasPrecision
                                         , bool InitializeData = true );

    //Crops the Odyssey Block Reference according to the given Rect.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockReference"
             , meta = ( DefaultToSelf="BrushContext" ) )
    static FOdysseyBlockProxy CropBlock( UOdysseyBrushAssetBase* BrushContext
                                         , FOdysseyBlockProxy Block
                                         , FOdysseyBrushRect Area);

    //Blends two Odyssey Block Reference on Top and Back.
    //If Top is bigger than Back = Back will crop Top.
    //If Top is smaller than Back = Both will be visible.
    //X | Y are an offset to pan Top.
    UFUNCTION(BlueprintPure
             , Category="OdysseyBlockReference"
             , meta = ( DefaultToSelf="BrushContext", DisplayName = "Blend Blocks" ) )
    static FOdysseyBlockProxy Blend( UOdysseyBrushAssetBase* BrushContext
                                   , FOdysseyBlockProxy Top
                                   , FOdysseyBlockProxy Back
                                   , FOdysseyBrushRect TopArea
                                   , UPARAM(DisplayName="Top Offset X") int X = 0
                                   , UPARAM(DisplayName="Top Offset Y") int Y = 0
                                   , float Opacity = 1.f
								   , EOdysseyPixelFormat Format = EOdysseyPixelFormat::kCanvasFormat
								   , EOdysseyPixelFormatPrecision Precision = EOdysseyPixelFormatPrecision::kCanvasPrecision
                                   , EOdysseyBlendingMode BlendingMode = EOdysseyBlendingMode::kNormal
                                   , EOdysseyAlphaMode AlphaMode = EOdysseyAlphaMode::kNormal);

    //Adjust the alpha component of each pixel in the given Block according to the given curve
    //Abscissa represents the actual alpha value.
    //Ordinate represents the adjusted alpha value.
    UFUNCTION(BlueprintPure
             , Category="OdysseyBlockReference"
             , meta = ( DefaultToSelf="BrushContext", DisplayName = "Adjust Block Alpha" ) )
    static FOdysseyBlockProxy AdjustAlpha( FOdysseyBlockProxy Block
                                         , UCurveFloat* Curve
                                         , bool PreserveNullAlpha = true);

	//Adjust the RGBA components of each pixel in the given Block according to the given curves
	//Abscissa represents the actual component value.
	//Ordinate represents the adjusted component value.
	UFUNCTION(BlueprintPure
			, Category = "OdysseyBlockReference"
			, meta = (DefaultToSelf = "BrushContext", DisplayName = "Adjust Block RGBA"))
	static FOdysseyBlockProxy AdjustRGBA(FOdysseyBlockProxy Block
		, UCurveFloat* CurveR
		, UCurveFloat* CurveG
		, UCurveFloat* CurveB
		, UCurveFloat* CurveAlpha
		, bool PreserveNullAlpha = true);


	//Adjust the GreyA components of each pixel in the given Block according to the given curves
	//Abscissa represents the actual component value.
	//Ordinate represents the adjusted component value.
	UFUNCTION(BlueprintPure
		, Category = "OdysseyBlockReference"
		, meta = (DefaultToSelf = "BrushContext", DisplayName = "Adjust Block GreyA"))
		static FOdysseyBlockProxy AdjustGreyA(FOdysseyBlockProxy Block
			, UCurveFloat* CurveGrey
			, UCurveFloat* CurveAlpha
			, bool PreserveNullAlpha = true);


	//Adjust the HSVA components of each pixel in the given Block according to the given curves
	//Abscissa represents the actual component value.
	//Ordinate represents the adjusted component value.
	UFUNCTION(BlueprintPure
		, Category = "OdysseyBlockReference"
		, meta = (DefaultToSelf = "BrushContext", DisplayName = "Adjust Block HSVA"))
		static FOdysseyBlockProxy AdjustHSVA(FOdysseyBlockProxy Block
			, UCurveFloat* CurveH
			, UCurveFloat* CurveS
			, UCurveFloat* CurveV
			, UCurveFloat* CurveAlpha
			, bool PreserveNullAlpha = true);

	//Adjust the HSLA components of each pixel in the given Block according to the given curves
	//Abscissa represents the actual component value.
	//Ordinate represents the adjusted component value.
	UFUNCTION(BlueprintPure
		, Category = "OdysseyBlockReference"
		, meta = (DefaultToSelf = "BrushContext", DisplayName = "Adjust Block HSLA"))
		static FOdysseyBlockProxy AdjustHSLA(FOdysseyBlockProxy Block
			, UCurveFloat* CurveH
			, UCurveFloat* CurveS
			, UCurveFloat* CurveL
			, UCurveFloat* CurveAlpha
			, bool PreserveNullAlpha = true);

	//Adjust the CMYKA components of each pixel in the given Block according to the given curves
	//Abscissa represents the actual component value.
	//Ordinate represents the adjusted component value.
	UFUNCTION(BlueprintPure
		, Category = "OdysseyBlockReference"
		, meta = (DefaultToSelf = "BrushContext", DisplayName = "Adjust Block CMYKA"))
		static FOdysseyBlockProxy AdjustCMYKA(FOdysseyBlockProxy Block
			, UCurveFloat* CurveC
			, UCurveFloat* CurveM
			, UCurveFloat* CurveY
			, UCurveFloat* CurveK
			, UCurveFloat* CurveAlpha
			, bool PreserveNullAlpha = true);

	//Adjust the LabA components of each pixel in the given Block according to the given curves
	//Abscissa represents the actual component value.
	//Ordinate represents the adjusted component value.
	UFUNCTION(BlueprintPure
		, Category = "OdysseyBlockReference"
		, meta = (DefaultToSelf = "BrushContext", DisplayName = "Adjust Block LabA"))
		static FOdysseyBlockProxy AdjustLabA(FOdysseyBlockProxy Block
			, UCurveFloat* CurveL
			, UCurveFloat* CurveA
			, UCurveFloat* CurveB
			, UCurveFloat* CurveAlpha
			, bool PreserveNullAlpha = true);

    //---

    //Returns the Width of an Odyssey Block Reference as an Integer.
    UFUNCTION( BlueprintPure, Category="OdysseyBlockReference", meta = ( DisplayName="Get Block Width" ) )
    static int GetWidth( FOdysseyBlockProxy Sample );

    //Returns the Height of an Odyssey Block Reference as an Integer.
    UFUNCTION( BlueprintPure, Category="OdysseyBlockReference", meta = ( DisplayName="Get Block Height" ) )
    static int GetHeight( FOdysseyBlockProxy Sample );

    //---

    //Requires a Font to return an Array of Odyssey Block Reference.
    //This node turns the Font into a big block that contains all characters in the font.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockReference"
             , meta = ( DefaultToSelf="BrushContext" ) )
    static TArray< FOdysseyBlockProxy > GetFontBlocks( UOdysseyBrushAssetBase* BrushContext, const UFont* Font, EOdysseyPixelFormat Format = EOdysseyPixelFormat::kCanvasFormat, EOdysseyPixelFormatPrecision Precision = EOdysseyPixelFormatPrecision::kCanvasPrecision );
    
    //Requires a Font and a String to return an Array of Odyssey Block Reference.
    //This node find the correspondance between letters from the String and characters from the Font.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockReference" )
    static TArray< FOdysseyFontCharacter > GetFontCharacterInfo( const UFont* Font, const FString& String );

    //---
    
    //This node automatically picks the color up at position on the canvas. 
    UFUNCTION(BlueprintPure, Category="OdysseyBlockReference")
    static bool GetColorAtPosition( FOdysseyBlockProxy Block, float X, float Y, FOdysseyBrushColor& Color );

    /* Format Management */

    //Get the Odyssey Block Reference Pixel Format
    UFUNCTION(BlueprintPure, Category="OdysseyBlockReference", meta = ( DisplayName="Get Block Format" ))
    static EOdysseyPixelFormat GetFormat(FOdysseyBlockProxy Block);

    //Get the Odyssey Block Reference Pixel Precision
    UFUNCTION(BlueprintPure, Category="OdysseyBlockReference", meta = ( DisplayName="Get Block Precision" ))
    static EOdysseyPixelFormatPrecision GetPrecision(FOdysseyBlockProxy Block);

    //Get the Odyssey Block Reference Rectangle
    UFUNCTION(BlueprintPure, Category="OdysseyBlockReference", meta = ( DisplayName="Get Block Rectangle" ))
    static FOdysseyBrushRect GetRect(FOdysseyBlockProxy Block);
};
