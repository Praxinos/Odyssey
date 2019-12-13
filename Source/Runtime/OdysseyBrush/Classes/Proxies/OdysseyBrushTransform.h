// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include "Proxies/OdysseyBrushColor.h"
#include "Proxies/OdysseyBrushCache.h"
#include "Proxies/OdysseyBrushBlock.h"
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OdysseyBrushTransform.generated.h"

class  FOdysseyBlock;

/////////////////////////////////////////////////////
// Odyssey Matrix
USTRUCT(BlueprintType)
struct ODYSSEYBRUSH_API FOdysseyMatrix
{
    GENERATED_BODY()

    FOdysseyMatrix()
        : mat( glm::mat3( 1.f ) )
    {}

    FOdysseyMatrix( const  glm::mat3&  iMat )
        : mat( iMat )
    {}

    FString ID()
    {
        return  FString::FromBlob( ( const uint8* )glm::value_ptr( mat ), 9 * sizeof( float ) );
    }

    glm::mat3  mat;
};

/////////////////////////////////////////////////////
// Odyssey Resampling
UENUM( BlueprintType )
enum  class  EResamplingMethod : uint8
{
    kNearestNeighbour   UMETA( DisplayName="Nearest Neighbour" ),
    kBilinear           UMETA( DisplayName="Bilinear" ),
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

    UFUNCTION(BlueprintPure, meta = ( AdvancedDisplay="Cache" ), Category="OdysseyTransform")
    static FOdysseyBlockProxy Transform( FOdysseyBlockProxy Sample, FOdysseyMatrix Transform, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure, meta = ( AdvancedDisplay="Cache" ), Category="OdysseyTransform")
    static FOdysseyBlockProxy Rotate( FOdysseyBlockProxy Sample, float Angle, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure, meta = ( AdvancedDisplay="Cache" ), Category="OdysseyTransform")
    static FOdysseyBlockProxy ScaleUniform( FOdysseyBlockProxy Sample, float Scale, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure, meta = ( AdvancedDisplay="Cache" ), Category="OdysseyTransform")
    static FOdysseyBlockProxy ScaleXY( FOdysseyBlockProxy Sample, float ScaleX, float ScaleY, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure, meta = ( AdvancedDisplay="Cache" ), Category="OdysseyTransform")
    static FOdysseyBlockProxy Shear( FOdysseyBlockProxy Sample, float ShearX, float ShearY, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure, meta = ( AdvancedDisplay="Cache" ), Category="OdysseyTransform")
    static FOdysseyBlockProxy ResizeUniform( FOdysseyBlockProxy Sample, float Size, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure, meta = ( AdvancedDisplay="Cache" ), Category="OdysseyTransform")
    static FOdysseyBlockProxy Resize( FOdysseyBlockProxy Sample, float SizeX, float Size, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure, meta = ( AdvancedDisplay="Cache" ), Category="OdysseyTransform")
    static FOdysseyBlockProxy FlipX( FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure, meta = ( AdvancedDisplay="Cache" ), Category="OdysseyTransform")
    static FOdysseyBlockProxy FlipY( FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );

    UFUNCTION(BlueprintPure, meta = ( AdvancedDisplay="Cache" ), Category="OdysseyTransform")
    static FOdysseyBlockProxy FlipXY( FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour, ECacheLevel Cache = ECacheLevel::kStep );
};
