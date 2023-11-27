// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/ExportDialog.h"

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
#include "ILevelSequenceEditorToolkit.h"
#include "EposSequenceEditorToolkit.h"
#include "Export/ExportConverter.h"
#include "Export/ExportStruct.h"
#include "Export/ImageSequence/SImageSequenceExportDialog.h"
#include "Export/PDF/SPDFExportDialog.h"
#include "Export/SceneRenderer.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "Settings/NamingConventionSettings.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"

/* LevelSequenceEditorHelpers
 *****************************************************************************/

#define LOCTEXT_NAMESPACE "ImageSequenceExportDialog"

//---

enum class EExportTab
{
    kImageSequence,
    kPDF,
};

class SExportStoryboardSettings
    : public SCompoundWidget
{
    SLATE_BEGIN_ARGS( SExportStoryboardSettings )
        : _DefaultTab( EExportTab::kImageSequence )
        {}
        SLATE_ARGUMENT( TSharedPtr<SWindow>, ParentWindow )
        SLATE_ARGUMENT( EExportTab, DefaultTab )
        SLATE_NAMED_SLOT( FArguments, ImageSequenceSettings )
        SLATE_NAMED_SLOT( FArguments, PDFSettings )
    SLATE_END_ARGS()

    void Construct( const FArguments& InArgs );

private:
    virtual FReply OnKeyDown( const FGeometry& iMyGeometry, const FKeyEvent& iKeyEvent ) override;

private:
    int32 GetActiveTabIndex() const;

    FText GetFullPath() const;
    FText GetErrorText() const;
    FText GetWarningText() const;

    bool CanExportStoryboard() const;
    FReply OnExportStoryboard();

private:
    TWeakPtr<SWindow>           mParentWindow;

    EExportTab                  mActiveTab;

    TSharedPtr<SExportImageSequenceSettings>    mImageSequenceWidget;
    TSharedPtr<SExportPDFSettings>              mPDFWidget;
};

void
SExportStoryboardSettings::Construct( const FArguments& InArgs )
{
    mParentWindow = InArgs._ParentWindow;
    check( mParentWindow.IsValid() );

    mImageSequenceWidget = StaticCastSharedRef<SExportImageSequenceSettings>( InArgs._ImageSequenceSettings.Widget );
    mPDFWidget = StaticCastSharedRef<SExportPDFSettings>( InArgs._PDFSettings.Widget );

    //---

    TSharedRef<SPrimaryButton> export_panels = SNew( SPrimaryButton )
                                               .Text( LOCTEXT( "ExportStoryboard", "Export Panels" ) )
                                               .IsEnabled( this, &SExportStoryboardSettings::CanExportStoryboard )
                                               .OnClicked( this, &SExportStoryboardSettings::OnExportStoryboard );

    mParentWindow.Pin().Get()->SetWidgetToFocusOnActivate( export_panels );

    //---

    ChildSlot
    [
        SNew(SVerticalBox)

        // Tabs
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 4, 4, 4, 4 )
        [
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            .FillWidth( .5f )
            [
                SNew( SSpacer )
            ]
            + SHorizontalBox::Slot()
            .HAlign( HAlign_Fill )
            .Padding( FMargin( 0.f, 1.0f, 1.0f, 0.0f ) )
            [
                SNew(SCheckBox)
                .Style( FAppStyle::Get(),  "ToolPalette.DockingTab" )
                .Padding( 7.f )
                .HAlign( HAlign_Center )
                .OnCheckStateChanged_Lambda( [this] (const ECheckBoxState) { mActiveTab = EExportTab::kImageSequence; } )
                .IsChecked_Lambda( [this] () -> ECheckBoxState { return mActiveTab == EExportTab::kImageSequence ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; } )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "export-storyboard.tabs.export-image-sequence", "Image Sequence" ) )
                ]
            ]
            + SHorizontalBox::Slot()
            .HAlign( HAlign_Fill )
            .Padding( FMargin( 0.f, 1.0f, 1.0f, 0.0f ) )
            [
                SNew(SCheckBox)
                .Style( FAppStyle::Get(),  "ToolPalette.DockingTab" )
                .Padding( 7.f )
                .HAlign( HAlign_Center )
                .OnCheckStateChanged_Lambda( [this] (const ECheckBoxState) { mActiveTab = EExportTab::kPDF; } )
                .IsChecked_Lambda( [this] () -> ECheckBoxState { return mActiveTab == EExportTab::kPDF ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; } )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "export-storyboard.tabs.export-pdf", "PDF" ) )
                ]
            ]
            + SHorizontalBox::Slot()
            .FillWidth( .5f )
            [
                SNew( SSpacer )
            ]
        ]

        + SVerticalBox::Slot()
        .FillHeight( 1.0f )
        .Padding(4, 4, 4, 4)
        [
            SNew( SWidgetSwitcher )
            .WidgetIndex( this, &SExportStoryboardSettings::GetActiveTabIndex )

            + SWidgetSwitcher::Slot()
            [
                mImageSequenceWidget.ToSharedRef()
            ]

            + SWidgetSwitcher::Slot()
            [
                mPDFWidget.ToSharedRef()
            ]
        ]

        //---

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Fill )
        .VAlign( VAlign_Bottom )
        .Padding( 10.f, 4.f )
        [
            SNew(STextBlock)
            .Text( this, &SExportStoryboardSettings::GetFullPath )
            .AutoWrapText( true )
            .Justification( ETextJustify::Right )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Fill )
        .Padding( 10.f, 4.f )
        [
            SNew(STextBlock)
            .Text(this, &SExportStoryboardSettings::GetErrorText)
            .TextStyle( FAppStyle::Get(), TEXT("Log.Error") )
            .Visibility_Lambda( [this]() { return GetErrorText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible; } )
            .Justification( ETextJustify::Right )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Fill )
        .Padding( 10.f, 4.f )
        [
            SNew( STextBlock )
            .Text(this, &SExportStoryboardSettings::GetWarningText)
            .TextStyle( FAppStyle::Get(), TEXT("Log.Warning") )
            .Visibility_Lambda( [this]() { return GetWarningText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible; } )
            .Justification( ETextJustify::Right )
        ]

        //---

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( HAlign_Right )
        .Padding( 10.f, 4.f, 10.f, 8.f )
        [
            export_panels
        ]
    ];
}

