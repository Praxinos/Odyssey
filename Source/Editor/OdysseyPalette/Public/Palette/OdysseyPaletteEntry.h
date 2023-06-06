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
    /**
     * @brief Delegate called when something changed the result of RenderImage()
     * 
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnNameChanged, UOdysseyPaletteEntry*)

    /**
     * @brief Delegate called when something changed the result of RenderImage()
     * 
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsActivatedChanged, UOdysseyPaletteEntry*)

    /* IsExpandedChanged
     * - concerned Child 
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsExpandedChanged, UOdysseyPaletteEntry*);

    /* ParentChanged
     * - concerned Child
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnParentChanged, UOdysseyPaletteEntry*);

    /* ChildrenChanged
     * - concerned Child
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnChildrenChanged, UOdysseyPaletteEntry*);


public:
    static FOnNameChanged& OnNameChanged();
    static FOnIsActivatedChanged& OnIsActivatedChanged();
    static FOnIsExpandedChanged& OnIsExpandedChanged();
    static FOnParentChanged& OnParentChanged();
    static FOnChildrenChanged& OnChildrenChanged();

public:
    UFUNCTION(BlueprintPure, Category = PaletteEntry)
    UOdysseyPalette* GetPalette() const;

protected:
    //Property changed methods
    virtual void NameChanged();
    virtual void IsActivatedChanged();
    virtual void IsExpandedChanged();
    virtual void ParentChanged();
    virtual void ChildrenChanged();

    virtual void PropertyChanged(const FName& iPropertyName);

public:
    // UObject overrides
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;

public:
    UPROPERTY(EditAnywhere, Category = PaletteEntry)
    FText EntryName;

    UPROPERTY(EditDefaultsOnly, Category = PaletteEntryDescription)
    FText EntryTypeName = FText::FromString(TEXT("Unnamed Entry Type"));

    UPROPERTY(EditDefaultsOnly, Category = PaletteEntryDescription)
    FText Description = FText::FromString(TEXT(""));

    //Defaults Properties
    UPROPERTY(EditDefaultsOnly, Category = PaletteEntry)
    bool CanHaveChildren = false;

    UPROPERTY()
    UOdysseyPaletteEntry* Parent;

    UPROPERTY()
    TArray<UOdysseyPaletteEntry*> Children;
};