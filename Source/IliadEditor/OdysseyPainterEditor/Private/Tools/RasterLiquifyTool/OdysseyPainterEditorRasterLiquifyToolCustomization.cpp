// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorRasterLiquifyToolCustomization.h"
#include "OdysseyPainterEditorRasterLiquifyTool.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "OdysseyStyle.h"
#include "DetailWidgetRow.h"
#include "IPropertyRowGenerator.h"
#include "IPropertyTable.h"
#include "Misc/Attribute.h"
#include "Widgets/Input/SButton.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

UOdysseyPainterEditorRasterLiquifyTool*
FOdysseyPainterEditorRasterLiquifyToolCustomization::GetTool( const IDetailLayoutBuilder& iDetailBuilder )
{
    TArray<TWeakObjectPtr<UObject>> objects;

    iDetailBuilder.GetObjectsBeingCustomized( objects );

    return Cast<UOdysseyPainterEditorRasterLiquifyTool>( objects[0].Get() );
}

TSharedPtr<IPropertyHandle>
FOdysseyPainterEditorRasterLiquifyToolCustomization::GetPropertyHandle( const FName& iPropertyName )
{
    for ( TSharedPtr<IPropertyHandle> propertyHandle : mPropertyHandleArray )
    {
        if( propertyHandle->GetProperty()->GetFName() == iPropertyName )
        {
            return propertyHandle;
        }
    }

    return nullptr;
}

void
FOdysseyPainterEditorRasterLiquifyToolCustomization::CustomizeDetails( IDetailLayoutBuilder& iDetailBuilder )
{
    FPropertyEditorModule& propertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    // Create a category so this is displayed early in the properties
    const TSharedRef<IPropertyTable> propertyTable = propertyModule.CreatePropertyTable();
    TArray<FName> categoryNameArray;

    iDetailBuilder.GetCategoryNames( categoryNameArray );

    mLiquifyTool = GetTool( iDetailBuilder );

    for( FName& categoryName : categoryNameArray )
    {
        IDetailCategoryBuilder& category = iDetailBuilder.EditCategory( categoryName );

        for( TFieldIterator<FProperty> fieldIt( UOdysseyPainterEditorRasterLiquifyTool::StaticClass() ); fieldIt; ++fieldIt )
        {
            const TWeakFieldPtr<FProperty>& property = *fieldIt;
            int32 idx = mPropertyHandleArray.Add( iDetailBuilder.GetProperty( property->GetFName() ).ToSharedPtr() );
            TSharedPtr<SWidget> nameWidget;
            TSharedPtr<SWidget> valueWidget;

            if( property->GetFName() == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorRasterLiquifyTool, Mode ) )
            {
                category.AddCustomRow( FText::FromName( property->GetFName() ) )
                .Visibility ( TAttribute<EVisibility>::CreateSP( this, &FOdysseyPainterEditorRasterLiquifyToolCustomization::GetPropertyVisibility, mPropertyHandleArray[idx] ) )
                .WholeRowContent()
                [
                    CreateModeWidget()
                ];

                iDetailBuilder.HideProperty( mPropertyHandleArray[idx] );
            }
            else
            {
                category.AddProperty(  mPropertyHandleArray[idx] );
            }
        }

        category.AddCustomRow( FText::FromName( GET_FUNCTION_NAME_CHECKED( UOdysseyPainterEditorRasterLiquifyTool, Reset ) ) )
        .WholeRowContent()
        [
            SNew(SButton)
            .Text( FText::FromName( GET_FUNCTION_NAME_CHECKED( UOdysseyPainterEditorRasterLiquifyTool, Reset ) ) )
            .HAlign( EHorizontalAlignment::HAlign_Center )
            .VAlign( EVerticalAlignment::VAlign_Center )
            .IsEnabled_Lambda( [&](){ return ( mLiquifyTool->GetEditingArea( ).Area() == 0 ) ? false : true; } )
            .OnReleased_Lambda( [&](){ mLiquifyTool->Reset( ); } )
        ];
    }
}

EVisibility
FOdysseyPainterEditorRasterLiquifyToolCustomization::GetPropertyVisibility( TSharedPtr<IPropertyHandle> iPropertyHandle ) const
{
    return iPropertyHandle->IsEditable() ? EVisibility::Visible : EVisibility::Hidden;
}

TSharedRef<SWidget>
FOdysseyPainterEditorRasterLiquifyToolCustomization::CreateModeWidget()
{
    TSharedPtr<SSegmentedControl<EOdysseyLiquifyMode>> segmentedControl =
        SNew( SSegmentedControl<EOdysseyLiquifyMode> )
              .SupportsEmptySelection( false )
              .SupportsMultiSelection( false )
              //.UniformPadding( FMargin( 2, 0, 2, 0 ) )
              .UniformPadding( FMargin( 0, 0, 0, 0 ) )
              .Value( this, &FOdysseyPainterEditorRasterLiquifyToolCustomization::GetSelectedMode )
              .OnValueChecked( this, &FOdysseyPainterEditorRasterLiquifyToolCustomization::OnModeSelected );

    for ( EOdysseyLiquifyMode mode : TEnumRange<EOdysseyLiquifyMode>() )
    {
        FText modeName = UEnum::GetDisplayValueAsText(mode);

        segmentedControl->AddSlot( mode )
        .Icon( GetModeIcon( mode ) )
        .ToolTip( modeName );
    }

    return segmentedControl.ToSharedRef();
}

const FSlateBrush*
FOdysseyPainterEditorRasterLiquifyToolCustomization::GetModeIcon( EOdysseyLiquifyMode iMode ) const
{
    switch ( iMode )
    {
        case EOdysseyLiquifyMode::Push: return FOdysseyStyle::GetBrush( "Liquify.Push" );
        case EOdysseyLiquifyMode::Twirl: return FOdysseyStyle::GetBrush( "Liquify.Twirl" );
        case EOdysseyLiquifyMode::Pinch: return FOdysseyStyle::GetBrush( "Liquify.Pinch" );
        case EOdysseyLiquifyMode::Expand: return FOdysseyStyle::GetBrush( "Liquify.Expand" );
        case EOdysseyLiquifyMode::Crystals: return FOdysseyStyle::GetBrush( "Liquify.Crystals" );
        case EOdysseyLiquifyMode::Edge: return FOdysseyStyle::GetBrush( "Liquify.Edge" );
        case EOdysseyLiquifyMode::Reconstruct: return FOdysseyStyle::GetBrush( "Liquify.Reconstruct" );
        case EOdysseyLiquifyMode::Adjust: return FOdysseyStyle::GetBrush( "Liquify.Adjust" );
    }

    return nullptr;
}

EOdysseyLiquifyMode
FOdysseyPainterEditorRasterLiquifyToolCustomization::GetSelectedMode() const
{
    return mLiquifyTool->Mode;
}

void
FOdysseyPainterEditorRasterLiquifyToolCustomization::OnModeSelected( EOdysseyLiquifyMode iMode
                                                                   , ECheckBoxState iState )
{
    if( iState == ECheckBoxState::Checked )
    {
        TSharedPtr<IPropertyHandle> modePropertyHandle = GetPropertyHandle( GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorRasterLiquifyTool, Mode ) );

        modePropertyHandle->SetValue( static_cast<uint8>(iMode) );
    }
}

#undef LOCTEXT_NAMESPACE
