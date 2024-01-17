// IDDN.FR.001.220036.002.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "AssetExportTask.h"
#include "EposSequencerExportTask.generated.h"

/**
 * Contains data for a group of assets to import
 */
UCLASS(Transient, BlueprintType)
class UEposSequencerExportTask : public UAssetExportTask
{
    GENERATED_BODY()

public:

    /* A UWorld for LevelSequences, UUserWidget for WidgetAnimations, or AActor for Actor Sequences, etc... */
    UPROPERTY(BlueprintReadWrite, Category = Miscellaneous)
    TObjectPtr<UObject> SequencerContext;
};
