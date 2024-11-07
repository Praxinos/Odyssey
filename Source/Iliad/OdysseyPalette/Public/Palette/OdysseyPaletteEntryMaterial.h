// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPaletteEntry.h"

#include "OdysseyPaletteEntryMaterial.generated.h"

class UMaterial;

/////////////////////////////////////////////////////
// OdysseyColorPaletteEntryMaterial
UCLASS()
class ODYSSEYPALETTE_API UOdysseyPaletteEntryMaterial : public UOdysseyPaletteEntry
{
    GENERATED_BODY()

public:
    UOdysseyPaletteEntryMaterial();

public:
    UPROPERTY(EditAnywhere, Category=Palette)
    TArray<UMaterial*> EntryMaterial;
};
