// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyTabletAPISwitcher.h"

#include "Dialog/SCustomDialog.h"
#include "StylusInputInterface.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SComboBox.h"

#include "OdysseyStylusInputSettings.h"

#define LOCTEXT_NAMESPACE "Widgets"

//---

void
SOdysseyTabletAPISwitcher::Open()
{
    TSharedPtr<SOdysseyTabletAPISwitcher> apiSwitcher = SNew(SOdysseyTabletAPISwitcher);

    TSharedPtr<SCustomDialog> customDialog;

    FText dialogTitle = LOCTEXT("tablet-api-switcher.select-tablet-api", "Select Tablet API" );
    FText oKText = LOCTEXT("tablet-api-switcher.ok", "OK" );
    FText cancelText = LOCTEXT("tablet-api-switcher.cancel", "Cancel");

    customDialog = SNew( SCustomDialog )
        .Title( dialogTitle )
        .Buttons( { SCustomDialog::FButton( oKText ), SCustomDialog::FButton( cancelText ) } )
        .Content()
        [
            apiSwitcher.ToSharedRef()
        ];

    if( customDialog->ShowModal() == 0/*OK*/ )
    {
        UOdysseyStylusInputSettings* settings = GetMutableDefault<UOdysseyStylusInputSettings>();
        settings->StylusInputDriver = apiSwitcher->TabletAPISelected();
        settings->SaveConfig();
        UOdysseyStylusInputSettings::OnStylusInputDriverChanged.Broadcast(settings->StylusInputDriver);
    }
}

//---

void
SOdysseyTabletAPISwitcher::Construct( const FArguments& iArgs )
{
    const UOdysseyStylusInputSettings* settings = GetDefault<UOdysseyStylusInputSettings>();
    mTabletAPISelected = settings->GetStylusDriver();

    mOptions = UE::StylusInput::GetAvailableInterfaces();

    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        [
            SNew( STextBlock )
            .Text( FText::FromString("Select tablet API") )
        ]
        + SVerticalBox::Slot()
        [
            SNew(SComboBox<FName>)
            .OptionsSource(&mOptions)
            .OnGenerateWidget(this, &SOdysseyTabletAPISwitcher::GenerateTabletAPIComboBoxItem)
            .OnSelectionChanged( this, &SOdysseyTabletAPISwitcher::ChangeSelectionTabletAPIComboBoxItem )
            [
                SNew( STextBlock )
                .Text( this, &SOdysseyTabletAPISwitcher::GetComboBoxTabletAPISelectedAsText )
            ]
        ]
    ];
}

FName
SOdysseyTabletAPISwitcher::TabletAPISelected()
{
    return mTabletAPISelected;
}

TSharedRef<SWidget>
SOdysseyTabletAPISwitcher::GenerateTabletAPIComboBoxItem( FName iItem )
{
    return SNew( STextBlock )
           .Text( UOdysseyStylusInputSettings::GetFormatText(iItem) );
}

void
SOdysseyTabletAPISwitcher::ChangeSelectionTabletAPIComboBoxItem( FName iNewSelection, ESelectInfo::Type iSelectInfo )
{
    mTabletAPISelected = iNewSelection;
}

FText
SOdysseyTabletAPISwitcher::GetComboBoxTabletAPISelectedAsText() const
{
    return UOdysseyStylusInputSettings::GetFormatText(mTabletAPISelected);
}

#undef LOCTEXT_NAMESPACE
