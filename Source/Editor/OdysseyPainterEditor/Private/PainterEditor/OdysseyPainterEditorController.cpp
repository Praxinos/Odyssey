// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorController.h"

#include "AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#include "Models/OdysseyPainterEditorCommands.h"
#include "PainterEditor/OdysseyPainterEditorGUI.h"
#include "SOdysseyAboutScreen.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "OdysseyPainterEditorStrokeOptionsTab.h"
#include "OdysseyPainterEditorTopTab.h"

#include "Brush/SOdysseyBrushExposedParameters.h"
#include "Brush/SOdysseyBrushSelector.h"
#include "Color/SOdysseyColorSelector.h"
#include "Color/SOdysseyColorSliders.h"
#include "Mesh/SOdysseyMeshSelector.h"
#include "SOdysseyPaintModifiers.h"
#include "SOdysseyStrokeOptions.h"
#include "Dialogs/CustomDialog.h"
#include "UndoHistory/SOdysseyUndoHistory.h"
#include "PainterEditor/OdysseyPainterEditorState.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorController
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorController::~FOdysseyPainterEditorController()
{
}

FOdysseyPainterEditorController::FOdysseyPainterEditorController()
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyPainterEditorController::InitOdysseyPainterEditorController(const TSharedRef<FUICommandList>& iToolkitCommands)
{
    // Add Menu Extender
    //GetMenuExtenders().Add(CreateMenuExtenders(iToolkitCommands));

    // Register our commands. This will only register them if not previously registered
    // FOdysseyPainterEditorCommands::Register();

    // Build commands
    FOdysseyPainterEditorController::BindCommands(iToolkitCommands);

	//Add PaintEngine Callbacks
	// GetEditor()->PaintEngine()->OnPreviewBlockTilesChanged().AddRaw(this, &FOdysseyPainterEditorController::OnPaintEnginePreviewBlockTilesChanged);
    // GetEditor()->PaintEngine()->OnEditedBlockTilesWillChange().AddRaw(this, &FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesWillChange);
    // GetEditor()->PaintEngine()->OnEditedBlockTilesChanged().AddRaw(this, &FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesChanged);
    // GetEditor()->PaintEngine()->OnStrokeAbort().AddRaw(this, &FOdysseyPainterEditorController::OnPaintEngineStrokeAbort);
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Commands building

void
FOdysseyPainterEditorController::BindCommands(const TSharedRef<FUICommandList>& iToolkitCommands)
{
	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().AboutIliad,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnAboutIliad ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().VisitPraxinosWebsite,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnVisitPraxinosWebsite ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().VisitPraxinosForums,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnVisitPraxinosForums ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().SwitchTabletAPI,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorController::OnSwitchTabletAPI ),
        FCanExecuteAction() );
}

TSharedRef<SWidget>
FOdysseyPainterEditorController::GenerateTabletAPIComboBoxItem( TSharedPtr<EOdysseyStylusInputDriver> iItem )
{
	return  SNew(STextBlock)
            .Text( UOdysseyStylusInputSettings::GetFormatText( iItem ) );
}

void 
FOdysseyPainterEditorController::ChangeSelectionTabletAPIComboBoxItem( TSharedPtr<EOdysseyStylusInputDriver> iNewSelection, ESelectInfo::Type iSelectInfo )
{
    mComboBoxTabletAPISelected = iNewSelection;
}

FText 
FOdysseyPainterEditorController::GetComboBoxTabletAPISelectedAsText() const
{
    return UOdysseyStylusInputSettings::GetFormatText( mComboBoxTabletAPISelected );
}



void
FOdysseyPainterEditorController::OnPaintEnginePreviewBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
}

void
FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesWillChange(const TArray<::ul3::FRect>& iChangedTiles)
{
}

void
FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
}

void
FOdysseyPainterEditorController::OnPaintEngineStrokeAbort()
{
}

