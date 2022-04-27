// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "CinematicBoardWidgets/SCinematicBoardSectionNotes.h"

#include "Brushes/SlateColorBrush.h"
#include "SequencerSettings.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "NoteTrack/MovieSceneNoteSection.h"
#include "NoteTrack/MovieSceneNoteTrack.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Shot/ShotSequence.h"
#include "StoryNote.h"
#include "Styles/EposTracksEditorStyle.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionNotes"

//---

class EPOSTRACKSEDITOR_API SCinematicBoardSectionNote
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionNote )
        {}
        SLATE_ARGUMENT( TWeakObjectPtr<UMovieSceneNoteSection>, NoteSection )
        SLATE_ATTRIBUTE( EVisibility, OptionalWidgetsVisibility )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

    virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) override;

    virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

private:
    void EditNote();
    bool CanEditNote();

    void CacheLines();

    void BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder );

private:
    TWeakPtr<FCinematicBoardSection>        mBoardSection;
    TWeakObjectPtr<UMovieSceneNoteSection>  mNoteSection;
    TAttribute<EVisibility>                 mOptionalWidgetsVisibility;

    //TODO: maybe improve this by doing/storing it directly inside the note object ?
    FString         mCachedText;
    TArray<FString> mCachedLines;
};

void
SCinematicBoardSectionNote::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mNoteSection = InArgs._NoteSection;
    mOptionalWidgetsVisibility = InArgs._OptionalWidgetsVisibility;

    //---

    FSlimHorizontalToolBarBuilder LeftToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    LeftToolbarBuilder.SetStyle( &FEposTracksEditorStyle::Get(), "SectionTitleToolBar" );

    LeftToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateSP( this, &SCinematicBoardSectionNote::EditNote ),
            FCanExecuteAction::CreateSP( this, &SCinematicBoardSectionNote::CanEditNote )
        ),
        NAME_None,
        FText::GetEmpty(),
        LOCTEXT( "edit-note-tooltip", "Edit the note in its editor" ),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "NoteSettings" )
    );

    TSharedRef< SWidget > left_toolbar = LeftToolbarBuilder.MakeWidget();
    left_toolbar->SetVisibility( MakeAttributeLambda( [this]() { return mOptionalWidgetsVisibility.Get().IsVisible() ? EVisibility::Visible : EVisibility::Collapsed; } ) );

    //---

    auto GetFirstLine = [=]() -> FText
    {
        CacheLines();

        if( !mCachedLines.Num() )
            return FText::GetEmpty();

        if( mCachedLines.Num() == 1 )
            return FText::FromString( mCachedLines[0] );

        return FText::FromString( mCachedLines[0] + TEXT("...") );
    };

    auto GetTooltip = [=]() -> FText
    {
        if( !mNoteSection.IsValid() || !mNoteSection->GetNote() )
            return FText::GetEmpty();

        return FText::FromString( mNoteSection->GetNote()->Text );
    };

    //---

    auto GetFont = [this]() -> FSlateFontInfo
    {
        UStoryNote* note = mNoteSection.IsValid() ? mNoteSection->GetNote() : nullptr;

        return note ? note->Font : FSlateFontInfo();
    };

    auto GetColor = [this]() -> FLinearColor
    {
        UStoryNote* note = mNoteSection.IsValid() ? mNoteSection->GetNote() : nullptr;

        return note ? note->ColorAndOpacity : FLinearColor::White;
    };

    auto GetShadowColor = [this]() -> FLinearColor
    {
        UStoryNote* note = mNoteSection.IsValid() ? mNoteSection->GetNote() : nullptr;

        return note ? note->ShadowColorAndOpacity : FLinearColor::Black;
    };

    auto GetShadowOffset = [this]() -> FVector2D
    {
        UStoryNote* note = mNoteSection.IsValid() ? mNoteSection->GetNote() : nullptr;

        return note ? note->ShadowOffset : FVector2D::UnitVector;
    };

    //---

    ChildSlot
    .Padding( FMargin( 2, 3 ) )
    [
        SNew( SHorizontalBox )

        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign( VAlign_Center )
        [
            left_toolbar
        ]

        +SHorizontalBox::Slot()
        .VAlign( VAlign_Center )
        [
            SNew( STextBlock )
            .Text_Lambda( GetFirstLine )
            .ToolTipText_Lambda( GetTooltip )
            .Font_Lambda( GetFont )
            .ColorAndOpacity_Lambda( GetColor )
            .ShadowColorAndOpacity_Lambda( GetShadowColor )
            .ShadowOffset_Lambda( GetShadowOffset )
        ]
    ];
}

