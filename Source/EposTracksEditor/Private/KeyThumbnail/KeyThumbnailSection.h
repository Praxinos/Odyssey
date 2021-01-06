// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Layout/Margin.h"
#include "Sections/ThumbnailSection.h"

#include "KeyThumbnail/TrackEditorKeyThumbnail.h"

class AActor;
class FMenuBuilder;
class FSequencerSectionPainter;
class FTrackEditorThumbnailPool;

/**
 * KeyThumbnail section, which paints and ticks the appropriate section.
   This class will duplicate the whole 3 functions (Tick/BuildSectionContextMenu/OnPaintSection) to be able to use its own KeyThumbnailCache
 */
class FKeyThumbnailSection
    : public FViewportThumbnailSection
{
public:

    /** Create and initialize a new instance. */
    FKeyThumbnailSection( TSharedPtr<ISequencer> InSequencer, TSharedPtr<FTrackEditorThumbnailPool> InThumbnailPool, UMovieSceneSection& InSection );

    /** Virtual destructor. */
    virtual ~FKeyThumbnailSection();

protected:

    /** Called to force a redraw of this section's thumbnails */
    void RedrawThumbnails();

public:

    // ISequencerSection interface
    virtual void Tick( const FGeometry& AllottedGeometry, const FGeometry& ClippedGeometry, const double InCurrentTime, const float InDeltaTime ) override;
    virtual void BuildSectionContextMenu( FMenuBuilder& MenuBuilder, const FGuid& ObjectBinding ) override;
    virtual int32 OnPaintSection( FSequencerSectionPainter& InPainter ) const override;

    virtual TArray<double> GetKeys() const = 0;

protected:

    /** A list of all key thumbnails this section has.
        This list replace the original one inside FThumbnailSection
    */
    FTrackEditorKeyThumbnailCache KeyThumbnailCache;
};
