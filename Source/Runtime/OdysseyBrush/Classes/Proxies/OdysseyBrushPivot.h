// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreTypes.h"
#include "UObject/ObjectMacros.h"
#include "OdysseyBrushPivot.generated.h"

/////////////////////////////////////////////////////
// Pivot Enum
UENUM( BlueprintType )
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
UENUM( BlueprintType )
enum  class  EPivotOffsetMode : uint8
{
    kRelative       UMETA( DisplayName="Relative fraction" ),
    kAbsolute       UMETA( DisplayName="Absolute pixel" ),
};



/////////////////////////////////////////////////////
// Odyssey Brush Pivot
USTRUCT( BlueprintType )
struct ODYSSEYBRUSH_API FOdysseyPivot
{
    GENERATED_BODY()

    FOdysseyPivot()
        : Reference(    EPivotReference::kCenter )
        , OffsetMode(   EPivotOffsetMode::kRelative )
        , Offset(       FVector2D() )
    {}

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="OdysseyPivot" )
    EPivotReference     Reference;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="OdysseyPivot" )
    EPivotOffsetMode    OffsetMode;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="OdysseyPivot" )
    FVector2D           Offset;
};