//---

FReply
SExportStoryboardSettings::OnKeyDown( const FGeometry& iMyGeometry, const FKeyEvent& iKeyEvent ) //override
{
    if( iKeyEvent.GetKey() == EKeys::Escape )
    {
        mParentWindow.Pin()->RequestDestroyWindow();

        return FReply::Handled();
    }

    return SCompoundWidget::OnKeyDown( iMyGeometry, iKeyEvent );
}

//---

int32
SExportStoryboardSettings::GetActiveTabIndex() const
{
    return int32( mActiveTab );
}

FText
SExportStoryboardSettings::GetFullPath() const
{
    if( mActiveTab == EExportTab::kImageSequence )
    {
        return mImageSequenceWidget->GetFullPath();
    }

    if( mActiveTab == EExportTab::kPDF )
    {
        return mPDFWidget->GetFullPath();
    }

    return FText::GetEmpty();
}

FText
SExportStoryboardSettings::GetErrorText() const
{
    if( mActiveTab == EExportTab::kImageSequence )
    {
        FText text = mImageSequenceWidget->GetErrorText();
        if( !text.IsEmpty() )
            return text;
    }

    if( mActiveTab == EExportTab::kPDF )
    {
        FText text = mPDFWidget->GetErrorText();
        if( !text.IsEmpty() )
            return text;
    }

    return FText::GetEmpty();
}

FText
SExportStoryboardSettings::GetWarningText() const
{
    if( mActiveTab == EExportTab::kImageSequence )
    {
        FText text = mImageSequenceWidget->GetWarningText();
        if( !text.IsEmpty() )
            return text;
    }

    if( mActiveTab == EExportTab::kPDF )
    {
        FText text = mPDFWidget->GetWarningText();
        if( !text.IsEmpty() )
            return text;
    }

    return FText::GetEmpty();
}

bool
SExportStoryboardSettings::CanExportStoryboard() const
{
    if( !GetErrorText().IsEmpty() )
        return false;

    if( mActiveTab == EExportTab::kImageSequence )
    {
        if( !mImageSequenceWidget->CanExportStoryboard() )
            return false;
    }

    if( mActiveTab == EExportTab::kPDF )
    {
        if( !mPDFWidget->CanExportStoryboard() )
            return false;
    }

    return true;
}

FReply
SExportStoryboardSettings::OnExportStoryboard()
{
    if( mActiveTab == EExportTab::kImageSequence )
    {
        mImageSequenceWidget->ExportStoryboard();
    }

    if( mActiveTab == EExportTab::kPDF )
    {
        mPDFWidget->ExportStoryboard();
    }

    //---

    mParentWindow.Pin()->RequestDestroyWindow();

    return FReply::Handled();
}

//---
//---
//---

