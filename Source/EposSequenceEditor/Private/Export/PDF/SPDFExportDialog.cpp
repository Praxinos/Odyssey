// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/PDF/SPDFExportDialog.h"

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Async/Async.h"
#include "EditorStyleSet.h"
#include "Factories/Factory.h"
#include "FrameNumberDetailsCustomization.h"
#include "Framework/Docking/TabManager.h"
#include "HAL/PlatformProcess.h"
#include "HAL/Runnable.h"
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
#include "Widgets/Images/SThrobber.h"
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

DECLARE_DELEGATE_OneParam( FOnConstructPDFLayoutComplete, UPDFDocExportWidget* );

class FConstructPDFLayoutRunnable
    : public FRunnable
{
public:
    FConstructPDFLayoutRunnable( UClass* iWidgetClass, FExportStruct iExportStruct );
    virtual ~FConstructPDFLayoutRunnable();

    void CreateAndStartThread();

    // FRunnable functions
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    virtual void Exit() override;
    // FRunnable

    FOnConstructPDFLayoutComplete OnCompleteDelegate;

    friend class SExportPDFSettings;

protected:
    FRunnableThread* mThread { nullptr };

    UClass* mWidgetClass { nullptr };
    FExportStruct mExportStruct;

    UPDFDocExportWidget* mWidget { nullptr };

    //bool bStopThread = false;
};

//---

FConstructPDFLayoutRunnable::FConstructPDFLayoutRunnable( UClass* iWidgetClass, FExportStruct iExportStruct )
    : mWidgetClass( iWidgetClass )
    , mExportStruct( iExportStruct )
{
}

FConstructPDFLayoutRunnable::~FConstructPDFLayoutRunnable()
{
    if( mThread != nullptr )
    {
        mThread->Kill( true );
        delete mThread;
    }
}

void
FConstructPDFLayoutRunnable::CreateAndStartThread()
{
    mThread = FRunnableThread::Create( this, TEXT( "ConstructPDFLayout" ) );
}

bool FConstructPDFLayoutRunnable::Init()
{
    // First thing to do is set the slate loading thread ID
    // This guarantees all systems know that a slate thread exists
    GSlateLoadingThreadId = FPlatformTLS::GetCurrentThreadId();

    return true;
}

uint32 FConstructPDFLayoutRunnable::Run()
{
    FTaskTagScope Scope( ETaskTag::ESlateThread );

    check( GSlateLoadingThreadId == FPlatformTLS::GetCurrentThreadId() );

    //bStopThread = false;

    if( mWidgetClass )
    {
        UWorld* world = GEditor->GetEditorWorldContext().World();
        mWidget = CreateWidget<UPDFDocExportWidget>( world, mWidgetClass );
        check( mWidget );

        mWidget->OnConstructPDFLayout( mExportStruct, true );
    }

    OnCompleteDelegate.ExecuteIfBound( mWidget );
    mWidget = nullptr;

    // Tear down the slate loading thread ID
    FPlatformAtomics::InterlockedExchange( (int32*)&GSlateLoadingThreadId, 0 );

    // Return success
    return 0;
}


void FConstructPDFLayoutRunnable::Exit()
{
    mWidgetClass = nullptr;
    mWidget = nullptr;
}


void FConstructPDFLayoutRunnable::Stop()
{
    // Force our thread to stop early
    //bStopThread = true;
}

//---

SExportPDFSettings::~SExportPDFSettings()
{
    mPDFDocWidget = nullptr;
}

