// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Layout/SScrollBox.h"
#include "OdysseyPaletteEntry.h"

#include "OdysseyPalette.generated.h"

/////////////////////////////////////////////////////
// OdysseyColorPalette
UCLASS(BlueprintType, config = EditorPerProjectUserSettings, PerObjectConfig)
class ODYSSEYPALETTE_API UOdysseyPalette : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = Palette)
    TArray<UOdysseyPaletteEntry*> mPaletteEntries;

    UPROPERTY(config, DuplicateTransient, meta = (AllowedClasses = "OdysseyPaletteEntry"))
    TSoftObjectPtr<UOdysseyPaletteEntry> CurrentEntry;
};
