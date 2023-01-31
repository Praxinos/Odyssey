// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyTabletAPISwitcher.h"

#include "OdysseyStylusInputSettings.h"
#include "Dialog/SCustomDialog.h"

#define LOCTEXT_NAMESPACE "TabletAPISwitcher"

//---

void
SOdysseyTabletAPISwitcher::Open()
{
    UOdysseyStylusInputSettings* settings = GetMutableDefault< UOdysseyStylusInputSettings >();

    TSharedPtr<SOdysseyTabletAPISwitcher> apiSwitcher = SNew(SOdysseyTabletAPISwitcher);

	TSharedPtr<SCustomDialog> customDialog;

	FText dialogTitle = LOCTEXT("SelectTabletAPI", "Select Tablet API" );
	FText oKText = LOCTEXT("OkSwitchAPI", "OK" );
	FText cancelText = LOCTEXT("CancelSwitchAPI", "Cancel");

    customDialog = SNew( SCustomDialog )
        .Title( dialogTitle )
        .Buttons( { SCustomDialog::FButton( oKText ), SCustomDialog::FButton( cancelText ) } )
        .Content()
        [
            apiSwitcher.ToSharedRef()
        ];

    if( customDialog->ShowModal() == 0/*OK*/ )
    {
        settings->StylusInputDriver = *(apiSwitcher->TabletAPISelected().Get());
        settings->RefreshStylusInputDriver();
    }
}

//---

void
SOdysseyTabletAPISwitcher::Construct( const FArguments& iArgs )
{
    UOdysseyStylusInputSettings* settings = GetMutableDefault< UOdysseyStylusInputSettings >();
    mTabletAPISelected = MakeShared<EOdysseyStylusInputDriver>(settings->StylusInputDriver);

    #if PLATFORM_WINDOWS
        mOptions.Add(MakeShared< EOdysseyStylusInputDriver >(EOdysseyStylusInputDriver::OdysseyStylusInputDriver_Ink));
        mOptions.Add(MakeShared< EOdysseyStylusInputDriver >(EOdysseyStylusInputDriver::OdysseyStylusInputDriver_Wintab));
    #elif PLATFORM_MAC
        mOptions.Add(MakeShared< EOdysseyStylusInputDriver >(EOdysseyStylusInputDriver::OdysseyStylusInputDriver_NSEvent));
    #endif

    mOptions.Add(MakeShared< EOdysseyStylusInputDriver >(EOdysseyStylusInputDriver::OdysseyStylusInputDriver_None));

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
			SNew(SComboBox<TSharedPtr<EOdysseyStylusInputDriver>>)
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

TSharedPtr<EOdysseyStylusInputDriver>
SOdysseyTabletAPISwitcher::TabletAPISelected()
{
    return mTabletAPISelected;
}

TSharedRef<SWidget>
SOdysseyTabletAPISwitcher::GenerateTabletAPIComboBoxItem( TSharedPtr<EOdysseyStylusInputDriver> iItem )
{
	return  SNew(STextBlock)
            .Text( UOdysseyStylusInputSettings::GetFormatText( iItem ) );
}

void 
SOdysseyTabletAPISwitcher::ChangeSelectionTabletAPIComboBoxItem( TSharedPtr<EOdysseyStylusInputDriver> iNewSelection, ESelectInfo::Type iSelectInfo )
{
    mTabletAPISelected = iNewSelection;
}

FText 
SOdysseyTabletAPISwitcher::GetComboBoxTabletAPISelectedAsText() const
{
    return UOdysseyStylusInputSettings::GetFormatText( mTabletAPISelected );
}

#undef LOCTEXT_NAMESPACE
