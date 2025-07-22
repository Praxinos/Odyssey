// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "FOdysseyVectorObjectViewPaletteCustomization.h"

#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorVectorBucketView.h"
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
#include "OdysseyAnimation.h"
#include "OdysseyTextureLayerStackUserData.h"

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
    mPaletteEntryHandle = StructPropertyHandle->GetChildHandle( GET_MEMBER_NAME_CHECKED(FPaletteEntrySelection, OdysseyPaletteEntryColor) );

    //Not the best, but we need to know which palettes are loaded by the editor to filter the assets in the SObjectPropertyEntryBox below
    TArray<UObject*> OuterObjects;
    mEditor = nullptr;
    FOnShouldFilterAsset filterPalette;

    StructPropertyHandle->GetOuterObjects(OuterObjects);
    if( OuterObjects.Num() > 0 && OuterObjects[0]->IsA(UOdysseyPainterEditorVectorObjectView::StaticClass() ) )
    {
        UOdysseyPainterEditorVectorObjectView* view = Cast<UOdysseyPainterEditorVectorObjectView>(OuterObjects[0]);
        mEditor = view->GetEditor();
    }
    else if( OuterObjects.Num() > 0 && OuterObjects[0]->IsA(UOdysseyPainterEditorVectorBucketView::StaticClass()))
    {
        UOdysseyPainterEditorVectorBucketView* view = Cast<UOdysseyPainterEditorVectorBucketView>(OuterObjects[0]);
        mEditor = view->GetEditor();
    }

    if(mEditor)
    {

        TArray<UOdysseyPalette*> palettesAlreadyLoaded;
        for (UOdysseyPaletteSet* set : mEditor->GetPaletteSets())
        {
            palettesAlreadyLoaded.Add(set->mPalette);
        }

        filterPalette = FOnShouldFilterAsset::CreateLambda(
            [palettesAlreadyLoaded](const FAssetData& AssetData)
            {
                for (UObject* palette : palettesAlreadyLoaded)
                {
                    if (!palette)
                        continue;

                    if ( FAssetData(palette).GetSoftObjectPath() == AssetData.GetSoftObjectPath())
                        return false;
                }

                return true;
            });
    }
    // ---

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
            .OnShouldFilterAsset(filterPalette)
            .OnObjectChanged(this, &FOdysseyVectorObjectViewPaletteCustomization::OnPaletteChanged)
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
    mPaletteHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateRaw(this, &FOdysseyVectorObjectViewPaletteCustomization::OnChildPropertyValueChanged, StructPropertyHandle));
}

FGuid
FOdysseyVectorObjectViewPaletteCustomization::GetCurrentSet() const
{
    UOdysseyAnimation* animation = mEditor->GetAnimation();
    UOdysseyTextureLayerStackUserData* userData = mEditor->GetTextureUserData();

    if( animation )
    {
        for( int32 i = 0; i < animation->Palettes.Num(); i++ )
        {
            if( animation->Palettes[i]->mPalette == GetPalette() )
            {
                return animation->Palettes[i]->mSet;
            }
        }
    }
    else
    if( userData )
    {
        for( int32 i = 0; i < userData->Palettes.Num(); i++ )
        {
            if( userData->Palettes[i]->mPalette == GetPalette() )
            {
                return userData->Palettes[i]->mSet;
            }
        }
    }

    return FGuid();
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
            .Set(this, &FOdysseyVectorObjectViewPaletteCustomization::GetCurrentSet)
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
FOdysseyVectorObjectViewPaletteCustomization::OnPaletteCurrentColorEntryChanged(UOdysseyPaletteEntryColor* iEntry)
{
    mPaletteEntryHandle->SetValue(iEntry);
}

void FOdysseyVectorObjectViewPaletteCustomization::OnPaletteChanged(const FAssetData& AssetData)
{
    UOdysseyPalette* palette = Cast<UOdysseyPalette>(AssetData.GetAsset());
    mPaletteHandle->SetValue(palette);
    mPaletteEntryHandle->SetValue((UObject*)nullptr);
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
