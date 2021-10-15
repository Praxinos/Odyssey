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
    : mSection( InSection )
    , mSequencer( InSequencer )
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
    return &mSection;
}

FText
FNoteSection::GetSectionTitle() const
{
    return FText::GetEmpty();
}

FText
FNoteSection::GetSectionToolTip() const
{
    UMovieSceneNoteSection* NoteSection = Cast<UMovieSceneNoteSection>( &mSection );
    check( NoteSection );

    return NoteSection->GetNote() ? FText::FromString( NoteSection->GetNote()->Text ) : FText::GetEmpty();
}

float
FNoteSection::GetSectionHeight() const
{
    return mSection.GetTypedOuter<UMovieSceneNoteTrack>()->GetRowHeight();
}

int32
FNoteSection::OnPaintSection( FSequencerSectionPainter& iPainter ) const
{
    int32 LayerId = iPainter.PaintSectionBackground();

    return LayerId;
}

//void FNoteSection::Tick( const FGeometry& AllottedGeometry, const FGeometry& ParentGeometry, const double InCurrentTime, const float InDeltaTime )
//{
//}

TSharedPtr<ISequencer>
FNoteSection::GetSequencer() const
{
    return mSequencer.Pin();
}

#undef LOCTEXT_NAMESPACE