void
SCinematicBoardSectionNote::EditNote()
{
    UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();

    AssetEditorSubsystem->OpenEditorForAsset( mNoteSection->GetNote() );
}

bool
SCinematicBoardSectionNote::CanEditNote()
{
    return mNoteSection.IsValid() && mNoteSection->GetNote();
}

void
SCinematicBoardSectionNote::CacheLines()
{
    if( !mNoteSection.IsValid() || !mNoteSection->GetNote() )
        return;

    if( mCachedText == mNoteSection->GetNote()->Text )
        return;

    mCachedText = mNoteSection->GetNote()->Text;
    mCachedText.ParseIntoArrayLines( mCachedLines );
}

void
SCinematicBoardSectionNote::BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder )
{
    auto DeleteNote = [=]()
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        BoardSequenceTools::DeleteNote( sequencer, *subsection_object, mNoteSection );
    };

    //-

    CacheLines();
    FString first_line = mCachedLines.Num() ? mCachedLines[0] : FString();
    FString start_first_line = first_line.Left( 20 );
    if( mCachedLines.Num() > 1 || start_first_line.Len() != first_line.Len() )
        start_first_line += TEXT( "..." );
    FText note_name = FText::FromString( start_first_line );

    ioMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "note-section-label", "Note: {0}" ), note_name ) );

    ioMenuBuilder.AddMenuEntry( LOCTEXT( "edit-note-label", "Edit..." ),
                                LOCTEXT( "edit-note-tooltip", "Edit the note in its editor" ),
                                FSlateIcon(),
                                FUIAction( FExecuteAction::CreateSP( this, &SCinematicBoardSectionNote::EditNote ),
                                           FCanExecuteAction::CreateSP( this, &SCinematicBoardSectionNote::CanEditNote ) ) );

    ioMenuBuilder.AddMenuEntry( LOCTEXT( "delete-note-label", "Delete" ),
                                LOCTEXT( "delete-note-tooltip", "Delete the note" ),
                                FSlateIcon( FAppStyle::Get().GetStyleSetName(), "GenericCommands.Delete" ),
                                FUIAction( FExecuteAction::CreateLambda( DeleteNote ) ) );

    ioMenuBuilder.EndSection();
}

//---

FCursorReply
SCinematicBoardSectionNote::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    return FCursorReply::Cursor( EMouseCursor::Default );
}

FReply
SCinematicBoardSectionNote::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SCompoundWidget::OnMouseButtonDown( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionNote::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    if( MouseEvent.GetEffectingButton() == EKeys::RightMouseButton )
    {
        FMenuBuilder menu_builder( true, nullptr );
        BuildKeyContextMenu( menu_builder );

        TSharedPtr<SWidget> menu = menu_builder.MakeWidget();
        FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
        FSlateApplication::Get().PushMenu( AsShared(), WidgetPath, menu.ToSharedRef(), MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect( FPopupTransitionEffect::ContextMenu ) );

        return FReply::Handled();
    }

    return SCompoundWidget::OnMouseButtonUp( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionNote::OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) //override
{
    if( InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton /*&& InMouseEvent.IsControlDown()*/ )
    {
        if( !mNoteSection.IsValid() || !mNoteSection->GetNote() )
            return FReply::Unhandled();

        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();

        AssetEditorSubsystem->OpenEditorForAsset( mNoteSection->GetNote() );

        return FReply::Handled();
    }

    return SCompoundWidget::OnMouseButtonDoubleClick( InMyGeometry, InMouseEvent );
}

//---
//---
//---

SCinematicBoardSectionNotes::~SCinematicBoardSectionNotes()
{
    if( mSequencer.IsValid() )
        mSequencer.Pin()->OnMovieSceneDataChanged().Remove( mRebuildNoteListHandle );
}

void
SCinematicBoardSectionNotes::RebuildNoteList( EMovieSceneDataChangeType iType )
{
    mNeedRebuildNoteList = true;
}

void
SCinematicBoardSectionNotes::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;
    mSequencer = mBoardSection.Pin()->GetSequencer();

    mOptionalWidgetsVisibility = InArgs._OptionalWidgetsVisibility;

    mRebuildNoteListHandle = mSequencer.Pin()->OnMovieSceneDataChanged().AddSP( this, &SCinematicBoardSectionNotes::RebuildNoteList );

    check( !mNotes.Num() );

    //---

    auto CreateNote = [this]()
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = mSequencer.Pin().Get();
        BoardSequenceTools::CreateNote( sequencer, *subsection_object, sequencer->GetLocalTime().Time.FrameNumber );
    };

    auto CanCreateNote = [this]() -> bool
    {
        return true;
    };

    FSlimHorizontalToolBarBuilder MiddleToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    MiddleToolbarBuilder.SetStyle( &FEposTracksEditorStyle::Get(), "SectionFloatingToolBar" );

    MiddleToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateLambda( CreateNote ),
            FCanExecuteAction::CreateLambda( CanCreateNote ) ),
        NAME_None,
        FText::GetEmpty(),
        LOCTEXT( "CreateNote", "Create a new Note" ),
        FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Plus" ) );

    TSharedRef< SWidget > middle_toolbar = MiddleToolbarBuilder.MakeWidget();
    middle_toolbar->SetVisibility( mOptionalWidgetsVisibility );

    //---

    // It's needed for notes (unlike for planes) as there is a SBorder widget as parent with a colored background
    // It's not wished for board section as everything available for board should be done directly on it)
    auto IsNotesVisible = [=]() -> EVisibility
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();

        return Cast<UShotSequence>( subsection_object->GetSequence() ) ? EVisibility::Visible : EVisibility::Hidden;
    };

    ChildSlot
    [
        SNew( SBorder )
        .BorderImage( &FEposTracksEditorStyle::Get().GetWidgetStyle<FTableRowStyle>( "Notes.TableView.Row" ).EvenRowBackgroundBrush )
        .Visibility_Lambda( IsNotesVisible )
        [
            SNew( SVerticalBox )
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SAssignNew( mWidgetNoteList, SListView<TWeakObjectPtr<UMovieSceneNoteSection>> )
                .ListItemsSource( &mNotes )
                .OnGenerateRow( this, &SCinematicBoardSectionNotes::MakeNoteRow )
                .SelectionMode( ESelectionMode::None )
            ]
            + SVerticalBox::Slot()
            .HAlign( HAlign_Center )
            .AutoHeight()
            [
                middle_toolbar
            ]
        ]
    ];

    RebuildNoteList();
}

