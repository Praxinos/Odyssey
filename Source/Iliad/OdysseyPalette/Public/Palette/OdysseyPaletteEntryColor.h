// IDDN.FR.001.060015.014.S.X.2019.000.00000
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
    FColor& GetColor( FGuid iSet);
    void SetColor( FColor iColor, FGuid iSet );

    virtual void AddSet( FGuid iNewId ) override;
    virtual void DuplicateSetAt( FGuid iIndexToCopy, FGuid iNewId ) override;

    virtual void RemoveSet( FGuid iIndex ) override;

public:
    // UObject overrides
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
#endif //WITH_EDITOR

protected:
    //Property changed methods
    virtual void EntryColorChanged();

    virtual void PropertyChanged(const FName& iPropertyName);
    virtual void PostPropertyChanged(const FName& iPropertyName);

public:
    // Legacy, to delete next version
    UPROPERTY()
    TArray<FColor> EntryColors;
    //--

    UPROPERTY(EditAnywhere, Category = Palette)
    TMap<FGuid, FColor> EntryColorsIDs;
};
