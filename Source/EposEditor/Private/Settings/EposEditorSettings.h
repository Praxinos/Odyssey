// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "EposEditorSettings.generated.h"

//---

USTRUCT()
struct FBoardSettings
{
    GENERATED_BODY()
};

//---

USTRUCT()
struct FShotSettings
{
    GENERATED_BODY()
};

//---

/**
 * Epos Editor settings.
 */
UCLASS(config=Epos)
class UEposEditorSettings
    : public UObject
{
    GENERATED_BODY()

public:

    UEposEditorSettings(const FObjectInitializer& ObjectInitializer);

    /** Specifies Board stuff. */
    UPROPERTY(config, EditAnywhere, Category=Board)
    FBoardSettings BoardSettings;

    /** Specifies Shot stuff. */
    UPROPERTY(config, EditAnywhere, Category=Shot)
    FShotSettings ShotSettings;
};
