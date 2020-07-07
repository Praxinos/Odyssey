// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Layout/Margin.h"
#include "Sections/ThumbnailSection.h"
#include "TrackEditors/SubTrackEditorBase.h"

class FBoardTrackEditor;
class FMenuBuilder;
class FSequencerSectionPainter;
class FTrackEditorThumbnailPool;
class UMovieSceneCinematicBoardSection;

/**
 * Board section, which paints and ticks the appropriate section.
 */
class FBoardSection
    : public TSubSectionMixin<FViewportThumbnailSection>
{
public:

    /** Create and initialize a new instance. */
    FBoardSection( TSharedPtr<ISequencer> iSequencer, UMovieSceneCinematicBoardSection& iSection, TSharedPtr<FBoardTrackEditor> iBoardTrackEditor, TSharedPtr<FTrackEditorThumbnailPool> iThumbnailPool );

    /** Virtual destructor. */
    virtual ~FBoardSection();

public:

    // ISequencerSection interface

    virtual void    Tick( const FGeometry& iAllottedGeometry, const FGeometry& iClippedGeometry, const double iCurrentTime, const float iDeltaTime ) override;
    virtual int32   OnPaintSection( FSequencerSectionPainter& ioPainter ) const override;
    virtual void    BuildSectionContextMenu( FMenuBuilder& ioMenuBuilder, const FGuid& iObjectBinding ) override;
    virtual FText   GetSectionTitle() const override;
    virtual float   GetSectionHeight() const override;
    virtual FMargin GetContentPadding() const override;
    virtual bool    IsReadOnly() const override;

    // FThumbnail interface
    virtual void    SetSingleTime( double iGlobalTime ) override;
    virtual FText   HandleThumbnailTextBlockText() const override;
    virtual void    HandleThumbnailTextBlockTextCommitted( const FText& iNewThumbnailName, ETextCommit::Type iCommitType ) override;
    virtual UCameraComponent* GetViewCamera() override;

private:

    /** Add board takes menu */
    //void AddTakesMenu( FMenuBuilder& ioMenuBuilder );

private:

    /** The board track editor that contains this section */
    TWeakPtr<FBoardTrackEditor> mBoardTrackEditor;

    struct FCinematicSectionCache
    {
        FCinematicSectionCache( UMovieSceneCinematicBoardSection* iSection = nullptr );

        bool operator!=( const FCinematicSectionCache& iRHS ) const;

        FFrameRate   mInnerFrameRate;
        FFrameNumber mInnerFrameOffset;
        FFrameNumber mSectionStartFrame;
        float        mTimeScale;
    };

    /** Cached section thumbnail data */
    FCinematicSectionCache mThumbnailCacheData;
};
