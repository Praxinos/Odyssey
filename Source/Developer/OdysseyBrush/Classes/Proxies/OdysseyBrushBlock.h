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
// Odyssey Block Proxy
USTRUCT(BlueprintType)
struct ODYSSEYBRUSH_API FOdysseyBlockProxy
{
    GENERATED_BODY()

    FOdysseyBlockProxy()
        : m(        0       )
        , id(       "None"  )
        , valid(    false   )
    {}

    FOdysseyBlockProxy( FOdysseyBlock* iBlock, const  FString& iId )
        : m(        iBlock  )
        , id(       iId     )
        , valid(    true    )
    {}

    static
    FOdysseyBlockProxy
    MakeNullProxy()
    {
        return  FOdysseyBlockProxy();
    }

    FOdysseyBlock*  m;
    FString         id;
    bool            valid;
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
    //Converts Texture 2D to Odyssey Block Proxy.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockProxy"
             , meta = ( DefaultToSelf="BrushContext", DisplayName = "To OdysseyBlockProxy (Texture2D)", CompactNodeTitle = "->", BlueprintAutocast ) )
    static FOdysseyBlockProxy Conv_TextureToOdysseyBlockProxy( UTexture2D* Texture, EOdysseyBlockFormat Format, EOdysseyBlockFormatPrecision Precision, UOdysseyBrushAssetBase* BrushContext);

    //Applies a color on sample's alpha channel. Requires an Odyssey Brush Color input.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockProxy"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="Cache" ) )
    static FOdysseyBlockProxy FillPreserveAlpha( UOdysseyBrushAssetBase* BrushContext
                                               , FOdysseyBlockProxy Sample
                                               , FOdysseyBrushColor Color
                                               , ECacheLevel Cache = ECacheLevel::kStep );
    
    //Creates an empty Odyssey Block Proxy.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockProxy"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="InitializeData, Cache" ) )
    static FOdysseyBlockProxy CreateBlock( UOdysseyBrushAssetBase* BrushContext
                                         , int Width = 256
                                         , int Height = 256
                                         , EOdysseyBlockFormat Format = EOdysseyBlockFormat::kAuto
										 , EOdysseyBlockFormatPrecision Precision = EOdysseyBlockFormatPrecision::kAuto
                                         , const FString& ID = "NewDynamicBlock"
                                         , bool InitializeData = true
                                         , ECacheLevel Cache = ECacheLevel::kStep );

    //Blends two Odyssey Block Proxy on Top and Back.
    //If Top is bigger than Back = Back will crop Top.
    //If Top is smaller than Back = Both will be visible.
    //X | Y are an offset to pan Top.
    UFUNCTION(BlueprintPure
             , Category="OdysseyBlockProxy"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="Cache" ) )
    static FOdysseyBlockProxy Blend( UOdysseyBrushAssetBase* BrushContext
                                   , FOdysseyBlockProxy Top
                                   , FOdysseyBlockProxy Back
                                   , int X = 0
                                   , int Y = 0
                                   , float Opacity = 1.f
								   , EOdysseyBlockFormat Format = EOdysseyBlockFormat::kAuto
								   , EOdysseyBlockFormatPrecision Precision = EOdysseyBlockFormatPrecision::kAuto
                                   , EOdysseyBlendingMode BlendingMode = EOdysseyBlendingMode::kNormal
                                   , EOdysseyAlphaMode AlphaMode = EOdysseyAlphaMode::kNormal
                                   , ECacheLevel Cache = ECacheLevel::kState);

    //---

    //Returns the Width of an Odyssey Block Proxy as an Integer.
    UFUNCTION( BlueprintPure, Category="OdysseyBlockProxy" )
    static int GetWidth( FOdysseyBlockProxy Sample );

    //Returns the Height of an Odyssey Block Proxy as an Integer.
    UFUNCTION( BlueprintPure, Category="OdysseyBlockProxy" )
    static int GetHeight( FOdysseyBlockProxy Sample );

    //---

    //Requires a Font to return an Array of Odyssey Block Proxy.
    //This node turns the Font into a big block that contains all characters in the font.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockProxy"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="Cache" ) )
    static TArray< FOdysseyBlockProxy > GetFontBlocks( UOdysseyBrushAssetBase* BrushContext, const UFont* Font, EOdysseyBlockFormat Format = EOdysseyBlockFormat::kAuto, EOdysseyBlockFormatPrecision Precision = EOdysseyBlockFormatPrecision::kAuto, ECacheLevel Cache = ECacheLevel::kState );
    
    //Requires a Font and a String to return an Array of Odyssey Block Proxy.
    //This node find the correspondance between letters from the String and characters from the Font.
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockProxy" )
    static TArray< FOdysseyFontCharacter > GetFontCharacterInfo( const UFont* Font, const FString& String );

    //---
    
    //This node automatically picks the color up at position on the canvas. 
    UFUNCTION(BlueprintPure, Category="OdysseyBlockProxy")
    static bool GetColorAtPosition( FOdysseyBlockProxy Block, float X, float Y, FOdysseyBrushColor& Color );
};
