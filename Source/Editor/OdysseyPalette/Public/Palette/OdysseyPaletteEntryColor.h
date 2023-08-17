// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Palette/OdysseyPaletteEntry.h"

#include "OdysseyPaletteEntryColor.generated.h"

/////////////////////////////////////////////////////
// OdysseyColorPaletteEntryColor
UCLASS()
class ODYSSEYPALETTE_API UOdysseyPaletteEntryColor : public UOdysseyPaletteEntry
{
    GENERATED_BODY()

public:
    UOdysseyPaletteEntryColor();

    FColor& GetUsedColor();
    void SetUsedColor( FColor iColor );

    virtual void AddSet() override;
    virtual void DuplicateSetAt( int iIndex = -1 ) override;

    virtual void RemoveSet(int iIndex = -1) override;

public:
    UPROPERTY(EditAnywhere, Category = Palette)
    TArray<FColor> EntryColors;
};