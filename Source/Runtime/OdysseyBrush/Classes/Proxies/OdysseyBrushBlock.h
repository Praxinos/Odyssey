// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include "Proxies/OdysseyBrushColor.h"
#include "Proxies/OdysseyBrushCache.h"
#include "Proxies/OdysseyBrushBlending.h"
#include "OdysseyBrushBlock.generated.h"

class  FOdysseyBlock;
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
    MakeNullProxy() {
        return  FOdysseyBlockProxy();
    }

    FOdysseyBlock*  m;
    FString         id;
    bool            valid;
};


/////////////////////////////////////////////////////
// UOdysseyBlockProxyFunctionLibrary
UCLASS(meta=(ScriptName="OdysseyBlockProxyLibrary"))
class ODYSSEYBRUSH_API UOdysseyBlockProxyFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockProxy"
             , meta = ( DefaultToSelf="BrushContext", DisplayName = "To OdysseyBlockProxy (Texture2D)", CompactNodeTitle = "->", BlueprintAutocast ) )
    static FOdysseyBlockProxy Conv_TextureToOdysseyBlockProxy( UTexture2D* Texture, UOdysseyBrushAssetBase* BrushContext );

    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockProxy"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="Cache" ) )
    static FOdysseyBlockProxy FillPreserveAlpha( UOdysseyBrushAssetBase* BrushContext
                                               , FOdysseyBlockProxy Sample
                                               , FOdysseyBrushColor Color
                                               , ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION( BlueprintPure
             , Category="OdysseyBlockProxy"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="InitializeData, Cache" ) )
    static FOdysseyBlockProxy CreateBlock( UOdysseyBrushAssetBase* BrushContext
                                         , int Width = 256
                                         , int Height = 256
                                         , const FString& ID = "NewDynamicBlock"
                                         , bool InitializeData = true
                                         , ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure
             , Category="OdysseyBlockProxy"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="Cache" ) )
    static FOdysseyBlockProxy Blend( UOdysseyBrushAssetBase* BrushContext
                                   , FOdysseyBlockProxy Top
                                   , FOdysseyBlockProxy BacK
                                   , int X = 0
                                   , int Y = 0
                                   , float Opacity = 1.f
                                   , EOdysseyBlendingMode BlendingMode = EOdysseyBlendingMode::kNormal
                                   , EOdysseyAlphaMode AlphaMode = EOdysseyAlphaMode::kNormal
                                   , ECacheLevel Cache = ECacheLevel::kState );

    UFUNCTION( BlueprintPure, Category="OdysseyBlockProxy" )
    static int GetWidth( FOdysseyBlockProxy Sample );

    UFUNCTION(BlueprintPure, Category="OdysseyBlockProxy" )
    static int GetHeight( FOdysseyBlockProxy Sample );
};
