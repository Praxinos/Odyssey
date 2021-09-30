// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "Templates/SubclassOf.h"
#include "ISequencer.h"
#include "MovieSceneTrack.h"
#include "ISequencerSection.h"
#include "ISequencerTrackEditor.h"
#include "PropertyTrackEditor.h"
#include "Tracks/MovieSceneStringTrack.h"
#include "Sections/MovieSceneStringSection.h"


/**
 * A property track editor for strings.
 */
class FPatchStringPropertyTrackEditor
    : public FPropertyTrackEditor<UMovieSceneStringTrack>
{
public:

    /**
     * Constructor.
     *
     * @param InSequencer The sequencer instance to be used by this tool.
     */
    FPatchStringPropertyTrackEditor( TSharedRef<ISequencer> InSequencer )
        : FPropertyTrackEditor( InSequencer, GetAnimatedPropertyTypes() )
    {
    }

    /**
     * Retrieve a list of all property types that this track editor animates
     */
    static TArray<FAnimatedPropertyKey, TInlineAllocator<1>> GetAnimatedPropertyTypes()
    {
        return TArray<FAnimatedPropertyKey, TInlineAllocator<1>>( { FAnimatedPropertyKey::FromPropertyTypeName( NAME_StrProperty ) } );
    }

    /**
     * Creates an instance of this class (called by a sequencer).
     *
     * @param OwningSequencer The sequencer instance to be used by this tool
     * @return The new instance of this class
     */
    static TSharedRef<ISequencerTrackEditor> CreateTrackEditor( TSharedRef<ISequencer> OwningSequencer );

protected:

    //~ FPropertyTrackEditor interface

    virtual void GenerateKeysFromPropertyChanged( const FPropertyChangedParams& PropertyChangedParams, UMovieSceneSection* SectionToKey, FGeneratedTrackKeys& OutGeneratedKeys ) override;
};

/**
* A property track editor for note.
*/
class FNoteTrackEditor
    : public FPatchStringPropertyTrackEditor
{
public:

    /**
     * Factory function to create an instance of this class (called by a sequencer).
     *
     * @param InSequencer The sequencer instance to be used by this tool.
     * @return The new instance of this class.
     */
    static TSharedRef<ISequencerTrackEditor> CreateTrackEditor(TSharedRef<ISequencer> InSequencer);

public:

    /**
     * Creates and initializes a new instance.
     *
     * @param InSequencer The sequencer instance to be used by this tool.
     */
    FNoteTrackEditor(TSharedRef<ISequencer> InSequencer);

public:

    // ISequencerTrackEditor interface

    virtual TSharedRef<ISequencerSection> MakeSectionInterface(UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding) override;
    virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) override;
    virtual bool SupportsSequence(UMovieSceneSequence* InSequence) const override;
    virtual bool SupportsType(TSubclassOf<UMovieSceneTrack> Type) const override;
    virtual const FSlateBrush* GetIconBrush() const override;

private:

    /** Callback for executing the "Add Note Track" menu entry. */
    void HandleAddNoteTrackMenuEntryExecute();
    bool HandleAddNoteTrackMenuEntryCanExecute() const;
};
