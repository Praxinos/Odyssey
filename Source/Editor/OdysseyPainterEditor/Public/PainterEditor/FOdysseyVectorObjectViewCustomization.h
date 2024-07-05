#pragma once

#include "IDetailCustomization.h"
#include "PropertyHandle.h"
#include "Palette/OdysseyPaletteEntry.h"
#include "Editor/PropertyEditor/Public/DetailCategoryBuilder.h"

class FOdysseyVectorObjectViewCustomization : public IDetailCustomization
{
public:
    static TSharedRef<IDetailCustomization> MakeInstance();
    static void Register();
    static void Unregister();

    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
    bool OnShouldFilterAsset(const FAssetData& AssetData) const;
    void OnEntrySelected(UOdysseyPaletteEntry* SelectedEntry, ESelectInfo::Type SelectInfo);
    void OnPaletteChanged(const FAssetData& AssetData);

    TSharedPtr<IPropertyHandle> mPaletteEntryHandle;
    TSharedPtr<IPropertyHandle> mPaletteHandle;

    TArray< UOdysseyPaletteEntry* > mPaletteEntries;

    TSharedPtr<SComboBox<UOdysseyPaletteEntry*>> mComboBoxWidget;
};
