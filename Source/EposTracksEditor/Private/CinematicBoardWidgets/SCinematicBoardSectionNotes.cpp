// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionNotes.h"

#include "Brushes/SlateColorBrush.h"
#include "SequencerSettings.h"

#include "EposSequenceHelpers.h"
#include "EposTracksToolbarHelpers.h"
#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "NoteTrack/MovieSceneNoteSection.h"
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
        SLATE_ARGUMENT( TWeakObjectPtr<UStoryNote>, Note )
        SLATE_ATTRIBUTE( EVisibility, OptionalWidgetsVisibility )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

    virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) override;

    virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

private:
    void CacheLines();

    UMovieSceneNoteSection* GetSection(); //TODO: should be removed, see comment inside

    void BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder );

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;
    TWeakObjectPtr<UStoryNote>          mNote;
    TAttribute<EVisibility>             mOptionalWidgetsVisibility;

    //TODO: maybe improve this by doing/storing it directly inside the note object ?
    FString         mCachedText;
    TArray<FString> mCachedLines;
};

void
SCinematicBoardSectionNote::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mNote = InArgs._Note;
    mOptionalWidgetsVisibility = InArgs._OptionalWidgetsVisibility;

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
        return FText::FromString( mNote->Text );
    };

    ChildSlot
    .Padding( FMargin( 5, 3 ) )
    [
        SNew( SHorizontalBox )

        +SHorizontalBox::Slot()
        .VAlign( VAlign_Center )
        [
            SNew( STextBlock )
            .Text_Lambda( GetFirstLine )
            .ToolTipText_Lambda( GetTooltip )
            .ShadowOffset( FVector2D( 1, 1 ) )
        ]
    ];
}

void
SCinematicBoardSectionNote::CacheLines()
{
    if( mCachedText == mNote->Text )
        return;

    mCachedText = mNote->Text;
    mCachedText.ParseIntoArrayLines( mCachedLines );
}

UMovieSceneNoteSection*
SCinematicBoardSectionNote::GetSection()
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    //TODO: make it correct
    // This is not the correct way to get section, as a note may be used inside multiple note sections
    // And this way remove the first found one
    // The correct way would be to store the section in the same time as the note, and this implies to have listview row of a complex type like "FNoteAndSection" or just a listview of UMovieSceneNoteSection ? (or even FGuid for attached note ?), let's talk about this

    UMovieSceneNoteSection* section_of_note = nullptr;

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );
    if( result.mInnerMovieScene )
    {
        for( auto track : result.mInnerMovieScene->GetMasterTracks() )
        {
            for( auto section : track->GetAllSections() )
            {
                UMovieSceneNoteSection* note_section = Cast<UMovieSceneNoteSection>( section );
                if( note_section && note_section->GetNote() == mNote )
                    section_of_note = Cast<UMovieSceneNoteSection>( note_section );
            }
        }
    }

    return section_of_note;
}

