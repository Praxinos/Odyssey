// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include "Proxies/OdysseyBrushColor.h"
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
UENUM( BlueprintType, meta = (DisplayName="EOdysseyResamplingMethod") )
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
    //Sends a neutral value.
    UFUNCTION(BlueprintPure, Category="Odyssey|Matrix")
    static FOdysseyMatrix MakeIdentityMatrix();

    UFUNCTION(BlueprintPure, Category="Odyssey|Matrix")
    static FOdysseyMatrix MakeTranslationMatrix( float DeltaX, float DeltaY );
    
    //Requires an angle (float) to generate a rotation.
    UFUNCTION(BlueprintPure, Category="Odyssey|Matrix")
    static FOdysseyMatrix MakeRotationMatrix( float Angle );

    // Requires a normalized float (0.5 = 50% | 1 = 100%) to rescale a Matrix on X and Y axis.
    UFUNCTION(BlueprintPure, Category="Odyssey|Matrix")
    static FOdysseyMatrix MakeScaleMatrix( float ScaleX, float ScaleY );

    //Requires a normalized float (0.5 = 50% | 1 = 100%) to shear a Matrix on X and Y axis.
    UFUNCTION(BlueprintPure, Category="Odyssey|Matrix")
    static FOdysseyMatrix MakeShearMatrix( float ShearX, float ShearY );
    
    //Composes a Matrix with several Matrices.
    UFUNCTION(BlueprintPure, Category="Odyssey|Matrix")
    static FOdysseyMatrix ComposeMatrix( const FOdysseyMatrix& First, const FOdysseyMatrix& Second );
    
    //Returns the resulting recatngle of a matrix as if it were applied on the given Rectangle with the given ResamplingMethod
    UFUNCTION(BlueprintPure, Category="Odyssey|Matrix")
    static FOdysseyBrushRect GetMatrixResultRect( const FOdysseyMatrix& Matrix, const FOdysseyBrushRect& Rectangle, EResamplingMethod ResamplingMethod );

    //TOTEST with multiple matrix to check associativity
    //Waiting for Epic debug.
    //UFUNCTION(BlueprintPure, meta=(CommutativeAssociativeBinaryOperator = "true"), Category="Odyssey|Matrix")
    //static FOdysseyMatrix ComposeMatrixMatrix2( const FOdysseyMatrix& A, const FOdysseyMatrix& B ) { return ComposeMatrix( A, B ); }
    
    //Requires an Odyssey Matrix input and an Odyssey Block Reference to transform.
    UFUNCTION(BlueprintPure, meta = ( KeyWords = "Matrix"), Category="Odyssey|Transform")
    static FOdysseyBlockProxy Transform( UPARAM(DisplayName="Block") FOdysseyBlockProxy Sample, FOdysseyMatrix Transform, int OutputWidth, int OutputHeight, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour );

    //Requires an angle (Float) to rotate an Odyssey Block Reference.
    UFUNCTION(BlueprintPure
             , Category="Odyssey|Transform"
             , meta = ( KeyWords = "Matrix") )
    static FOdysseyBlockProxy Rotate( UPARAM(DisplayName="Block") FOdysseyBlockProxy Sample, float Angle, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour );

    //Requires a normalized Float (0.5 = 50% | 1 = 100%) to rescale an Odyssey Block Reference uniformly.
    UFUNCTION(BlueprintPure
             , Category="Odyssey|Transform"
             , meta = ( KeyWords = "Matrix") )
    static FOdysseyBlockProxy ScaleUniform( UPARAM(DisplayName="Block") FOdysseyBlockProxy Sample, float Scale, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour );

    //Requires a normalized Float (0.5 = 50% | 1 = 100%) to rescale an Odyssey Block Reference on X and Y axis.
    UFUNCTION(BlueprintPure
             , Category="Odyssey|Transform"
             , meta = ( KeyWords = "Matrix") )
    static FOdysseyBlockProxy ScaleXY( UPARAM(DisplayName="Block") FOdysseyBlockProxy Sample, float ScaleX, float ScaleY, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour );

    //Requires a normalized Float (0.5 = 50% | 1 = 100%) to shear an Odyssey Block Reference.
    UFUNCTION(BlueprintPure
             , Category="Odyssey|Transform"
             , meta = ( KeyWords = "Matrix") )
    static FOdysseyBlockProxy Shear( UPARAM(DisplayName="Block") FOdysseyBlockProxy Sample, float ShearX, float ShearY, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour );
    
    //Requires a Float in pixels to resize an Odyssey Block Reference uniformly.
    UFUNCTION(BlueprintPure
             , Category="Odyssey|Transform"
             , meta = ( KeyWords = "Matrix") )
    static FOdysseyBlockProxy ResizeUniform( UPARAM(DisplayName="Block") FOdysseyBlockProxy Sample, float Size, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour );
    
    //Requires a Float in pixels to resize an Odyssey Block Reference on X and Y axis.
    UFUNCTION(BlueprintPure
             , Category="Odyssey|Transform"
             , meta = ( KeyWords = "Matrix") )
    static FOdysseyBlockProxy Resize( UPARAM(DisplayName="Block") FOdysseyBlockProxy Sample, float SizeX, float SizeY, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour );
    
    //Flips an Odyssey Block Reference on horizontal axis.
    UFUNCTION(BlueprintPure
             , Category="Odyssey|Transform"
             , meta = ( KeyWords = "Matrix") )
    static FOdysseyBlockProxy FlipX( UPARAM(DisplayName="Block") FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour );

    //Flips an Odyssey Block Reference on vertical axis.
    UFUNCTION(BlueprintPure
             , Category="Odyssey|Transform"
             , meta = ( KeyWords = "Matrix") )
    static FOdysseyBlockProxy FlipY( UPARAM(DisplayName="Block") FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour );
    
    //Flips an Odyssey Block Reference on horizontal and vertical axis.
    UFUNCTION(BlueprintPure
             , Category="Odyssey|Transform"
             , meta = ( KeyWords = "Matrix") )
    static FOdysseyBlockProxy FlipXY( UPARAM(DisplayName="Block") FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod = EResamplingMethod::kNearestNeighbour );
};
