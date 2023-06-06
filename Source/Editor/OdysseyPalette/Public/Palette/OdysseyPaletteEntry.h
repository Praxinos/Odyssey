// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyPaletteEntry.generated.h"

class UOdysseyPalette;

/////////////////////////////////////////////////////
// OdysseyColorPaletteEntry
UCLASS()
class ODYSSEYPALETTE_API UOdysseyPaletteEntry : public UObject
{
    GENERATED_BODY()

public:
    UOdysseyPalette* GetPalette() const;

public:
    UPROPERTY(EditAnywhere, Category = PaletteEntry)
    FName EntryName;

    UPROPERTY(EditDefaultsOnly, Category = PaletteEntryDescription)
    FText EntryTypeName = FText::FromString(TEXT("Unnamed Entry Type"));

    UPROPERTY(EditDefaultsOnly, Category = PaletteEntryDescription)
    FText Description = FText::FromString(TEXT(""));
};