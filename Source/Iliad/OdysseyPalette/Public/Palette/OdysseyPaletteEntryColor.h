// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
    /**
     * @brief Delegate called when something changed the color of this entry
     *
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnEntryColorChanged, UOdysseyPaletteEntryColor*)


public:
    UOdysseyPaletteEntryColor();

public:
    static FOnEntryColorChanged& OnEntryColorChanged();

public:
    FColor& GetColor(int iSet);
    void SetColor( FColor iColor, int iSet );

    virtual void AddSet() override;
    virtual void DuplicateSetAt( int iIndex = -1 ) override;

    virtual void RemoveSet(int iIndex = -1) override;

public:
    // UObject overrides
    virtual void PostInitProperties() override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;

protected:
    //Property changed methods
    virtual void EntryColorChanged();

    virtual void PropertyChanged(const FName& iPropertyName);
    virtual void PostPropertyChanged(const FName& iPropertyName);

public:
    UPROPERTY(EditAnywhere, Category=Palette)
    TArray<FColor> EntryColors;
};