void
ExportStoryboardDialog::OpenExportImageSequenceDialog( const TSharedRef<FTabManager>& TabManager, UMovieSceneSequence* iSequence )
{
    TWeakPtr<ISequencer> current_sequencer = nullptr;
    TArray<TWeakPtr<ISequencer>> sequencers = FLevelEditorSequencerIntegration::Get().GetSequencers();
    for( auto sequencer : sequencers )
    {
        if( sequencer.IsValid() && sequencer.Pin()->GetRootMovieSceneSequence() == iSequence )
        {
            current_sequencer = sequencer;
            break;
        }
    }

    if( !current_sequencer.IsValid() )
        return;

    FMovieSceneSequenceID focused_sequence_id = current_sequencer.Pin()->GetFocusedTemplateID();

    //---

    TSharedRef<SWindow> window = SNew( SWindow )
        .Title( LOCTEXT( "ExportStoryboardDialogTitle", "Export Storyboard Settings" ) )
        .HasCloseButton( true )
        .SupportsMaximize( false )
        .SupportsMinimize( false )
        .ClientSize( FVector2D( 1600, 1100 ) );

    window->SetContent( SNew( SExportStoryboardSettings )
                        .ParentWindow( window )
                        .DefaultTab( EExportTab::kImageSequence )
                        .ImageSequenceSettings()
                        [
                            SNew( SExportImageSequenceSettings, current_sequencer, focused_sequence_id )
                        ]
                        .PDFSettings()
                        [
                            SNew( SExportPDFSettings, current_sequencer, focused_sequence_id )
                        ]
                        );

    GEditor->EditorAddModalWindow( window );

    // To have a none modal dialog for debugging
    //TSharedPtr<SDockTab> OwnerTab = TabManager->GetOwnerTab();
    //TSharedPtr<SWindow> RootWindow = OwnerTab.IsValid() ? OwnerTab->GetParentWindow() : TSharedPtr<SWindow>();
    //if( RootWindow.IsValid() )
    //{
    //    FSlateApplication::Get().AddWindowAsNativeChild( window, RootWindow.ToSharedRef() );
    //}
    //else
    //{
    //    FSlateApplication::Get().AddWindow( window );
    //}
}

void
ExportStoryboardDialog::OpenExportPDFDialog( const TSharedRef<FTabManager>& TabManager, UMovieSceneSequence* iSequence )
{
    TWeakPtr<ISequencer> current_sequencer = nullptr;
    TArray<TWeakPtr<ISequencer>> sequencers = FLevelEditorSequencerIntegration::Get().GetSequencers();
    for( auto sequencer : sequencers )
    {
        if( sequencer.IsValid() && sequencer.Pin()->GetRootMovieSceneSequence() == iSequence )
        {
            current_sequencer = sequencer;
            break;
        }
    }

    if( !current_sequencer.IsValid() )
        return;

    FMovieSceneSequenceID focused_sequence_id = current_sequencer.Pin()->GetFocusedTemplateID();

    //IAssetEditorInstance* assetEditor = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset( iSequence, false );
    //IEposSequenceEditorToolkit* eposSequenceEditor = static_cast<IEposSequenceEditorToolkit*>( assetEditor );
    //TSharedPtr<ISequencer> sequencer = eposSequenceEditor ? eposSequenceEditor->GetSequencer() : nullptr;
    //check( sequencer.IsValid() );

    //sequencer->ResetToNewRootSequence( *iSequence );

    //---

    TSharedRef<SWindow> window = SNew( SWindow )
        .Title( LOCTEXT( "ExportStoryboardDialogTitle", "Export Storyboard Settings" ) )
        .HasCloseButton( true )
        .SupportsMaximize( false )
        .SupportsMinimize( false )
        .ClientSize( FVector2D( 1600, 1100 ) );

    window->SetContent( SNew( SExportStoryboardSettings )
                        .ParentWindow( window )
                        .DefaultTab( EExportTab::kImageSequence )
                        .ImageSequenceSettings()
                        [
                            SNew( SExportImageSequenceSettings, current_sequencer, focused_sequence_id )
                        ]
                        .PDFSettings()
                        [
                            SNew( SExportPDFSettings, current_sequencer, focused_sequence_id )
                        ]
                        );

    GEditor->EditorAddModalWindow( window );

    // To have a none modal dialog for debugging
    //TSharedPtr<SDockTab> OwnerTab = TabManager->GetOwnerTab();
    //TSharedPtr<SWindow> RootWindow = OwnerTab.IsValid() ? OwnerTab->GetParentWindow() : TSharedPtr<SWindow>();
    //if( RootWindow.IsValid() )
    //{
    //    FSlateApplication::Get().AddWindowAsNativeChild( window, RootWindow.ToSharedRef() );
    //}
    //else
    //{
    //    FSlateApplication::Get().AddWindow( window );
    //}
}

#undef LOCTEXT_NAMESPACE