void
SCinematicBoardSectionNote::BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder )
{
    auto EditNote = [=]()
    {
        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
        check( !AssetEditorSubsystem->FindEditorsForAsset( mNote.Get() ).Num() );

        AssetEditorSubsystem->OpenEditorForAsset( mNote.Get() );
    };

    auto CanEditNote = [=]() -> bool
    {
        return !!mNote.Get();
    };

    //-

    auto DeleteNote = [=]()
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        BoardSequenceTools::DeleteNote( sequencer, *subsection_object, GetSection() );
    };

    //-

    CacheLines();
    FString first_line = mCachedLines.Num() ? mCachedLines[0] : FString();
    FString start_first_line = first_line.Left( 20 );
    if( mCachedLines.Num() > 1 || start_first_line.Len() != first_line.Len() )
        start_first_line += TEXT( "..." );
    FText note_name = FText::FromString( start_first_line );

    ioMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "note-section-label", "Note: {0}" ), note_name ) );

    ioMenuBuilder.AddMenuEntry( LOCTEXT( "edit-note-label", "Edit" ),
                                LOCTEXT( "edit-note-tooltip", "Edit the note in its editor" ),
                                FSlateIcon(),
                                FUIAction( FExecuteAction::CreateLambda( EditNote ),
                                           FCanExecuteAction::CreateLambda( CanEditNote ) ) );

    ioMenuBuilder.AddMenuEntry( LOCTEXT( "delete-note-label", "Delete" ),
                                LOCTEXT( "delete-note-tooltip", "Delete the note" ),
                                FSlateIcon( FCoreStyle::Get().GetStyleSetName(), "GenericCommands.Delete" ),
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
        UStoryNote* note = mNote.Get();
        if( !note )
            return FReply::Unhandled();

        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
        check( !AssetEditorSubsystem->FindEditorsForAsset( note ).Num() );

        AssetEditorSubsystem->OpenEditorForAsset( note );

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

    FToolBarBuilder MiddleToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    MiddleToolbarBuilder.SetLabelVisibility( EVisibility::Collapsed );
    MiddleToolbarBuilder.SetStyle( &*FEposTracksEditorStyle::Get(), "EposSection.ToolBar" );

    MiddleToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateLambda( CreateNote ),
            FCanExecuteAction::CreateLambda( CanCreateNote ),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( CanCreateNote ) ),
        NAME_None,
        FText::GetEmpty(),
        LOCTEXT( "CreateNote", "Create a new Note" ),
        FSlateIcon( FEditorStyle::GetStyleSetName(), "Plus" ) );
        //FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.CreateNote" ) );

    TSharedRef< SWidget > middle_widget = MiddleToolbarBuilder.MakeWidget();
    // To always keep the real space of the toolbar as hidden keeps space
    // Otherwise the verticalbox is (a little) smaller when the toolbar is collapsed
    middle_widget->SetVisibility( MakeAttributeLambda( [this]() { return mOptionalWidgetsVisibility.Get() == EVisibility::Visible ? EVisibility::Visible : EVisibility::Hidden; } ) );

    //---

    static const FSlateColorBrush background = FSlateColorBrush( FSequencerSectionPainter::BlendColor( FColor( 90, 90, 150 ) ) ); // Same as the UMovieSceneNoteTrack

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
        .BorderImage( &background )
        .Visibility_Lambda( IsNotesVisible )
        [
            SNew( SVerticalBox )
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SAssignNew( mWidgetNoteList, SListView<TWeakObjectPtr<UStoryNote>> )
                .ListItemsSource( &mNotes )
                .OnGenerateRow( this, &SCinematicBoardSectionNotes::MakeNoteRow )
                .SelectionMode( ESelectionMode::None )
            ]
            + SVerticalBox::Slot()
            .HAlign( HAlign_Center )
            .AutoHeight()
            [
                middle_widget
            ]
        ]
    ];

    RebuildNoteList();
}

//-

class STableRowNote
    : public STableRow<TWeakObjectPtr<UStoryNote>>
{
public:
    // Construct the widget
    virtual void ConstructChildren( ETableViewMode::Type InOwnerTableMode, const TAttribute<FMargin>& InPadding, const TSharedRef<SWidget>& InContent ) override;

public:
    virtual FReply OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) override;

private:
    FTableRowStyle mTableRowStyle;
};

void
STableRowNote::ConstructChildren( ETableViewMode::Type InOwnerTableMode, const TAttribute<FMargin>& InPadding, const TSharedRef<SWidget>& InContent ) //override
{
    STableRow<TWeakObjectPtr<UStoryNote>>::ConstructChildren( InOwnerTableMode, InPadding, InContent );

    //---

    FLinearColor original_color( FSequencerSectionPainter::BlendColor( FColor( 90, 90, 150 ) ) ); // Same as the UMovieSceneNoteTrack

    mTableRowStyle = FEposTracksEditorStyle::Get()->GetWidgetStyle<FTableRowStyle>( "EposNotes.TableView.Row" );
    mTableRowStyle.SetEvenRowBackgroundBrush( FSlateColorBrush( original_color ) );
    mTableRowStyle.SetOddRowBackgroundBrush( FSlateColorBrush( original_color * 1.33 ) ); // a little brighter

    Style = &mTableRowStyle;
}

FReply
STableRowNote::OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) //override
{
    return FReply::Unhandled();
}

//-

TSharedRef<ITableRow>
SCinematicBoardSectionNotes::MakeNoteRow( TWeakObjectPtr<UStoryNote> iItem, const TSharedRef<STableViewBase>& iOwnerTable )
{
    if( !mBoardSection.IsValid() )
        return SNew( STableRowNote, iOwnerTable );

    return
        SNew( STableRowNote, iOwnerTable )
        [
            SNew( SCinematicBoardSectionNote, mBoardSection.Pin().ToSharedRef() )
            .Note( iItem )
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

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *mSequencer.Pin().Get(), subsection, mSequencer.Pin()->GetFocusedTemplateID() );
    check( inner_sequence == result.mInnerSequence ); // Just to test

    // Get all unordered planes
    TArray<TWeakObjectPtr<UStoryNote>> notes;
    int note_count = ShotSequenceHelpers::GetAllNotes( *mSequencer.Pin().Get(), result.mInnerSequence, result.mInnerSequenceId, &notes, nullptr );

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

    for( auto note : notes )
    {
        mNotes.Add( note );
        //mNotes.Add( MakeShareable<UStoryNote>( note.Get() ) );
        //mNotes.Add( MakeShared<UStoryNote>( note.Get() ) );
    }

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
