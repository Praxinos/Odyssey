// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "IDetailCustomization.h"
#include "PropertyHandle.h"
#include "Palette/OdysseyPaletteEntry.h"
#include "Editor/PropertyEditor/Public/DetailCategoryBuilder.h"
#include "Widgets/Input/SComboBox.h"


class FOdysseyVectorObjectViewPaletteCustomization : public IPropertyTypeCustomization
{
public:
    static TSharedRef<IPropertyTypeCustomization> MakeInstance();
    static void Register();
    static void Unregister();

    //virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

    virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle
        , class FDetailWidgetRow& HeaderRow
        , IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

    virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle
        , class IDetailChildrenBuilder& StructBuilder
        , IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:
    bool OnShouldFilterAsset(const FAssetData& AssetData) const;
    void OnEntrySelected(UOdysseyPaletteEntry* SelectedEntry, ESelectInfo::Type SelectInfo);
    void OnPaletteChanged(const FAssetData& AssetData);
    void OnChildPropertyValueChanged(TSharedRef<IPropertyHandle> StructPropertyHandle);

private:
    TSharedPtr<IPropertyHandle> mPaletteEntryHandle;
    TSharedPtr<IPropertyHandle> mPaletteHandle;

    TArray< UOdysseyPaletteEntry* > mPaletteEntries;

    TSharedPtr<SComboBox<UOdysseyPaletteEntry*>> mComboBoxWidget;
};
