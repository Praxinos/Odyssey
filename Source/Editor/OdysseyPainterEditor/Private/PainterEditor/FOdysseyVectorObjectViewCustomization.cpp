#include "FOdysseyVectorObjectViewCustomization.h"

#include "OdysseyPainterEditorVectorObjectView.h"
#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "Editor.h"
#include "Widgets/Input/SComboBox.h"
#include "UObject/UObjectGlobals.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

TSharedRef<IDetailCustomization> FOdysseyVectorObjectViewCustomization::MakeInstance()
{
    return MakeShareable(new FOdysseyVectorObjectViewCustomization);
}

void FOdysseyVectorObjectViewCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    mPaletteEntryHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, OdysseyPaletteEntryColor));
    mPaletteHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, OdysseyPalette));

    IDetailCategoryBuilder& category = DetailBuilder.EditCategory("Appearance");

    category.AddCustomRow(LOCTEXT("PaletteEntry", "PaletteEntry"))
        .NameContent()
        [
            mPaletteEntryHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MaxDesiredWidth(500)
        [
            SAssignNew(mComboBoxWidget, SComboBox<UOdysseyPaletteEntry*>)
                .OptionsSource(&mPaletteEntries)
                .OnGenerateWidget_Lambda([](UOdysseyPaletteEntry* entry) -> TSharedRef<SWidget>
                    {
                        return SNew(STextBlock).Text( entry->EntryName );//entry ? entry->EntryName : TEXT("None"));
                    })
                .OnSelectionChanged(this, &FOdysseyVectorObjectViewCustomization::OnEntrySelected)
                        .Content()
                        [
                            SNew(STextBlock)
                                .Text_Lambda([this]() -> FText
                                    {
                                        UObject* objectEntry = nullptr;
                                        mPaletteEntryHandle->GetValue(objectEntry);
                                        UOdysseyPaletteEntry* entry = Cast<UOdysseyPaletteEntry>(objectEntry);
                                        return entry ? entry->EntryName : LOCTEXT("None", "None");
                                    })
                        ]
        ];

    category.AddCustomRow(LOCTEXT("Palette", "Palette"))
        .NameContent()
        [
            mPaletteHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MaxDesiredWidth(500)
        [
            SNew(SObjectPropertyEntryBox)
                .PropertyHandle(mPaletteHandle)
                .AllowedClass(UOdysseyPalette::StaticClass())
                .OnObjectChanged(this, &FOdysseyVectorObjectViewCustomization::OnPaletteChanged)
        ];

}

void FOdysseyVectorObjectViewCustomization::OnEntrySelected(UOdysseyPaletteEntry* SelectedObject, ESelectInfo::Type SelectInfo)
{
    if (SelectedObject->IsValidLowLevel())
    {
        mPaletteEntryHandle->SetValue(SelectedObject);
    }
}

void FOdysseyVectorObjectViewCustomization::OnPaletteChanged(const FAssetData& AssetData)
{
    UOdysseyPalette* palette = Cast<UOdysseyPalette>(AssetData.GetAsset());
    if ( palette )
    {
        mPaletteHandle->SetValue(palette);

        mPaletteEntries.Empty();
        for( int i = 0; i < palette->GetEntries().Num(); i++ )
            mPaletteEntries.Add(palette->GetEntries()[i]);

        mComboBoxWidget->RefreshOptions();

        UOdysseyPaletteEntry* entry = nullptr;
        mPaletteEntryHandle->SetValue(entry);
    }
}

void
FOdysseyVectorObjectViewCustomization::Register()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(UOdysseyPainterEditorVectorObjectView::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FOdysseyVectorObjectViewCustomization::MakeInstance));
    
    PropertyModule.NotifyCustomizationModuleChanged();
}

void
FOdysseyVectorObjectViewCustomization::Unregister()
{
    if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
    {
        // unregister properties when the module is shutdown
        FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
        PropertyModule.UnregisterCustomClassLayout(UOdysseyPainterEditorVectorObjectView::StaticClass()->GetFName());

        PropertyModule.NotifyCustomizationModuleChanged();
    }
}

#undef LOCTEXT_NAMESPACE
