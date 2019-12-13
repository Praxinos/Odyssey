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
    UFUNCTION(BlueprintPure, meta = (DisplayName = "To OdysseyBlockProxy (Texture2D)", CompactNodeTitle = "->", BlueprintAutocast), Category="OdysseyBlockProxy")
    static FOdysseyBlockProxy Conv_TextureToOdysseyBlockProxy( UTexture2D* Texture );

    UFUNCTION(BlueprintPure, meta = ( AdvancedDisplay="Cache" ), Category="OdysseyBlockProxy")
    static FOdysseyBlockProxy FillPreserveAlpha( FOdysseyBlockProxy Sample
                                               , FOdysseyBrushColor Color
                                               , ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure, meta = ( AdvancedDisplay="InitializeData, Cache" ), Category="OdysseyBlockProxy")
    static FOdysseyBlockProxy CreateBlock( int Width = 256
                                         , int Height = 256
                                         , const FString& ID = "NewDynamicBlock"
                                         , bool InitializeData = true
                                         , ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure, meta = ( AdvancedDisplay="Cache" ), Category="OdysseyBlockProxy")
    static FOdysseyBlockProxy Blend( FOdysseyBlockProxy Top
                                   , FOdysseyBlockProxy BacK
                                   , int X = 0
                                   , int Y = 0
                                   , float Opacity = 1.f
                                   , EOdysseyBlendingMode BlendingMode = EOdysseyBlendingMode::kNormal
                                   , EOdysseyAlphaMode AlphaMode = EOdysseyAlphaMode::kNormal
                                   , ECacheLevel Cache = ECacheLevel::kState );

    UFUNCTION(BlueprintPure, Category="OdysseyBlockProxy")
    static int GetWidth( FOdysseyBlockProxy Sample );

    UFUNCTION(BlueprintPure, Category="OdysseyBlockProxy")
    static int GetHeight( FOdysseyBlockProxy Sample );
};

#define  ODYSSEY_BRUSH_BLOCK_PROXY_CHECK_RETURN                     \
    if( !Sample.m ) return;

#define  ODYSSEY_BRUSH_BLOCK_PROXY_CHECK_RETURN_VALUE( iValue )     \
    if( !Sample.m ) return  iValue;
