// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Layout/Margin.h"
#include "Sections/ThumbnailSection.h"
#include "TrackEditors/SubTrackEditorBase.h"

#include "SingleCameraCutTrack/KeyThumbnailSection.h"

class FCinematicBoardTrackEditor;
class FMenuBuilder;
class FSequencerSectionPainter;
class FTrackEditorThumbnailPool;
class UMovieSceneCinematicBoardSection;

/**
 * Board section, which paints and ticks the appropriate section.
 */
class FCinematicBoardSection
    : public TSubSectionMixin<FKeyThumbnailSection>
{
public:

    /** Create and initialize a new instance. */
    FCinematicBoardSection( TSharedPtr<ISequencer> iSequencer, UMovieSceneCinematicBoardSection& iSection, TSharedPtr<FCinematicBoardTrackEditor> iCinematicBoardTrackEditor, TSharedPtr<FTrackEditorThumbnailPool> iThumbnailPool );

    /** Virtual destructor. */
    virtual ~FCinematicBoardSection();

public:

    // ISequencerSection interface
    virtual void    Tick( const FGeometry& iAllottedGeometry, const FGeometry& iClippedGeometry, const double iCurrentTime, const float iDeltaTime ) override;
    virtual int32   OnPaintSection( FSequencerSectionPainter& ioPainter ) const override;
    virtual void    BuildSectionContextMenu( FMenuBuilder& ioMenuBuilder, const FGuid& iObjectBinding ) override;
    virtual FText   GetSectionTitle() const override;
    virtual float   GetSectionHeight() const override;
    virtual FMargin GetContentPadding() const override;
    virtual bool    IsReadOnly() const override;

    virtual void    BeginResizeSection();
    virtual void    ResizeSection( ESequencerSectionResizeMode ResizeMode, FFrameNumber ResizeFrameNumber );

    virtual void    BeginSlipSection();
    virtual void    SlipSection( FFrameNumber SlipTime );

    virtual void    BeginDilateSection();
    virtual void    DilateSection( const TRange<FFrameNumber>& NewRange, float DilationFactor );

    // FThumbnailSection interface
    virtual void    SetSingleTime( double iGlobalTime ) override;
    virtual FText   HandleThumbnailTextBlockText() const override;
    virtual void    HandleThumbnailTextBlockTextCommitted( const FText& iNewThumbnailName, ETextCommit::Type iCommitType ) override;

    // IViewportThumbnailClient interface
    virtual UCameraComponent* GetViewCamera() override;

    // FKeyThumbnailSection interface
    virtual TArray<double> GetKeys() const override;

private:

    /** Add board takes menu */
    //void AddTakesMenu( FMenuBuilder& ioMenuBuilder );

private:

    /** The board track editor that contains this section */
    TWeakPtr<FCinematicBoardTrackEditor> mCinematicBoardTrackEditor;

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
