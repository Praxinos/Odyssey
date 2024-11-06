#include "FOdysseyVectorObjectViewPaletteCustomization.h"

#include "OdysseyPainterEditorVectorObjectView.h"
#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "Editor.h"
#include "Widgets/Input/SComboBox.h"
#include "UObject/UObjectGlobals.h"
#include "Editor/PropertyEditor/Public/IDetailChildrenBuilder.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

TSharedRef<IPropertyTypeCustomization> FOdysseyVectorObjectViewPaletteCustomization::MakeInstance()
{
    return MakeShareable(new FOdysseyVectorObjectViewPaletteCustomization);
}

void FOdysseyVectorObjectViewPaletteCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
    TArray<void*> rawPalettePtr;
    StructPropertyHandle->AccessRawData(rawPalettePtr);
    UOdysseyPalette* palette = static_cast<FPaletteEntrySelection*>(rawPalettePtr[0])->OdysseyPalette;

    if (palette)
    {
        mPaletteEntries.Empty();
        for (int i = 0; i < palette->GetEntries().Num(); i++)
        {
            if (palette->GetEntries()[i]->IsA(UOdysseyPaletteEntryColor::StaticClass()))
                mPaletteEntries.Add(palette->GetEntries()[i]);
        }
    }

    HeaderRow.NameContent()
        [
            StructPropertyHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(125.f)
        .MaxDesiredWidth(600.f)
        [
            StructPropertyHandle->CreatePropertyValueWidget()
        ];
}

void FOdysseyVectorObjectViewPaletteCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
    uint32 NumChildren;
    StructPropertyHandle->GetNumChildren(NumChildren);
    mPaletteHandle = StructPropertyHandle->GetChildHandle( 0 );
    mPaletteEntryHandle = StructPropertyHandle->GetChildHandle( 1 );

    StructBuilder.AddProperty(mPaletteHandle->AsShared())
        .CustomWidget()
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
                .OnObjectChanged(this, &FOdysseyVectorObjectViewPaletteCustomization::OnPaletteChanged)
        ];

    StructBuilder.AddProperty(mPaletteEntryHandle->AsShared())
        .CustomWidget()
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
                        return SNew(STextBlock).Text(entry->EntryName);//entry ? entry->EntryName : TEXT("None"));
                    })
                .OnSelectionChanged(this, &FOdysseyVectorObjectViewPaletteCustomization::OnEntrySelected)
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

    mPaletteEntryHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateRaw(this, &FOdysseyVectorObjectViewPaletteCustomization::OnChildPropertyValueChanged, StructPropertyHandle));
    mPaletteHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateRaw(this, &FOdysseyVectorObjectViewPaletteCustomization::OnChildPropertyValueChanged, StructPropertyHandle));

}

void FOdysseyVectorObjectViewPaletteCustomization::OnEntrySelected(UOdysseyPaletteEntry* SelectedObject, ESelectInfo::Type SelectInfo)
{
    if (SelectedObject->IsValidLowLevel())
    {
        mPaletteEntryHandle->SetValue(SelectedObject);

        mPaletteEntries.Empty();
        for (int i = 0; i < SelectedObject->GetPalette()->GetEntries().Num(); i++)
        {
            if (SelectedObject->GetPalette()->GetEntries()[i]->IsA(UOdysseyPaletteEntryColor::StaticClass()))
                mPaletteEntries.Add(SelectedObject->GetPalette()->GetEntries()[i]);
        }

        mComboBoxWidget->RefreshOptions();
    }
}

void FOdysseyVectorObjectViewPaletteCustomization::OnPaletteChanged(const FAssetData& AssetData)
{
    UOdysseyPalette* palette = Cast<UOdysseyPalette>(AssetData.GetAsset());
    if ( palette )
    {
        mPaletteHandle->SetValue(palette);

        mPaletteEntries.Empty();
        for( int i = 0; i < palette->GetEntries().Num(); i++ )
        {
            if( palette->GetEntries()[i]->IsA(UOdysseyPaletteEntryColor::StaticClass()) )
                mPaletteEntries.Add(palette->GetEntries()[i]);
        }

        mComboBoxWidget->RefreshOptions();

        UOdysseyPaletteEntry* entry = nullptr;
        mPaletteEntryHandle->SetValue(entry);
    }
}

void
FOdysseyVectorObjectViewPaletteCustomization::OnChildPropertyValueChanged(TSharedRef<IPropertyHandle> StructPropertyHandle)
{
    FProperty* property = StructPropertyHandle.Get().GetProperty();
    TArray<UObject*> OuterObjects;

    StructPropertyHandle.Get().GetOuterObjects(OuterObjects);

    for (int i = 0; i < OuterObjects.Num(); i++)
    {
        UObject* OuterObject = OuterObjects[0];
        FPropertyChangedEvent propertyChangedEvent = FPropertyChangedEvent(property
            , EPropertyChangeType::ValueSet
            , OuterObjects);

        OuterObject->PostEditChangeProperty(propertyChangedEvent);
    }
}


void
FOdysseyVectorObjectViewPaletteCustomization::Register()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
    PropertyModule.RegisterCustomPropertyTypeLayout( FPaletteEntrySelection::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FOdysseyVectorObjectViewPaletteCustomization::MakeInstance ) );

    PropertyModule.NotifyCustomizationModuleChanged();
}

void
FOdysseyVectorObjectViewPaletteCustomization::Unregister()
{
    if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
    {
        // unregister properties when the module is shutdown
        FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
        PropertyModule.UnregisterCustomPropertyTypeLayout(FPaletteEntrySelection::StaticStruct()->GetFName());

        PropertyModule.NotifyCustomizationModuleChanged();
    }
}



#undef LOCTEXT_NAMESPACE
