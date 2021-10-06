// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "NoteTrack/NoteSection.h"

#include "AssetRegistryModule.h"
#include "SequencerSectionPainter.h"
#include "EditorStyleSet.h"
#include "ISequencerSection.h"
#include "CommonMovieSceneTools.h"
#include "SequencerUtilities.h"

#include "NoteTrack/MovieSceneNoteTrack.h"
#include "NoteTrack/MovieSceneNoteSection.h"
#include "NoteTrack/SNoteSectionContent.h"
#include "StoryNote.h"

#define LOCTEXT_NAMESPACE "FNoteSection"

//---

FNoteSection::FNoteSection( UMovieSceneSection& InSection, TWeakPtr<ISequencer> InSequencer )
    : Section( InSection )
    , Sequencer( InSequencer )
{
}

FNoteSection::~FNoteSection()
{
}

TSharedRef<SWidget>
FNoteSection::GenerateSectionWidget()
{
    return SNew( SNoteSectionContent, SharedThis( this ) );
}

UMovieSceneSection*
FNoteSection::GetSectionObject()
{
    return &Section;
}

FText
FNoteSection::GetSectionTitle() const
{
    return FText::GetEmpty();
}

FText
FNoteSection::GetSectionToolTip() const
{
    UMovieSceneNoteSection* NoteSection = Cast<UMovieSceneNoteSection>( &Section );
    check( NoteSection );

    return NoteSection->GetNote() ? FText::FromString( NoteSection->GetNote()->Text ) : FText::GetEmpty();
}

float
FNoteSection::GetSectionHeight() const
{
    return Section.GetTypedOuter<UMovieSceneNoteTrack>()->GetRowHeight();
}

int32
FNoteSection::OnPaintSection( FSequencerSectionPainter& Painter ) const
{
    int32 LayerId = Painter.PaintSectionBackground();

    return LayerId;
}

//void FNoteSection::Tick( const FGeometry& AllottedGeometry, const FGeometry& ParentGeometry, const double InCurrentTime, const float InDeltaTime )
//{
//}

#undef LOCTEXT_NAMESPACE
