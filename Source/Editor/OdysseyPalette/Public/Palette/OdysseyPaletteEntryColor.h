// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyPaletteEntryColor.generated.h"

/////////////////////////////////////////////////////
// OdysseyColorPaletteEntryColor
UCLASS()
class ODYSSEYPALETTE_API UOdysseyPaletteEntryColor : public UOdysseyPaletteEntry
{
    GENERATED_BODY()

public:
    UOdysseyPaletteEntryColor();

public:
    UPROPERTY(EditAnywhere, Category = Palette)
    FLinearColor EntryColor;
};