//-

class STableRowNote
    : public STableRow<TWeakObjectPtr<UMovieSceneNoteSection>>
{
public:
    virtual FReply OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) override;
};

FReply
STableRowNote::OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) //override
{
    return FReply::Unhandled();
}

//-

TSharedRef<ITableRow>
SCinematicBoardSectionNotes::MakeNoteRow( TWeakObjectPtr<UMovieSceneNoteSection> iItem, const TSharedRef<STableViewBase>& iOwnerTable )
{
    if( !mBoardSection.IsValid() )
        return SNew( STableRowNote, iOwnerTable );

    return
        SNew( STableRowNote, iOwnerTable )
        .Style( FEposTracksEditorStyle::Get(), "Notes.TableView.Row" )
        [
            SNew( SCinematicBoardSectionNote, mBoardSection.Pin().ToSharedRef() )
            .NoteSection( iItem )
            .OptionalWidgetsVisibility( mOptionalWidgetsVisibility )
        ];
}

void
SCinematicBoardSectionNotes::RebuildNoteList()
{
    if( !mBoardSection.IsValid() )
        return;

    //---

    if( !mNeedRebuildNoteList )
        return;

    mNeedRebuildNoteList = false;

    //---

    UMovieSceneSubSection& subsection = mBoardSection.Pin()->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;
    if( !inner_moviescene )
    {
        mNotes.Empty();

        if( mWidgetNoteList )
            mWidgetNoteList->RequestListRefresh();

        return;
    }

    //---

    auto note_sections = BoardSequenceTools::GetAllNotes( mSequencer.Pin().Get(), subsection );

    //---

    //auto need_rebuild = [this]( const TArray<FGuid>& iBindings )
    //{
    //    if( iBindings.Num() != mNotes.Num() )
    //        return true;

    //    if( !iBindings.Num() ) // Rebuild when no possessables, otherwise list view will be empty and not containing max_planes rows (with invalid guid)
    //        return true;

    //    for( int i = 0; i < iBindings.Num(); i++ )
    //        if( iBindings[i] != mNotes[i]->GetGuid() )
    //            return true;

    //    return false;
    //};
    //if( !need_rebuild( ordered_plane_bindings ) ) //TOCHECK: check if it's really ok
    //    return;

    mNotes.Empty();

    mNotes = note_sections;

    if( mWidgetNoteList )
        mWidgetNoteList->RequestListRefresh();
}

void
SCinematicBoardSectionNotes::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) //override
{
    SCompoundWidget::Tick( AllottedGeometry, InCurrentTime, InDeltaTime );

    RebuildNoteList();
}

#undef LOCTEXT_NAMESPACE
