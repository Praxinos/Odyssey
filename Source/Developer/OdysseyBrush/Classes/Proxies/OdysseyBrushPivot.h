// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreTypes.h"
#include "UObject/ObjectMacros.h"
#include <ULIS3>
#include "OdysseyBrushPivot.generated.h"

class FOdysseyBlock;
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

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="OdysseyHandlePositionReference" )
    EPivotReference     Reference;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="OdysseyHandlePositionReference" )
    EPivotOffsetMode    OffsetMode;

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

FRectF ComputeRectWithPivot(TSharedPtr<FOdysseyBlock> iBlock, const FOdysseyPivot& iPivot, float iX, float iY );    //PATCH: until ::ulis3::FRectF
//::ul3::FRect ComputeRectWithPivot( FOdysseyBlock* iBlock, const FOdysseyPivot& iPivot, float iX, float iY );

