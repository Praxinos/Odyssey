// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "NoteTrack/SNoteSectionContent.h"

#include "Brushes/SlateColorBrush.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#include "NoteTrack/NoteSection.h"
#include "NoteTrack/MovieSceneNoteSection.h"
#include "StoryNote.h"
#include "Styles/EposTracksEditorStyle.h"

#define LOCTEXT_NAMESPACE "SNoteSectionContent"

//---

void
SNoteSectionContent::Construct( const FArguments& InArgs, TSharedRef<FNoteSection> iNoteSection )
{
    mNoteSection = iNoteSection;

    //---

    FSlimHorizontalToolBarBuilder LeftToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    LeftToolbarBuilder.SetStyle( &FEposTracksEditorStyle::Get(), "SectionTitleToolBar" );

    auto EditNote = [this]()
    {
        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();

        UMovieSceneNoteSection* note_section_object = mNoteSection.IsValid() ? Cast<UMovieSceneNoteSection>( mNoteSection.Pin()->GetSectionObject() ) : nullptr;
        UStoryNote* note = note_section_object ? note_section_object->GetNote() : nullptr;

        if( note )
            AssetEditorSubsystem->OpenEditorForAsset( note );
    };

    auto CanEditNote = [this]() -> bool
    {
        UMovieSceneNoteSection* note_section_object = mNoteSection.IsValid() ? Cast<UMovieSceneNoteSection>( mNoteSection.Pin()->GetSectionObject() ) : nullptr;
        UStoryNote* note = note_section_object ? note_section_object->GetNote() : nullptr;

        return !!note;
    };

    LeftToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateLambda( EditNote ),
            FCanExecuteAction::CreateLambda( CanEditNote )
        ),
        NAME_None,
        FText::GetEmpty(),
        LOCTEXT( "edit-note-tooltip", "Edit the note in its editor" ),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "NoteSettings" )
    );

    TSharedRef< SWidget > left_toolbar = LeftToolbarBuilder.MakeWidget();
    left_toolbar->SetVisibility( MakeAttributeSP( this, &SNoteSectionContent::OptionalWidgetsVisibility ) );

    //---

    auto GetFont = [this]() -> FSlateFontInfo
    {
        UMovieSceneNoteSection* note_section_object = mNoteSection.IsValid() ? Cast<UMovieSceneNoteSection>( mNoteSection.Pin()->GetSectionObject() ) : nullptr;
        UStoryNote* note = note_section_object ? note_section_object->GetNote() : nullptr;

        return note ? note->Font : FSlateFontInfo();
    };

    auto GetColor = [this]() -> FLinearColor
    {
        UMovieSceneNoteSection* note_section_object = mNoteSection.IsValid() ? Cast<UMovieSceneNoteSection>( mNoteSection.Pin()->GetSectionObject() ) : nullptr;
        UStoryNote* note = note_section_object ? note_section_object->GetNote() : nullptr;

        return note ? note->ColorAndOpacity : FLinearColor::White;
    };

    auto GetShadowColor = [this]() -> FLinearColor
    {
        UMovieSceneNoteSection* note_section_object = mNoteSection.IsValid() ? Cast<UMovieSceneNoteSection>( mNoteSection.Pin()->GetSectionObject() ) : nullptr;
        UStoryNote* note = note_section_object ? note_section_object->GetNote() : nullptr;

        return note ? note->ShadowColorAndOpacity : FLinearColor::Black;
    };

    auto GetShadowOffset = [this]() -> FVector2D
    {
        UMovieSceneNoteSection* note_section_object = mNoteSection.IsValid() ? Cast<UMovieSceneNoteSection>( mNoteSection.Pin()->GetSectionObject() ) : nullptr;
        UStoryNote* note = note_section_object ? note_section_object->GetNote() : nullptr;

        return note ? note->ShadowOffset : FVector2D::UnitVector;
    };

    //---

    ChildSlot
    // Because in ...\Sequencer\Private\DisplayNodes\SequencerTrackNode.cpp - FSequencerTrackNode::GetNodeHeight(), there is an additional padding 2 * SequencerNodeConstants::CommonPadding for the 'mother' section
    // and VAlign can't be used, because f.e., one shot section with planes and one board section without, the content of the board section won't be at the same level as in shot section
    //.Padding( 0.f, 4.f )
    [
        SNew( SHorizontalBox )

        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign( VAlign_Center )
        [
            left_toolbar
        ]

        + SHorizontalBox::Slot()
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .Padding( 5 )
            [
                SAssignNew( mNoteTextWidget, SInlineEditableTextBlock )
                .Text( this, &SNoteSectionContent::GetNoteText )
                .MultiLine( true )
                .ModiferKeyForNewLine( EModifierKey::Shift )
                .Font_Lambda( GetFont )
                .ColorAndOpacity_Lambda( GetColor )
                .ShadowColorAndOpacity_Lambda( GetShadowColor )
                .ShadowOffset_Lambda( GetShadowOffset )
                .OnTextCommitted( this, &SNoteSectionContent::OnNoteTextCommited )
            ]
        ]
    ];
}

FText
SNoteSectionContent::GetNoteText() const
{
    if( !mNoteSection.IsValid() )
        return FText::GetEmpty();

    FNoteSection* note_section = mNoteSection.Pin().Get();
    UMovieSceneNoteSection* note_section_object = note_section ? Cast<UMovieSceneNoteSection>( note_section->GetSectionObject() ) : nullptr;
    UStoryNote* note = note_section_object->GetNote();

    return note ? FText::FromString( note->Text ) : FText::GetEmpty();
}

void
SNoteSectionContent::OnNoteTextCommited( const FText& iText, ETextCommit::Type CommitType )
{
    FNoteSection* note_section = mNoteSection.Pin().Get();
    ISequencer* sequencer = note_section->GetSequencer().Get();
    UMovieSceneNoteSection* note_section_object = note_section ? Cast<UMovieSceneNoteSection>( note_section->GetSectionObject() ) : nullptr;
    UStoryNote* note = note_section_object ? note_section_object->GetNote() : nullptr;
    if( !note )
        return;

    if( CommitType != ETextCommit::OnEnter || iText.ToString() == note->Text )
        return;

    //---

    const FScopedTransaction Transaction( LOCTEXT( "SetNewNoteText", "Set Note Text" ) );

    note->Modify();
    note->Text = iText.ToString();

    sequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

FReply
SNoteSectionContent::OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) //override
{
    if( InMouseEvent.IsControlDown() )
    {
        FNoteSection* note_section = mNoteSection.Pin().Get();
        UMovieSceneNoteSection* note_section_object = note_section ? Cast<UMovieSceneNoteSection>( note_section->GetSectionObject() ) : nullptr;
        UStoryNote* note = note_section_object ? note_section_object->GetNote() : nullptr;
        if( !note )
            return FReply::Unhandled();

        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
        AssetEditorSubsystem->OpenEditorForAsset( note );

        return FReply::Handled();
    }

    mNoteTextWidget->EnterEditingMode();

    return FReply::Handled();
}

int32
SNoteSectionContent::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

EVisibility
SNoteSectionContent::OptionalWidgetsVisibility() const
{
    return mOptionalWidgetsVisibility;
}

void
SNoteSectionContent::OnMouseEnter( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    mOptionalWidgetsVisibility = EVisibility::Visible;
}

void
SNoteSectionContent::OnMouseLeave( const FPointerEvent& MouseEvent ) //override
{
    mOptionalWidgetsVisibility = EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE
