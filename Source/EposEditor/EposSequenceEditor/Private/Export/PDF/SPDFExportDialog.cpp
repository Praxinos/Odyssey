// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/PDF/SPDFExportDialog.h"

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "EditorStyleSet.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
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
#include "Misc/ScopedSlowTask.h"
#include "MovieSceneSequenceVisitor.h"
#include "PropertyEditorModule.h"
#include "SequencerSettings.h"
#include "SPrimaryButton.h"
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

#include "Board/BoardSequence.h"
#include "IEposSequenceEditorToolkit.h"
#include "Export/ExportConverter.h"
#include "Export/ExportStruct.h"
#include "Export/PDF/ExportPDFExporter.h"
#include "Export/PDF/ExportPDFSettings.h"
#include "Export/PDF/PDFDocExportWidget.h"
#include "Export/SceneRenderer.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "Settings/NamingConventionSettings.h"

#define LOCTEXT_NAMESPACE "PDFExportDialog"

//---

SExportPDFSettings::~SExportPDFSettings()
{
    mPDFDocWidget = nullptr;
}

void
SExportPDFSettings::Construct( const FArguments& InArgs, TWeakPtr<ISequencer> iSequencer, FMovieSceneSequenceIDRef iSequenceId )
{
    mSequencer = iSequencer;
    mCurrentSequenceId = iSequenceId;
    mCurrentSequence = mSequencer.Pin()->GetEvaluationTemplate().GetSequence( mCurrentSequenceId );
    mCurrentEposSequence = Cast<UEposMovieSceneSequence>( mCurrentSequence );
    mRootEposSequence = EposSequenceHelpers::GetRootEposSequence( *mSequencer.Pin(), mCurrentSequenceId, mRootEposSequenceId );

    mExportPDFSettings = GetMutableDefault<UExportPDFSettings>();

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

    UClass* pdf_doc_class = mExportPDFSettings->Options.PDFDocWidgetSoftClass.LoadSynchronous();
    if( pdf_doc_class )
    {
        const FText ProgressText = LOCTEXT( "ConstructAndRenderPDFLayout", "Construct and Render PDF Layout..." );
        FScopedSlowTask Progress( 0, ProgressText );
        Progress.MakeDialog();

        UWorld* world = GEditor->GetEditorWorldContext().World();
        mPDFDocWidget = CreateWidget<UPDFDocExportWidget>( world, pdf_doc_class );
        check( mPDFDocWidget );

        FExportStruct image_sequence_struct;
        FExportConverter converter( mSequencer, mRootEposSequence ? mRootEposSequenceId : mCurrentSequenceId, &mExportPDFSettings->Options.MarkSettings, &image_sequence_struct );

        mPDFDocWidget->OnConstructPDFLayout( image_sequence_struct, true );
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
        .Expose( mPDFSlot )
        [
            //SNew( SHorizontalBox )

            //+ SHorizontalBox::Slot()
            //.FillWidth( 1 )
            //[
            //    SNew( SSpacer )
            //]

            //+ SHorizontalBox::Slot()
            //.AutoWidth()
            ////.HAlign( HAlign_Center )
            ////.VAlign( VAlign_Fill )
            //.Expose( mPDFSlot )
            //[
                SNew( SBox )
                .MinAspectRatio( this, &SExportPDFSettings::GetPageRatio )
                .MaxAspectRatio( this, &SExportPDFSettings::GetPageRatio )
                [
                    mPDFDocWidget ? mPDFDocWidget->TakeWidget() : SNullWidget::NullWidget
                ]
            //]

            //+ SHorizontalBox::Slot()
            //.FillWidth( 1 )
            //[
            //    SNew( SSpacer )
            //]
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

FOptionalSize
SExportPDFSettings::GetPageRatio() const
{
    if( !mPDFDocWidget )
        return FMath::Sqrt( 1.f );

    int32 current_page = mPDFDocWidget->GetCurrentPDFPageNumber();

    if( mPDFDocWidget->GetPDFPageFormat( current_page ) == EPDFPageFormat::A4 && mPDFDocWidget->GetPDFPageOrientation( current_page ) == EPDFPageOrientation::Landscape )
        return FMath::Sqrt( 2.f );

    if( mPDFDocWidget->GetPDFPageFormat( current_page ) == EPDFPageFormat::A4 && mPDFDocWidget->GetPDFPageOrientation( current_page ) == EPDFPageOrientation::Portrait )
        return FMath::InvSqrt( 2.f );

    return FMath::Sqrt( 1.f );
}

void
SExportPDFSettings::GlobalSettingsChanged( const FPropertyChangedEvent& iEvent )
{
    mExportPDFSettings->SaveConfig();

    FSlateApplication::Get().DismissAllMenus();

    //---

    mPDFSlot->DetachWidget();
    mPDFDocWidget = nullptr;

    UClass* pdf_doc_class = mExportPDFSettings->Options.PDFDocWidgetSoftClass.LoadSynchronous();
    if( pdf_doc_class )
    {
        const FText ProgressText = LOCTEXT( "ConstructAndRenderPDFLayout", "Construct and Render PDF Layout..." );
        FScopedSlowTask Progress( 0, ProgressText );
        Progress.MakeDialog();

        UWorld* world = GEditor->GetEditorWorldContext().World();
        mPDFDocWidget = CreateWidget<UPDFDocExportWidget>( world, pdf_doc_class );
        check( mPDFDocWidget );

        FExportStruct image_sequence_struct;
        FExportConverter converter( mSequencer, mRootEposSequence ? mRootEposSequenceId : mCurrentSequenceId, &mExportPDFSettings->Options.MarkSettings, &image_sequence_struct );

        mPDFDocWidget->OnConstructPDFLayout( image_sequence_struct, true );

        mPDFSlot->AttachWidget(
            SNew( SBox )
            .MinAspectRatio( this, &SExportPDFSettings::GetPageRatio )
            .MaxAspectRatio( this, &SExportPDFSettings::GetPageRatio )
            [
                mPDFDocWidget->TakeWidget()
            ]
        );
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
    FExportConverter converter( mSequencer, mRootEposSequence ? mRootEposSequenceId : mCurrentSequenceId, &mExportPDFSettings->Options.MarkSettings, &image_sequence_struct );

    FExportPDFExporter exporter( mSequencer, &image_sequence_struct, &mExportPDFSettings->Options );
    exporter.Export();
}

#undef LOCTEXT_NAMESPACE
