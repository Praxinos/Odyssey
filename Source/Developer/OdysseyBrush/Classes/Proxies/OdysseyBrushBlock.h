// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include "Proxies/OdysseyBrushColor.h"
#include "Proxies/OdysseyBrushCache.h"
#include "Proxies/OdysseyBrushFormat.h"
#include "Proxies/OdysseyBrushBlending.h"

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

    //Applies a color on sample's alpha channel. Requires an Odyssey Brush Color input.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockReference"
             , meta = ( DefaultToSelf="BrushContext", DeprecatedFunction, DeprecationMessage = "Use Fill" ) )
    static FOdysseyBlockProxy FillPreserveAlpha(  UOdysseyBrushAssetBase* BrushContext
                                                , FOdysseyBlockProxy Sample
                                                , FOdysseyBrushColor Color );

    //Applies a color on the whole given Sample. Requires an Odyssey Brush Color input.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockReference"
             , meta = ( DefaultToSelf="BrushContext" ) )
    static FOdysseyBlockProxy Fill( UOdysseyBrushAssetBase* BrushContext
                                    , FOdysseyBlockProxy Sample
                                    , FOdysseyBrushColor Color
                                    , float Opacity = 1.f
                                    , EOdysseyPixelFormat Format = EOdysseyPixelFormat::kAuto
                                    , EOdysseyPixelFormatPrecision Precision = EOdysseyPixelFormatPrecision::kAuto
                                    , EOdysseyBlendingMode BlendingMode = EOdysseyBlendingMode::kNormal
                                    , EOdysseyAlphaMode AlphaMode = EOdysseyAlphaMode::kNormal);

    //Creates an empty Odyssey Block Reference.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockReference"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="InitializeData" ) )
    static FOdysseyBlockProxy CreateBlock( UOdysseyBrushAssetBase* BrushContext
                                         , int Width = 256
                                         , int Height = 256
                                         , EOdysseyPixelFormat Format = EOdysseyPixelFormat::kAuto
										 , EOdysseyPixelFormatPrecision Precision = EOdysseyPixelFormatPrecision::kAuto
                                         , bool InitializeData = true );

    //Blends two Odyssey Block Reference on Top and Back.
    //If Top is bigger than Back = Back will crop Top.
    //If Top is smaller than Back = Both will be visible.
    //X | Y are an offset to pan Top.
    UFUNCTION(BlueprintPure
             , Category="OdysseyBlockReference"
             , meta = ( DefaultToSelf="BrushContext" ) )
    static FOdysseyBlockProxy Blend( UOdysseyBrushAssetBase* BrushContext
                                   , FOdysseyBlockProxy Top
                                   , FOdysseyBlockProxy Back
                                   , int X = 0
                                   , int Y = 0
                                   , float Opacity = 1.f
								   , EOdysseyPixelFormat Format = EOdysseyPixelFormat::kAuto
								   , EOdysseyPixelFormatPrecision Precision = EOdysseyPixelFormatPrecision::kAuto
                                   , EOdysseyBlendingMode BlendingMode = EOdysseyBlendingMode::kNormal
                                   , EOdysseyAlphaMode AlphaMode = EOdysseyAlphaMode::kNormal);

    //---

    //Returns the Width of an Odyssey Block Reference as an Integer.
    UFUNCTION( BlueprintPure, Category="OdysseyBlockReference" )
    static int GetWidth( FOdysseyBlockProxy Sample );

    //Returns the Height of an Odyssey Block Reference as an Integer.
    UFUNCTION( BlueprintPure, Category="OdysseyBlockReference" )
    static int GetHeight( FOdysseyBlockProxy Sample );

    //---

    //Requires a Font to return an Array of Odyssey Block Reference.
    //This node turns the Font into a big block that contains all characters in the font.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockReference"
             , meta = ( DefaultToSelf="BrushContext" ) )
    static TArray< FOdysseyBlockProxy > GetFontBlocks( UOdysseyBrushAssetBase* BrushContext, const UFont* Font, EOdysseyPixelFormat Format = EOdysseyPixelFormat::kAuto, EOdysseyPixelFormatPrecision Precision = EOdysseyPixelFormatPrecision::kAuto );
    
    //Requires a Font and a String to return an Array of Odyssey Block Reference.
    //This node find the correspondance between letters from the String and characters from the Font.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockReference" )
    static TArray< FOdysseyFontCharacter > GetFontCharacterInfo( const UFont* Font, const FString& String );

    //---
    
    //This node automatically picks the color up at position on the canvas. 
    UFUNCTION(BlueprintPure, Category="OdysseyBlockReference")
    static bool GetColorAtPosition( FOdysseyBlockProxy Block, float X, float Y, FOdysseyBrushColor& Color );
};
