// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationSettingsCustomization.h"

#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "IDetailPropertyRow.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "SEnumCombo.h"

#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "OdysseyAnimationSettings.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationSettingsCustomization"

//---

//static
TSharedRef<IPropertyTypeCustomization>
FOdysseyAnimationSettingsCustomization::MakeInstance()
{
    return MakeShareable( new FOdysseyAnimationSettingsCustomization() );
}

void
FOdysseyAnimationSettingsCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    // No header needed (to avoid the collapsing)

    //ioHeaderRow
    //    .NameContent()
    //    [
    //        iStructPropertyHandle->CreatePropertyNameWidget()
    //    ];
}

void
FOdysseyAnimationSettingsCustomization::CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    mSettings = GetEditStruct( iStructPropertyHandle );
    check( mSettings );

    if( mSettings->DefaultLayerClass == UOdysseyAnimationLayerImageRaster::StaticClass() )
    {
        mSelectedDefaultLayerType = EOdysseyAnimationDefaultLayerType::Raster;
    }
    else if( mSettings->DefaultLayerClass == UOdysseyAnimationLayerImageVector::StaticClass() )
    {
        mSelectedDefaultLayerType = EOdysseyAnimationDefaultLayerType::Vector;
    }
    else
    {
        checkNoEntry(); // Must be enclosed in braces
    }

    uint32 num_children;
    FPropertyAccess::Result result = iStructPropertyHandle->GetNumChildren( num_children );

    for( uint32 i = 0; i < num_children; i++ )
    {
        TSharedPtr<IPropertyHandle> handle = iStructPropertyHandle->GetChildHandle( i );
        if( !handle.IsValid() )
            continue;

        if( handle->GetProperty() && handle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED( FOdysseyAnimationSettings, DefaultLayerClass ) )
        {
            mDefaultLayerClassHandle = handle;

            mDefaultLayerClassHandle->SetToolTipText( LOCTEXT( "default-layer-type-tooltip", "Select the type of the default layer type." ) );

            FText search_string = LOCTEXT( "search.default-layer-type", "Default Layer Type" );
            ioChildBuilder.AddCustomRow( search_string )
            .NameContent()
            [
                //mDefaultLayerClassHandle->CreatePropertyNameWidget()
                SNew( STextBlock )
                .Text( LOCTEXT( "default-layer-type", "Layer Type" ) )
                .ToolTipText( mDefaultLayerClassHandle->GetToolTipText() )
                .Font( IDetailLayoutBuilder::GetDetailFont() )
            ]
            .ValueContent()
            [
                SNew( SEnumComboBox, StaticEnum<EOdysseyAnimationDefaultLayerType>() )
                .ContentPadding( 0 )
                .CurrentValue( this, &FOdysseyAnimationSettingsCustomization::GetDefaultLayerType )
                .OnEnumSelectionChanged( this, &FOdysseyAnimationSettingsCustomization::OnDefaultLayerTypeChanged )
            ];
        }
        else
        {
            ioChildBuilder.AddProperty( handle.ToSharedRef() );
        }
    }
}

FOdysseyAnimationSettings*
FOdysseyAnimationSettingsCustomization::GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const
{
    TArray<FOdysseyAnimationSettings*> options;

    if( iStructPropertyHandle->IsValidHandle() )
        iStructPropertyHandle->AccessRawData( reinterpret_cast<TArray<void*>&>( options ) );

    if( options.Num() == 1 )
        return options[0];

    return nullptr;
}

int32
FOdysseyAnimationSettingsCustomization::GetDefaultLayerType() const
{
    return static_cast<int32>( mSelectedDefaultLayerType );
}

void
FOdysseyAnimationSettingsCustomization::OnDefaultLayerTypeChanged( int32 iValue, ESelectInfo::Type iSelectInfo )
{
    mSelectedDefaultLayerType = static_cast<EOdysseyAnimationDefaultLayerType>( iValue );

    switch( mSelectedDefaultLayerType )
    {
        case EOdysseyAnimationDefaultLayerType::Raster: mSettings->DefaultLayerClass = UOdysseyAnimationLayerImageRaster::StaticClass(); break;
        case EOdysseyAnimationDefaultLayerType::Vector: mSettings->DefaultLayerClass = UOdysseyAnimationLayerImageVector::StaticClass(); break;
        default: checkNoEntry(); break;
    }
}

#undef LOCTEXT_NAMESPACE