void
SExportPDFSettings::Construct( const FArguments& InArgs, TWeakPtr<ISequencer> iSequencer, UMovieSceneSequence* iCurrentSequence )
{
    mSequencer = iSequencer;
    mCurrentSequence = iCurrentSequence;
    mRootSequence = iSequencer.Pin()->GetRootMovieSceneSequence();

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

    //FExportStruct image_sequence_struct;
    //FExportConverter converter( mSequencer, mRootSequence, &mExportPDFSettings->Options.MarkSettings, &image_sequence_struct );

    UClass* pdf_doc_class = mExportPDFSettings->Options.PDFDocWidgetSoftClass.LoadSynchronous();
    if( pdf_doc_class )
    {
        //FExportStruct image_sequence_struct;
        //FExportConverter converter( mSequencer, mRootSequence, &mExportPDFSettings->Options.MarkSettings, &image_sequence_struct );

        //TSharedPtr<FConstructPDFLayoutRunnable> runnable = MakeShared<FConstructPDFLayoutRunnable>( pdf_doc_class, image_sequence_struct );

        //auto UpdateWidget = [this]( UPDFDocExportWidget* iWidget )
        //{
        //    mPDFSlot->DetachWidget();
        //    mPDFDocWidget = nullptr;

        //    mPDFDocWidget = iWidget;

        //    mPDFSlot->AttachWidget(
        //        SNew( SBox )
        //        .MinAspectRatio( this, &SExportPDFSettings::GetPageRatio )
        //        .MaxAspectRatio( this, &SExportPDFSettings::GetPageRatio )
        //        [
        //            mPDFDocWidget->TakeWidget()
        //        ]
        //    );
        //};

        //runnable->OnCompleteDelegate = FOnConstructPDFLayoutComplete::CreateLambda( UpdateWidget );

        //runnable->CreateAndStartThread();

        //---

        const FText ProgressText = LOCTEXT( "ConstructAndRenderPDFLayout", "Construct and Render PDF Layout..." );
        FScopedSlowTask Progress( 0, ProgressText );
        Progress.MakeDialog();

        UWorld* world = GEditor->GetEditorWorldContext().World();
        mPDFDocWidget = CreateWidget<UPDFDocExportWidget>( world, pdf_doc_class );
        check( mPDFDocWidget );

        FExportStruct image_sequence_struct;
        FExportConverter converter( mSequencer, mRootSequence, &mExportPDFSettings->Options.MarkSettings, &image_sequence_struct );

        mPDFDocWidget->OnConstructPDFLayout( image_sequence_struct, true );
    }

    //TFuture<void> Result = Async( EAsyncExecution::TaskGraphMainThread,
    //                              [this, pdf_doc_class, image_sequence_struct]()
    //                              {
    //                                  if( pdf_doc_class )
    //                                  {
    //                                      UWorld* world = GEditor->GetEditorWorldContext().World();
    //                                      mPDFDocWidget = CreateWidget<UPDFDocExportWidget>( world, pdf_doc_class );
    //                                      check( mPDFDocWidget );

    //                                      mPDFDocWidget->OnConstructPDFLayout( image_sequence_struct, true );
    //                                  }
    //                              } );


    //uint32 LastProcessed = 0;
    //uint32 max_processed = 200;
    //const FText ProgressText = LOCTEXT( "ConstructPDFLayout", "Construct PDF Layout {0}/{1} ..." );
    //FScopedSlowTask Progress( max_processed, FText::Format( ProgressText, LastProcessed, max_processed ) );
    //Progress.MakeDialog();

    //while( !Result.WaitFor( FTimespan::FromMilliseconds( 33.0 ) ) )
    //{
    //    //uint32 LocalProcessed = Processed.Load( EMemoryOrder::Relaxed );
    //    //Progress.EnterProgressFrame( LocalProcessed - LastProcessed, FText::Format( ProgressText, LocalProcessed, StaticMeshes.Num() ) );
    //    LastProcessed++;
    //    Progress.EnterProgressFrame( 1, FText::Format( ProgressText, LastProcessed, max_processed ) );
    //    //LastProcessed = LocalProcessed;
    //}

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
                //SNew( SThrobber )
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

    //FExportStruct image_sequence_struct;
    //FExportConverter converter( mSequencer, mRootSequence, &mExportPDFSettings->Options.MarkSettings, &image_sequence_struct );

    //UClass* pdf_doc_class = mExportPDFSettings->Options.PDFDocWidgetSoftClass.LoadSynchronous();

    //                                  mPDFSlot->DetachWidget();
    //                                  mPDFDocWidget = nullptr;

    //                                  mPDFSlot->AttachWidget(
    //                                      SNew( SThrobber )
    //                                  );

    //TFuture<void> Result = Async( EAsyncExecution::TaskGraphMainThread,
    //                              [this, pdf_doc_class, image_sequence_struct]()
    //                              {
    //                                  ensure( IsInGameThread() );

    //                                  //---

    //                                  if( pdf_doc_class )
    //                                  {
    //                                      UWorld* world = GEditor->GetEditorWorldContext().World();
    //                                      mPDFDocWidget = CreateWidget<UPDFDocExportWidget>( world, pdf_doc_class );
    //                                      check( mPDFDocWidget );

    //                                      mPDFDocWidget->OnConstructPDFLayout( image_sequence_struct, true );
    //                                  }

    //                                  //---

    //                              } );

    //FPlatformProcess::Sleep( 10 );

    //ensure( IsInGameThread() );

    //uint32 LastProcessed = 0;
    //uint32 max_processed = 200;
    //const FText ProgressText = LOCTEXT( "ConstructPDFLayout", "Construct PDF Layout {0}/{1} ..." );
    //FScopedSlowTask Progress( 0, FText::Format( ProgressText, LastProcessed, max_processed ) );
    //Progress.MakeDialog();

    //while( !Result.WaitFor( FTimespan::FromMilliseconds( 33.0 ) ) )
    //{
    //    //uint32 LocalProcessed = Processed.Load( EMemoryOrder::Relaxed );
    //    //Progress.EnterProgressFrame( LocalProcessed - LastProcessed, FText::Format( ProgressText, LocalProcessed, StaticMeshes.Num() ) );
    //    LastProcessed++;
    //    //Progress.EnterProgressFrame( 1, FText::Format( ProgressText, LastProcessed, max_processed ) );
    //    //LastProcessed = LocalProcessed;
    //}

    //                                  mPDFSlot->DetachWidget();

    //                                  mPDFSlot->AttachWidget(
    //                                      SNew( SBox )
    //                                      .MinAspectRatio( this, &SExportPDFSettings::GetPageRatio )
    //                                      .MaxAspectRatio( this, &SExportPDFSettings::GetPageRatio )
    //                                      [
    //                                          mPDFDocWidget->TakeWidget()
    //                                      ]
    //                                  );

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
        FExportConverter converter( mSequencer, mRootSequence, &mExportPDFSettings->Options.MarkSettings, &image_sequence_struct );

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
    FExportConverter converter( mSequencer, mRootSequence, &mExportPDFSettings->Options.MarkSettings, &image_sequence_struct );

    FExportPDFExporter exporter( mSequencer, &image_sequence_struct, &mExportPDFSettings->Options );
    exporter.Export();
}

#undef LOCTEXT_NAMESPACE
