// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreTypes.h"
#include "UObject/ObjectMacros.h"
#include <ULIS>
#include "OdysseyBrushPivot.generated.h"

/////////////////////////////////////////////////////
// Pivot Enum
UENUM( BlueprintType, meta=(DisplayName="EOdysseyHandlePositionReference"))
enum  class  EPivotReference : uint8
{
    kTopLeft        UMETA( DisplayName="Top Left" ),
    kTopMiddle      UMETA( DisplayName="Top Middle" ),
    kTopRight       UMETA( DisplayName="Top Right" ),
    kMiddleLeft     UMETA( DisplayName="Middle Left" ),
    kCenter         UMETA( DisplayName="Center" ),
    kMiddleRight    UMETA( DisplayName="Middle Right" ),
    kBotLeft        UMETA( DisplayName="Bot Left"  ),
    kBotMiddle      UMETA( DisplayName="Bot Middle" ),
    kBotRight       UMETA( DisplayName="Bot Right" ),
};


/////////////////////////////////////////////////////
// Pivot Enum
UENUM( BlueprintType, meta=(DisplayName="EOdysseyHandlePositionOffsetMode") )
enum  class  EPivotOffsetMode : uint8
{
    kRelative       UMETA( DisplayName="Relative fraction" ),
    kAbsolute       UMETA( DisplayName="Absolute pixel" ),
};

/////////////////////////////////////////////////////
// Odyssey Brush Pivot
USTRUCT( BlueprintType, meta=(DisplayName="FHandlePosition") )
struct ODYSSEYBRUSH_API FOdysseyPivot
{
    GENERATED_BODY()

    FOdysseyPivot()
        : Reference(    EPivotReference::kCenter )
        , OffsetMode(   EPivotOffsetMode::kRelative )
        , Offset(       FVector2D(0,0) )
    {}

    /**
     * Reference of the pivot.
     *
     * Define the reference point of the pivot on the Odyssey Block
     * (Center, Bottom Middle, Top Left, etc).
     */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="OdysseyHandlePositionReference" )
    EPivotReference     Reference;

    /**
     * How the Offset is interpreted.
     *
     * "Relative fraction" is relative to the Odyssey Block Proxy
     * dimensions; "Absolute pixel" uses X and Y positions in pixels.
     */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="OdysseyHandlePositionReference" )
    EPivotOffsetMode    OffsetMode;

    /**
     * Offset of the pivot from its Reference.
     *
     * With "Relative fraction", X and Y are normalized fractions of the
     * Odyssey Block Proxy's size (0 = 0%, 0.5 = 50%, 1 = 100%).
     * With "Absolute pixel", X and Y are expressed in pixels.
     */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="OdysseyHandlePositionReference" )
    FVector2D           Offset;
};

/////////////////////////////////////////////////////
// Utilities
struct FRectF    //PATCH: until ::ulis3::FRectF
{
    float x;
    float y;
    float w;
    float h;
};

FRectF ComputeRectWithPivot(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock, const FOdysseyPivot& iPivot, float iX, float iY );    //PATCH: until ::ulis3::FRectF
//::ULIS::FRectI ComputeRectWithPivot( FOdysseyBlock* iBlock, const FOdysseyPivot& iPivot, float iX, float iY );
