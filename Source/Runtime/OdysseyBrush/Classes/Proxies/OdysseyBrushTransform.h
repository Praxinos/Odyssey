// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include "Proxies/OdysseyBrushColor.h"
#include "Proxies/OdysseyBrushCache.h"
#include "Proxies/OdysseyBrushBlock.h"
#include "OdysseyBrushAssetBase.h"
#include <ULIS3>

#include "OdysseyBrushTransform.generated.h"

class  FOdysseyBlock;

/////////////////////////////////////////////////////
// Odyssey Matrix
USTRUCT(BlueprintType)
struct ODYSSEYBRUSH_API FOdysseyMatrix
{
    GENERATED_BODY()
    FOdysseyMatrix();
    FOdysseyMatrix( const  ::ul3::FTransform2D&  iMat );
    FString ID() const;
    const ::ul3::FTransform2D& GetValue() const;

private:
    ::ul3::FTransform2D m;
};

/////////////////////////////////////////////////////
// Odyssey Resampling
UENUM( BlueprintType )
enum  class  EResamplingMethod : uint8
{
    kNearestNeighbour   UMETA( DisplayName="Nearest Neighbour" ),
    kBilinear           UMETA( DisplayName="Bilinear" ),
    kBicubic            UMETA( DisplayName="Bicubic" ),
    kArea               UMETA( DisplayName="Area" ),
};

/////////////////////////////////////////////////////
// UOdysseyTransformProxyLibrary
UCLASS(meta=(ScriptName="OdysseyTransformLibrary"))
class ODYSSEYBRUSH_API UOdysseyTransformProxyLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="OdysseyTransform")
    static FOdysseyMatrix MakeIdentityMatrix();

    UFUNCTION(BlueprintPure, Category="OdysseyTransform")
    static FOdysseyMatrix MakeTranslationMatrix( float DeltaX, float DeltaY );

    UFUNCTION(BlueprintPure, Category="OdysseyTransform")
    static FOdysseyMatrix MakeRotationMatrix( float Angle );

    UFUNCTION(BlueprintPure, Category="OdysseyTransform")
    static FOdysseyMatrix MakeScaleMatrix( float ScaleX, float ScaleY );

    UFUNCTION(BlueprintPure, Category="OdysseyTransform")
    static FOdysseyMatrix MakeShearMatrix( float ShearX, float ShearY );

    UFUNCTION(BlueprintPure, Category="OdysseyTransform")
    static FOdysseyMatrix ComposeMatrix( const FOdysseyMatrix& First, const FOdysseyMatrix& Second );

    UFUNCTION(BlueprintPure, meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="Cache" ), Category="OdysseyTransform")
    static FOdysseyBlockProxy Transform( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, FOdysseyMatrix Transform, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure
             , Category="OdysseyTransform"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="Cache" ) )
    static FOdysseyBlockProxy Rotate( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, float Angle, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure
             , Category="OdysseyTransform"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="Cache" ) )
    static FOdysseyBlockProxy ScaleUniform( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, float Scale, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure
             , Category="OdysseyTransform"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="Cache" ) )
    static FOdysseyBlockProxy ScaleXY( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, float ScaleX, float ScaleY, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure
             , Category="OdysseyTransform"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="Cache" ) )
    static FOdysseyBlockProxy Shear( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, float ShearX, float ShearY, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure
             , Category="OdysseyTransform"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="Cache" ) )
    static FOdysseyBlockProxy ResizeUniform( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, float Size, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure
             , Category="OdysseyTransform"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="Cache" ) )
    static FOdysseyBlockProxy Resize( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, float SizeX, float SizeY, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure
             , Category="OdysseyTransform"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="Cache" ) )
    static FOdysseyBlockProxy FlipX( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure
             , Category="OdysseyTransform"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="Cache" ) )
    static FOdysseyBlockProxy FlipY( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure
             , Category="OdysseyTransform"
             , meta = ( DefaultToSelf="BrushContext", AdvancedDisplay="Cache" ) )
    static FOdysseyBlockProxy FlipXY( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );
};