void
FOdysseyPainterEditorController::OnAboutIliad()
{
    const FText aboutWindowTitle = LOCTEXT( "AboutIliad", "About Iliad" );

    TSharedPtr<SWindow> aboutWindow =
        SNew( SWindow )
        .Title( aboutWindowTitle )
        .ClientSize( FVector2D( 600.f, 460.f ) )
        .SupportsMaximize( false )
        .SupportsMinimize( false )
        .SizingRule( ESizingRule::FixedSize )
        [
            SNew( SOdysseyAboutScreen )
        ];

    TSharedPtr<SDockTab> OwnerTab = GetEditor()->Toolkit()->GetTabManager()->GetOwnerTab();
    TSharedPtr<SWindow> parentWindow = NULL;
    if (OwnerTab.IsValid())
    {
        parentWindow = FSlateApplication::Get().FindWidgetWindow(OwnerTab.ToSharedRef());
    }
    else
    {
        parentWindow = FGlobalTabmanager::Get()->GetRootWindow();
    }

	FSlateApplication::Get().AddModalWindow(aboutWindow.ToSharedRef(), parentWindow);
}

void
FOdysseyPainterEditorController::OnVisitPraxinosWebsite()
{
    FString URL = "https://praxinos.coop/";
    FPlatformProcess::LaunchURL( *URL, NULL, NULL );
}

void
FOdysseyPainterEditorController::OnVisitPraxinosForums()
{
    FString URL = "https://praxinos.coop/forum";
    FPlatformProcess::LaunchURL( *URL, NULL, NULL );
}

void
FOdysseyPainterEditorController::OnSwitchTabletAPI()
{
    UOdysseyStylusInputSettings* settings = GetMutableDefault< UOdysseyStylusInputSettings >();
    mComboBoxTabletAPISelected = MakeShared<EOdysseyStylusInputDriver>(settings->StylusInputDriver);
    TArray<TSharedPtr<EOdysseyStylusInputDriver>> options;

    #if PLATFORM_WINDOWS
        options.Add(MakeShared< EOdysseyStylusInputDriver >(EOdysseyStylusInputDriver::OdysseyStylusInputDriver_Ink));
        options.Add(MakeShared< EOdysseyStylusInputDriver >(EOdysseyStylusInputDriver::OdysseyStylusInputDriver_Wintab));
    #elif PLATFORM_MAC
        options.Add(MakeShared< EOdysseyStylusInputDriver >(EOdysseyStylusInputDriver::OdysseyStylusInputDriver_NSEvent));
    #endif

    options.Add(MakeShared< EOdysseyStylusInputDriver >(EOdysseyStylusInputDriver::OdysseyStylusInputDriver_None));

	TSharedRef<SVerticalBox> dialogContents = SNew(SVerticalBox)
        + SVerticalBox::Slot()
        [
            SNew( STextBlock )
            .Text( FText::FromString("Select tablet API") )
        ]
		+ SVerticalBox::Slot()
		[
			SNew(SComboBox<TSharedPtr<EOdysseyStylusInputDriver>>)
            .OptionsSource(&options)
            .OnGenerateWidget(this, &FOdysseyPainterEditorController::GenerateTabletAPIComboBoxItem)
            .OnSelectionChanged( this, &FOdysseyPainterEditorController::ChangeSelectionTabletAPIComboBoxItem )
            [
                SNew( STextBlock )
                .Text( this, &FOdysseyPainterEditorController::GetComboBoxTabletAPISelectedAsText )
            ]
		];

	TSharedPtr<SCustomDialog> customDialog;

	FText dialogTitle = LOCTEXT("SelectTabletAPI", "Select Tablet API" );

	FText oKText = LOCTEXT("OkSwitchAPI", "OK" );
	FText cancelText = LOCTEXT("CancelSwitchAPI", "Cancel");

	customDialog = SNew(SCustomDialog)
		.Title(dialogTitle)
		.DialogContent(dialogContents)
		.Buttons( { SCustomDialog::FButton(oKText), SCustomDialog::FButton(cancelText) } );

    if( customDialog->ShowModal() == 0/*OK*/ )
    {
        settings->StylusInputDriver = *(mComboBoxTabletAPISelected.Get());
        settings->RefreshStylusInputDriver();
    }
}


#undef LOCTEXT_NAMESPACE
