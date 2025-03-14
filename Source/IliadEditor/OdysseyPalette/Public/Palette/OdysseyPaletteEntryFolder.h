// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

#include "OdysseyPaletteEntry.h"

#include "OdysseyPaletteEntryFolder.generated.h"

/////////////////////////////////////////////////////
// OdysseyColorPaletteEntryFolder
UCLASS()
class ODYSSEYPALETTE_API UOdysseyPaletteEntryFolder : public UOdysseyPaletteEntry
{
    GENERATED_BODY()

public:
    UOdysseyPaletteEntryFolder();

public:
    UPROPERTY(AssetRegistrySearchable)
    TArray<TObjectPtr<UOdysseyPaletteEntry>> mChildren;
};
