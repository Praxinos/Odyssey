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

    UPROPERTY( BlueprintReadWrite, Category = "Odyssey|Block")
    int32 StartU;

    UPROPERTY( BlueprintReadWrite, Category = "Odyssey|Block")
    int32 StartV;

    UPROPERTY( BlueprintReadWrite, Category = "Odyssey|Block")
    int32 USize;

    UPROPERTY( BlueprintReadWrite, Category = "Odyssey|Block")
    int32 VSize;

    UPROPERTY( BlueprintReadWrite, Category = "Odyssey|Block")
    uint8 TextureIndex;

    UPROPERTY( BlueprintReadWrite, Category = "Odyssey|Block")
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
             , Category="Odyssey|Block"
             , meta = ( DisplayName = "To Odyssey Block Reference (Texture2D)", BlueprintAutocast ) )
    static FOdysseyBlockProxy Conv_TextureToOdysseyBlockProxy( UTexture2D* Texture, EOdysseyColorModel ColorModel = EOdysseyColorModel::kRGBA, EOdysseyChannelDepth ChannelDepth = EOdysseyChannelDepth::k8);

    //Convert the Odyssey Block Reference to the specified Color Model and Channel Depth
    UFUNCTION(BlueprintPure, Category="Odyssey|Block", meta = ( DisplayName="Convert Block to Format" ))
    static FOdysseyBlockProxy ConvertToFormat(FOdysseyBlockProxy Block, EOdysseyColorModel ColorModel = EOdysseyColorModel::kRGBA, EOdysseyChannelDepth ChannelDepth = EOdysseyChannelDepth::k8);

    //Applies a color on sample's alpha channel. Requires an Odyssey Brush Color input.
    UFUNCTION( BlueprintPure
             , Category="Odyssey|Block"
             , meta = ( DeprecatedFunction, DeprecationMessage = "Use Fill Block With Color" ) )
    static FOdysseyBlockProxy FillPreserveAlpha(UPARAM(DisplayName="Block") FOdysseyBlockProxy Sample
                                                , FOdysseyBrushColor Color );

    //Blends a color on the whole given Sample. Requires an Odyssey Brush Color input.
	UFUNCTION(BlueprintPure
		, Category = "Odyssey|Block"
		, meta = (DisplayName = "Blend Block With Color"))
	static FOdysseyBlockProxy BlendColor( FOdysseyBrushColor Color
			, FOdysseyBlockProxy Back
			, FOdysseyBrushRect Area
			, float Opacity = 1.f
			, EOdysseyColorModel ColorModel = EOdysseyColorModel::kRGBA
			, EOdysseyChannelDepth ChannelDepth = EOdysseyChannelDepth::k8
			, EOdysseyBlendingMode BlendingMode = EOdysseyBlendingMode::kNormal
			, EOdysseyAlphaMode AlphaMode = EOdysseyAlphaMode::kNormal);

    //Fills the given Sample with thge given Color. Requires an Odyssey Brush Color input.
    UFUNCTION( BlueprintPure
             , Category="Odyssey|Block"
             , meta = ( DisplayName = "Fill Block With Color" ) )
    static FOdysseyBlockProxy Fill( FOdysseyBlockProxy Block
                                    , FOdysseyBrushColor Color
                                    , FOdysseyBrushRect Area
                                    , bool PreserveAlpha = false
                                    , EOdysseyColorModel ColorModel = EOdysseyColorModel::kRGBA
                                    , EOdysseyChannelDepth ChannelDepth = EOdysseyChannelDepth::k8);

    //Creates an empty Odyssey Block Reference.
    UFUNCTION( BlueprintPure
             , Category="Odyssey|Block"
             , meta = ( AdvancedDisplay="InitializeData" ) )
    static FOdysseyBlockProxy CreateBlock( int Width = 256
                                         , int Height = 256
                                         , EOdysseyColorModel ColorModel = EOdysseyColorModel::kRGBA
										 , EOdysseyChannelDepth ChannelDepth = EOdysseyChannelDepth::k8
                                         , bool InitializeData = true );

    //Crops the Odyssey Block Reference according to the given Rect.
    UFUNCTION( BlueprintPure
             , Category="Odyssey|Block" )
    static FOdysseyBlockProxy CropBlock( FOdysseyBlockProxy Block
                                         , FOdysseyBrushRect Area);

    //Blends two Odyssey Block Reference on Top and Back.
    //If Top is bigger than Back = Back will crop Top.
    //If Top is smaller than Back = Both will be visible.
    //X | Y are an offset to pan Top.
    UFUNCTION(BlueprintPure
             , Category="Odyssey|Block"
             , meta = ( DisplayName = "Blend Blocks" ) )
    static FOdysseyBlockProxy Blend( FOdysseyBlockProxy Top
                                   , FOdysseyBlockProxy Back
                                   , FOdysseyBrushRect TopArea
                                   , UPARAM(DisplayName="Top Offset X") int X = 0
                                   , UPARAM(DisplayName="Top Offset Y") int Y = 0
                                   , float Opacity = 1.f
								   , EOdysseyColorModel ColorModel = EOdysseyColorModel::kRGBA
								   , EOdysseyChannelDepth ChannelDepth = EOdysseyChannelDepth::k8
                                   , EOdysseyBlendingMode BlendingMode = EOdysseyBlendingMode::kNormal
                                   , EOdysseyAlphaMode AlphaMode = EOdysseyAlphaMode::kNormal);

    //Adjust the alpha component of each pixel in the given Block according to the given curve
    //Abscissa represents the actual alpha value.
    //Ordinate represents the adjusted alpha value.
    UFUNCTION(BlueprintPure
             , Category="Odyssey|Block"
             , meta = ( DisplayName = "Adjust Block Alpha" ) )
    static FOdysseyBlockProxy AdjustAlpha( FOdysseyBlockProxy Block
                                         , UCurveFloat* Curve
                                         , bool PreserveNullAlpha = true);

	//Adjust the RGBA components of each pixel in the given Block according to the given curves
	//Abscissa represents the actual component value.
	//Ordinate represents the adjusted component value.
	UFUNCTION(BlueprintPure
			, Category = "Odyssey|Block"
			, meta = (DisplayName = "Adjust Block RGBA"))
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
		, Category = "Odyssey|Block"
		, meta = (DisplayName = "Adjust Block GreyA"))
		static FOdysseyBlockProxy AdjustGreyA(FOdysseyBlockProxy Block
			, UCurveFloat* CurveGrey
			, UCurveFloat* CurveAlpha
			, bool PreserveNullAlpha = true);


	//Adjust the HSVA components of each pixel in the given Block according to the given curves
	//Abscissa represents the actual component value.
	//Ordinate represents the adjusted component value.
	UFUNCTION(BlueprintPure
		, Category = "Odyssey|Block"
		, meta = (DisplayName = "Adjust Block HSVA"))
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
		, Category = "Odyssey|Block"
		, meta = (DisplayName = "Adjust Block HSLA"))
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
		, Category = "Odyssey|Block"
		, meta = (DisplayName = "Adjust Block CMYKA"))
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
		, Category = "Odyssey|Block"
		, meta = (DisplayName = "Adjust Block LabA"))
		static FOdysseyBlockProxy AdjustLabA(FOdysseyBlockProxy Block
			, UCurveFloat* CurveL
			, UCurveFloat* CurveA
			, UCurveFloat* CurveB
			, UCurveFloat* CurveAlpha
			, bool PreserveNullAlpha = true);

    //---

    //Returns the Width of an Odyssey Block Reference as an Integer.
    UFUNCTION( BlueprintPure, Category="Odyssey|Block", meta = ( DisplayName="Get Block Width" ) )
    static int GetWidth( UPARAM(DisplayName="Block") FOdysseyBlockProxy Sample );

    //Returns the Height of an Odyssey Block Reference as an Integer.
    UFUNCTION( BlueprintPure, Category="Odyssey|Block", meta = ( DisplayName="Get Block Height" ) )
    static int GetHeight( UPARAM(DisplayName="Block") FOdysseyBlockProxy Sample );

    //---

    //Requires a Font to return an Array of Odyssey Block Reference.
    //This node turns the Font into a big block that contains all characters in the font.
    UFUNCTION( BlueprintPure
             , Category="Odyssey|Block" )
    static TArray< FOdysseyBlockProxy > GetFontBlocks( const UFont* Font, EOdysseyColorModel ColorModel = EOdysseyColorModel::kRGBA, EOdysseyChannelDepth ChannelDepth = EOdysseyChannelDepth::k8 );
    
    //Requires a Font and a String to return an Array of Odyssey Block Reference.
    //This node find the correspondance between letters from the String and characters from the Font.
    UFUNCTION( BlueprintPure
             , Category="Odyssey|Block" )
    static TArray< FOdysseyFontCharacter > GetFontCharacterInfo( const UFont* Font, const FString& String );

    //Requires a Font and a String to return Arrays of width/height of each character.
    //This node compute the box of each character corresponding to the font.
    UFUNCTION( BlueprintPure
             , Category = "Odyssey|Block" )
    static void GetCharactersSize( const UFont* Font, const FString& String, TArray<float>& Width, TArray<float>& Height );

    //---
    
    //This node automatically picks the color up at position on the canvas. 
    UFUNCTION(BlueprintPure, Category="Odyssey|Block")
    static bool GetColorAtPosition( FOdysseyBlockProxy Block, float X, float Y, FOdysseyBrushColor& Color );

    /* Format Management */

    //Get the Odyssey Block Reference Color Model
    UFUNCTION(BlueprintPure, Category="Odyssey|Block", meta = ( DisplayName="Get Block Color Model" ))
    static EOdysseyColorModel GetColorModel(FOdysseyBlockProxy Block);

    //Get the Odyssey Block Reference Channel Depth
    UFUNCTION(BlueprintPure, Category="Odyssey|Block", meta = ( DisplayName="Get Block Channel Depth" ))
    static EOdysseyChannelDepth GetChannelDepth(FOdysseyBlockProxy Block);

    //Get the Odyssey Block Reference Rectangle
    UFUNCTION(BlueprintPure, Category="Odyssey|Block", meta = ( DisplayName="Get Block Rectangle" ))
    static FOdysseyBrushRect GetRect(FOdysseyBlockProxy Block);
};
