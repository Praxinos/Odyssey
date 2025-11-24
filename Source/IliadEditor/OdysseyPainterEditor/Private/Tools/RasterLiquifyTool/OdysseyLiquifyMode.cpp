// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyLiquifyMode.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "OdysseyStyle.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"

class FOdysseyLiquifyModeDetailCustomization : public IPropertyTypeCustomization
{
public:
    /** IPropertyTypeCustomization interface */
    virtual void CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;
    virtual void CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;

private:
    FOdysseyLiquifyMode* GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const;

    EOdysseyLiquifyMode GetSelectedMode() const;
    void OnModeSelected(EOdysseyLiquifyMode iShape, ECheckBoxState iState);
    const FSlateBrush* GetModeIcon(EOdysseyLiquifyMode iMode) const;

private:
    TSharedPtr<IPropertyHandle> mModePropertyHandle;
    TSharedPtr<IPropertyHandle> mEnumValuePropertyHandle;
};

void
FOdysseyLiquifyModeDetailCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle
                                                       , FDetailWidgetRow& ioHeaderRow
                                                       , IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    // No header needed (to avoid the collapsing)
}

void
FOdysseyLiquifyModeDetailCustomization::CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle
                                                         , IDetailChildrenBuilder& ioChildBuilder
                                                         , IPropertyTypeCustomizationUtils& ioStructCustomizationUtils )
{
    mModePropertyHandle = iStructPropertyHandle;
    mEnumValuePropertyHandle = iStructPropertyHandle.Get().GetChildHandle( GET_MEMBER_NAME_CHECKED( FOdysseyLiquifyMode, Value ) );

    TSharedPtr<SSegmentedControl<EOdysseyLiquifyMode>> segmentedControl =
        SNew( SSegmentedControl<EOdysseyLiquifyMode> )
              .SupportsEmptySelection( false )
              .SupportsMultiSelection( false )
              .UniformPadding( FMargin( 2, 0, 2, 0 ) )
              .Value( this, &FOdysseyLiquifyModeDetailCustomization::GetSelectedMode)
              .OnValueChecked(this, &FOdysseyLiquifyModeDetailCustomization::OnModeSelected);

    for ( EOdysseyLiquifyMode mode : TEnumRange<EOdysseyLiquifyMode>() )
    {
        FText modeName = UEnum::GetDisplayValueAsText(mode);

        segmentedControl->AddSlot( mode )
        .Icon( GetModeIcon( mode ) )
        .ToolTip( modeName );
    }

    ioChildBuilder.AddCustomRow( FText() )
    [
        segmentedControl.ToSharedRef()
    ];
}

FOdysseyLiquifyMode*
FOdysseyLiquifyModeDetailCustomization::GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const
{
    TArray<FOdysseyLiquifyMode*> editStruct;

    if( iStructPropertyHandle->IsValidHandle() )
        iStructPropertyHandle->AccessRawData( reinterpret_cast<TArray<void*>&>( editStruct ) );

    if( editStruct.Num() == 1 )
        return editStruct[0];

    return nullptr;
}

const FSlateBrush*
FOdysseyLiquifyModeDetailCustomization::GetModeIcon( EOdysseyLiquifyMode iMode ) const
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
FOdysseyLiquifyModeDetailCustomization::GetSelectedMode() const
{
    uint8 value;

    mEnumValuePropertyHandle->GetValue( value );

    return static_cast<EOdysseyLiquifyMode>(value);
}

void
FOdysseyLiquifyModeDetailCustomization::OnModeSelected( EOdysseyLiquifyMode iMode
                                                      , ECheckBoxState iState )
{
    mEnumValuePropertyHandle->SetValue( static_cast<uint8>(iMode) );
}

void
FOdysseyLiquifyMode::RegisterDetailCustomization()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomPropertyTypeLayout(
        FOdysseyLiquifyMode::StaticStruct()->GetFName(),
        FOnGetPropertyTypeCustomizationInstance::CreateLambda(
            []()
            {
                return MakeShareable( new FOdysseyLiquifyModeDetailCustomization() );
            }
        )
    );
}

void
FOdysseyLiquifyMode::UnregisterDetailCustomization()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.UnregisterCustomPropertyTypeLayout( FOdysseyLiquifyMode::StaticStruct()->GetFName() );
}

FOdysseyLiquifyMode::FOdysseyLiquifyMode()
    : Value(EOdysseyLiquifyMode::Push)
{
}

EOdysseyLiquifyMode
FOdysseyLiquifyMode::Get() const
{
    return Value;
}
