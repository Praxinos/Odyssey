// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "FOdysseyVectorObjectViewPaletteCustomization.h"

#include "OdysseyPainterEditorVectorObjectView.h"
#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "Editor.h"
#include "Widgets/Input/SComboBox.h"
#include "UObject/UObjectGlobals.h"
#include "Editor/PropertyEditor/Public/IDetailChildrenBuilder.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SOdysseyPaletteTreeView.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/SOdysseyPaletteSetComboBox.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

TSharedRef<IPropertyTypeCustomization> FOdysseyVectorObjectViewPaletteCustomization::MakeInstance()
{
    return MakeShareable(new FOdysseyVectorObjectViewPaletteCustomization);
}

void FOdysseyVectorObjectViewPaletteCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
    uint32 NumChildren;
    StructPropertyHandle->GetNumChildren(NumChildren);
    mPaletteHandle = StructPropertyHandle->GetChildHandle( GET_MEMBER_NAME_CHECKED(FPaletteEntrySelection, OdysseyPalette) );
    mPaletteEntryHandle = StructPropertyHandle->GetChildHandle( GET_MEMBER_NAME_CHECKED(FPaletteEntrySelection, OdysseyPaletteEntryColor) );
    mPaletteSetHandle = StructPropertyHandle->GetChildHandle( GET_MEMBER_NAME_CHECKED(FPaletteEntrySelection, OdysseyPaletteSet) );

    HeaderRow
    .NameContent()
    [
        StructPropertyHandle->CreatePropertyNameWidget()
    ]
    .ValueContent()
    [
        MakeCurrentColorEntryWidget()
    ];

    mPaletteEntryHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateRaw(this, &FOdysseyVectorObjectViewPaletteCustomization::OnChildPropertyValueChanged, StructPropertyHandle));
    mPaletteSetHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateRaw(this, &FOdysseyVectorObjectViewPaletteCustomization::OnChildPropertyValueChanged, StructPropertyHandle));
    mPaletteHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateRaw(this, &FOdysseyVectorObjectViewPaletteCustomization::OnChildPropertyValueChanged, StructPropertyHandle));
}

TSharedRef<SWidget>
FOdysseyVectorObjectViewPaletteCustomization::MakeCurrentColorEntryWidget()
{
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .Padding(FMargin(0, 0, 4, 0))
        .AutoWidth()
        [
            SNew(SColorBlock )
                .Color(this, &FOdysseyVectorObjectViewPaletteCustomization::GetCurrentEntryColorAsLinear)
        ]
        + SHorizontalBox::Slot()
        [
            SNew(STextBlock)
            .Text_Lambda(
                [this]() -> FText
                {
                    UOdysseyPaletteEntryColor* entry = GetCurrentEntryColor();
                    return entry ? entry->EntryName : LOCTEXT("vector-object-view-palette.selected-entry.none", "None");
                }
            )
        ];
}

void FOdysseyVectorObjectViewPaletteCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
    uint32 NumChildren;
    StructPropertyHandle->GetNumChildren(NumChildren);
    mPaletteHandle = StructPropertyHandle->GetChildHandle( GET_MEMBER_NAME_CHECKED(FPaletteEntrySelection, OdysseyPalette) );
    mPaletteSetHandle = StructPropertyHandle->GetChildHandle( GET_MEMBER_NAME_CHECKED(FPaletteEntrySelection, OdysseyPaletteSet) );
    mPaletteEntryHandle = StructPropertyHandle->GetChildHandle( GET_MEMBER_NAME_CHECKED(FPaletteEntrySelection, OdysseyPaletteEntryColor) );

    StructBuilder.AddProperty(mPaletteHandle->AsShared())
    .CustomWidget()
    .NameContent()
    [
        mPaletteHandle->CreatePropertyNameWidget()
    ]
    .ValueContent()
    [
        SNew(SObjectPropertyEntryBox)
            .PropertyHandle(mPaletteHandle)
            .AllowedClass(UOdysseyPalette::StaticClass())
            .OnObjectChanged(this, &FOdysseyVectorObjectViewPaletteCustomization::OnPaletteChanged)
    ];

    StructBuilder.AddProperty(mPaletteSetHandle->AsShared())
    .CustomWidget()
    .NameContent()
    [
        mPaletteSetHandle->CreatePropertyNameWidget()
    ]
    .ValueContent()
    [
        SNew(SOdysseyPaletteSetComboBox)
        .Palette(this, &FOdysseyVectorObjectViewPaletteCustomization::GetPalette)
        .CurrentSet(this, &FOdysseyVectorObjectViewPaletteCustomization::GetCurrentSet)
        .OnCurrentSetSelected(this, &FOdysseyVectorObjectViewPaletteCustomization::OnPaletteCurrentSetSelected)
    ];

    StructBuilder.AddProperty(mPaletteEntryHandle->AsShared())
    .CustomWidget()
    .NameContent()
    [
        mPaletteEntryHandle->CreatePropertyNameWidget()
    ]
    .ValueContent()
    [
        SNew(SComboButton)
        .OnGetMenuContent(this, &FOdysseyVectorObjectViewPaletteCustomization::GetPaletteEntryMenuContent)
        .ButtonContent()
        [
            MakeCurrentColorEntryWidget()
        ]
    ];

    mPaletteEntryHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateRaw(this, &FOdysseyVectorObjectViewPaletteCustomization::OnChildPropertyValueChanged, StructPropertyHandle));
    mPaletteSetHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateRaw(this, &FOdysseyVectorObjectViewPaletteCustomization::OnChildPropertyValueChanged, StructPropertyHandle));
    mPaletteHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateRaw(this, &FOdysseyVectorObjectViewPaletteCustomization::OnChildPropertyValueChanged, StructPropertyHandle));
}

TSharedRef<SWidget>
FOdysseyVectorObjectViewPaletteCustomization::GetPaletteEntryMenuContent()
{
    return SNew(SBox)
        .MinDesiredWidth(300)
        .MaxDesiredHeight(300)
        [
            SNew(SOdysseyPaletteTreeView)
            .Visibility(this, &FOdysseyVectorObjectViewPaletteCustomization::GetTreeViewVisibility)
            .Palette(this, &FOdysseyVectorObjectViewPaletteCustomization::GetPalette)
            .CurrentColorEntry(this, &FOdysseyVectorObjectViewPaletteCustomization::GetCurrentEntryColor)
            .OnCurrentColorEntryChanged(this, &FOdysseyVectorObjectViewPaletteCustomization::OnPaletteCurrentColorEntryChanged)
        ];
}

EVisibility
FOdysseyVectorObjectViewPaletteCustomization::GetTreeViewVisibility() const
{
    return GetPalette() ? EVisibility::Visible : EVisibility::Collapsed;
}

UOdysseyPalette*
FOdysseyVectorObjectViewPaletteCustomization::GetPalette() const
{
    UObject* palette;
    mPaletteHandle->GetValue(palette);
    return Cast<UOdysseyPalette>(palette);
}

int
FOdysseyVectorObjectViewPaletteCustomization::GetCurrentSet() const
{
    int set;
    mPaletteSetHandle->GetValue(set);
    return set;
}

UOdysseyPaletteEntryColor*
FOdysseyVectorObjectViewPaletteCustomization::GetCurrentEntryColor() const
{
    UObject* entry;
    mPaletteEntryHandle->GetValue(entry);
    return Cast<UOdysseyPaletteEntryColor>(entry);
}

FLinearColor
FOdysseyVectorObjectViewPaletteCustomization::GetCurrentEntryColorAsLinear() const
{
    UOdysseyPaletteEntryColor* entry = GetCurrentEntryColor();
    if (!entry)
        return FLinearColor();
    return FLinearColor( entry->GetColor(GetCurrentSet()) );
}
void
FOdysseyVectorObjectViewPaletteCustomization::OnPaletteCurrentSetSelected(int iSet)
{
    mPaletteSetHandle->SetValue(iSet);
}

void
FOdysseyVectorObjectViewPaletteCustomization::OnPaletteCurrentColorEntryChanged(UOdysseyPaletteEntryColor* iEntry)
{
    mPaletteEntryHandle->SetValue(iEntry);
}

void FOdysseyVectorObjectViewPaletteCustomization::OnPaletteChanged(const FAssetData& AssetData)
{
    UOdysseyPalette* palette = Cast<UOdysseyPalette>(AssetData.GetAsset());
    mPaletteHandle->SetValue(palette);
    mPaletteEntryHandle->SetValue((UObject*)nullptr);
    mPaletteSetHandle->SetValue(0);
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
