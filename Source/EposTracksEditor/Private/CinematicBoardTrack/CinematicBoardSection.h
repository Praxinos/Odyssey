// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Layout/Margin.h"
#include "Sections/ThumbnailSection.h"
#include "TrackEditors/SubTrackEditorBase.h"

#include "KeyThumbnail/KeyThumbnailSection.h"

class FCinematicBoardTrackEditor;
class FMenuBuilder;
class FSequencerSectionPainter;
class FTrackEditorThumbnailPool;
class SCinematicBoardSectionTitle;
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
    virtual TSharedRef<SWidget> GenerateSectionWidget() override;
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
    virtual void BuildKeys() override;
    virtual TArray<double> GetKeys() const override; //TODO: maybe move it to UMovieScene*Section ? but what to do with TimeSpace ?

    //---

    // Draw the thumbnails by calling Super::OnPaintSection(...)
    int32 OnPaintSectionThumbnails( FSequencerSectionPainter& ioPainter ) const;

private:

    /** Add board takes menu */
    //void AddTakesMenu( FMenuBuilder& ioMenuBuilder );

private:
    TArray<double> mKeys;

private:
    // Store data about a subsequence using the hierarchy to get camera/planes/...
    struct FInnerSequenceResult
    {
        FInnerSequenceResult();
        FInnerSequenceResult( const FMovieSceneSequenceID& iID, const FMovieSceneSequenceHierarchy* iHierarchy, IMovieScenePlayer* ioPlayer );

        bool IsValid() const;
        bool IsFilled() const;

        const FMovieSceneSequenceID             mInnerSequenceID;
        const FMovieSceneSequenceHierarchy*     mHierarchy;
        IMovieScenePlayer*                      mPlayer;

        const FMovieSceneSequenceHierarchyNode* mNode;
        const FMovieSceneSubSequenceData*       mSubData;
        UMovieSceneSequence*                    mInnerMovieSceneSequence;
        UMovieScene*                            mInnerMovieScene;
    };

    FInnerSequenceResult GetInnerSequenceID( const UMovieSceneSubSection* iSubSection = nullptr ) const;
    void FillInnerSequenceResult( FInnerSequenceResult& iInnerSequenceResult ) const;
    UCameraComponent* FindCameraCutComponentRecursive( FFrameNumber iGlobalTime, FInnerSequenceResult iInnerSequenceResult );

private:
    /** Get all (static mesh) possessables inside the given section
        This is used by GetMaxPlaneBindings() which loops over all sections in the track
        to get section with the max number of planes
    */
    TArray<FMovieScenePossessable> GetPlaneBindings( const UMovieSceneSubSection& iSection ) const;

public:
    /** Get all (static mesh) possessables inside the current subsection */
    TArray<FMovieScenePossessable> GetPlaneBindings() const;
    /** Get the camera possessable inside the current subsection */
    FMovieScenePossessable GetCameraBinding() const;
    /** Get the maximum number of planes inside all subsections of the current track */
    int GetMaxPlaneBindings() const;

private:
    // To be able to call GetSectionObjectAs()/GetSequencer() inside sub-widgets (because GetSectionObjectAs()/GetSequencer() are protected)
    // This is the easiest way to achieve this without having to readd publicly the same functions
    friend class SCinematicBoardSectionThumbnails;
    friend class SCinematicBoardSectionPlanes;

    TSharedPtr<SCinematicBoardSectionTitle> mWidgetTitle;

public:
    /** Get the painter (named root) provided by OnPaintSection()
        FPaintArgs parameter must be given to try to force the call of this function only inside SWidget::OnPaint() functions
    */
    const FSequencerSectionPainter* GetRootPainter( const FPaintArgs& ) const;

private:
    /** This is used to be able to get all parameters of the ioPainter object given by OnPaintSection()
        Then, parameters (selected/highlighted/...) can be used inside the custom widgets (SCinematicBoardSection...)

        This is possible because inside the 'mother' widget (SSequencerSection) (which also create all custom subwidgets inside GenerateSectionWidget()),
        the functions sectionInterface->OnPaintSection() (aka this class) and SCompoundWidget::OnPaint() are called right one after other.
        So we can store the ioPainter get on OnPaintSection() and use it when OnPaint() is called inside custom widgets.

        @Warning: the variable is NOT valid ouside this case ! even if it is not null !
        It can't be set to nullptr once every OnPaint() functions are called because it's not possible to know which will be the last one.

        (It's mutable because it is used inside OnPaintSection() which is const)
    */
    mutable const FSequencerSectionPainter* mRootPainter;

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
