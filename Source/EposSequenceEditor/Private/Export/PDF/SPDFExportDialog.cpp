// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/PDF/SPDFExportDialog.h"

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "EditorStyleSet.h"
#include "Factories/Factory.h"
#include "FrameNumberDetailsCustomization.h"
#include "Framework/Docking/TabManager.h"
#include "IAssetTools.h"
#include "IDetailsView.h"
#include "ImageWrapperHelper.h"
#include "ISequencer.h"
#include "IStructureDetailsView.h"
#include "LevelEditorSequencerIntegration.h"
#include "Math/UnitConversion.h"
#include "MovieSceneSequenceVisitor.h"
#include "PropertyEditorModule.h"
#include "SequencerSettings.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/STileView.h"
#include "SPrimaryButton.h"

#include "Board/BoardSequence.h"
#include "IEposSequenceEditorToolkit.h"
#include "Export/ExportConverter.h"
#include "Export/ExportStruct.h"
#include "Export/PDF/ExportPDFExporter.h"
#include "Export/PDF/ExportPDFSettings.h"
#include "Export/PDF/ExportPDFSheetWidget.h"
#include "Export/SceneRenderer.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "Settings/NamingConventionSettings.h"

/* LevelSequenceEditorHelpers
 *****************************************************************************/

#define LOCTEXT_NAMESPACE "PDFExportDialog"

//---

SExportPDFSettings::~SExportPDFSettings()
{
    mPDFSheetWidget = nullptr;
}

void
SExportPDFSettings::Construct( const FArguments& InArgs, TWeakPtr<ISequencer> iSequencer, UMovieSceneSequence* iCurrentSequence )
{
    mSequencer = iSequencer;
    mCurrentSequence = iCurrentSequence;
    mRootSequence = iSequencer.Pin()->GetRootMovieSceneSequence();

    mExportPDFSettings = GetMutableDefault<UExportPDFSettings>();

    // Force initialize the ratio from the more relevant camera
    mExportPDFSettings->Options.AspectRatio = GetMostRelevantCameraAspectRatio( mSequencer.Pin().Get(), mRootSequence );
    mExportPDFSettings->Options.ImageSize.X = mExportPDFSettings->Options.ImageSize.Y * mExportPDFSettings->Options.AspectRatio;

    FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    //---

    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.bShowOptions = false;
    DetailsViewArgs.bAllowFavoriteSystem = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    //DetailsViewArgs.ViewIdentifier = "NewStoryboardSettings";

    FStructureDetailsViewArgs StructureDetailsViewArgs;

    //---

    {
        mDetailsViewExportPDFSettings = PropertyEditor.CreateDetailView( DetailsViewArgs );
        mDetailsViewExportPDFSettings->OnFinishedChangingProperties().AddSP( this, &SExportPDFSettings::GlobalSettingsChanged );
        mDetailsViewExportPDFSettings->SetObject( mExportPDFSettings );
    }

    //---

    UClass* pdf_sheet_class = mExportPDFSettings->Options.SheetClassPath.TryLoadClass<UExportPDFSheetWidget>();
    if( pdf_sheet_class )
    {
        UWorld* world = GEditor->GetEditorWorldContext().World();
        mPDFSheetWidget = CreateWidget<UExportPDFSheetWidget>( world, pdf_sheet_class );
        check( mPDFSheetWidget );

        FExportStruct image_sequence_struct;
        FExportConverter converter( mSequencer, mRootSequence, &mExportPDFSettings->Options.MarkSettings, &image_sequence_struct );

        mPDFSheetWidget->OnConstructPDFLayout( image_sequence_struct );
    }

    //---

    ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 4, 4, 4, 4 )
        [
            mDetailsViewExportPDFSettings.ToSharedRef()
        ]

        + SVerticalBox::Slot()
        .FillHeight( 1.0f )
        .Padding( 4, 4, 4, 4 )
        [
            SAssignNew( mBorderWhiteBackground, SBorder )
            .BorderImage( FCoreStyle::Get().GetBrush( "WhiteTexture" ) )
            .Padding( 0 )
            [
                mPDFSheetWidget ? mPDFSheetWidget->TakeWidget() : SNullWidget::NullWidget
            ]
        ]
    ];
}

//---

void
SExportPDFSettings::AddReferencedObjects( FReferenceCollector& Collector ) //override
{
    Collector.AddReferencedObject( mExportPDFSettings );
}

FString
SExportPDFSettings::GetReferencerName() const //override
{
    return "SExportPDFSettings";
}

//---

void
SExportPDFSettings::GlobalSettingsChanged( const FPropertyChangedEvent& iEvent )
{
    mExportPDFSettings->SaveConfig();

    //---

    mBorderWhiteBackground->ClearContent();
    mPDFSheetWidget = nullptr;

    UClass* pdf_sheet_class = mExportPDFSettings->Options.SheetClassPath.TryLoadClass<UExportPDFSheetWidget>();
    if( pdf_sheet_class )
    {
        UWorld* world = GEditor->GetEditorWorldContext().World();
        mPDFSheetWidget = CreateWidget<UExportPDFSheetWidget>( world, pdf_sheet_class );
        check( mPDFSheetWidget );

        FExportStruct image_sequence_struct;
        FExportConverter converter( mSequencer, mRootSequence, &mExportPDFSettings->Options.MarkSettings, &image_sequence_struct );

        mPDFSheetWidget->OnConstructPDFLayout( image_sequence_struct );

        mBorderWhiteBackground->SetContent( mPDFSheetWidget->TakeWidget() );
    }
}

FText
SExportPDFSettings::GetFullPath() const
{
    FString FullPath = mExportPDFSettings->Options.ExportPath.Path;
    FullPath /= mExportPDFSettings->Options.ExportFile;
    FullPath += TEXT( ".pdf" );

    return FText::FromString( FullPath );
}

FText
SExportPDFSettings::GetErrorText() const
{
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

    if( mExportPDFSettings->Options.ExportPath.Path.IsEmpty() )
        return LOCTEXT( "StoryboardEmptyPath", "Error: Empty Storyboard Path" );

    if( mExportPDFSettings->Options.ExportFile.IsEmpty() )
        return LOCTEXT( "StoryboardEmptyFile", "Error: Empty Storyboard File" );

    return FText::GetEmpty();
}

FText
SExportPDFSettings::GetWarningText() const
{
    return FText::GetEmpty();
}

bool
SExportPDFSettings::CanExportStoryboard() const
{
    if( !GetErrorText().IsEmpty() )
        return false;

    if( mExportPDFSettings->Options.ExportPath.Path.IsEmpty() )
        return false;

    if( mExportPDFSettings->Options.ExportFile.IsEmpty() )
        return false;

    return true;
}

void
SExportPDFSettings::ExportStoryboard()
{
    FExportStruct image_sequence_struct;
    FExportConverter converter( mSequencer, mRootSequence, &mExportPDFSettings->Options.MarkSettings, &image_sequence_struct );

    FExportPDFExporter exporter( mSequencer, &image_sequence_struct, &mExportPDFSettings->Options );
    exporter.Export();
}

#undef LOCTEXT_NAMESPACE
