// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Layout/Margin.h"
#include "Sections/ThumbnailSection.h"
#include "TrackEditors/SubTrackEditorBase.h"

#include "CinematicBoardTrack/CinematicBoardSectionHelpers.h"
#include "KeyThumbnail/KeyThumbnailSection.h"

class FCinematicBoardTrackEditor;
class FMenuBuilder;
class FSequencerSectionPainter;
class FTrackEditorThumbnailPool;
class SCinematicBoardSectionLayout;
class SCinematicBoardSectionTitle;
class UMovieSceneCinematicBoardSection;
struct FInnerSequenceData;

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
    virtual void BuildThumbnailKeys() override;
    virtual TArray<double> GetThumbnailKeys() const override; //TODO: use const& for return type ???????????????????????? check where it is called !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    //---

    FTimeToPixel ConstructConverterForViewRange( FGeometry* oGeometry = nullptr ) const;
    FTimeToPixel ConstructConverterForSection( const FGeometry& iGeometry ) const;

    //---

    virtual void BuildCameraTransformChannelProxy();
    virtual TSharedPtr<FMovieSceneChannelProxy> GetCameraTransformChannelProxy() const;
    virtual void ReBuildCameraTransformMetaChannel();
    virtual TSharedPtr<FMetaFloatChannel> GetCameraTransformMetaChannel() const;

    virtual void BuildPlanesTransformChannelProxy();
    virtual TSharedPtr<FMovieSceneChannelProxy> GetPlaneTransformChannelProxy( FMovieScenePossessable iPossessable ) const;
    virtual void ReBuildPlanesTransformMetaChannel();
    virtual TSharedPtr<FMetaFloatChannel> GetPlaneTransformMetaChannel( FMovieScenePossessable iPossessable ) const;

    virtual void BuildPlaneMaterialsKeys();
    virtual TArray<double> GetPlaneMaterialKeys( FMovieScenePossessable iPossessable ) const;

private:
    TArray<double> mThumbnailKeys;
    TSharedPtr<FMovieSceneChannelProxy> mCameraTransformKeys;
    TSharedPtr<FMetaFloatChannel>       mCameraTransformMetaKeys;
    TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>>    mPlanesTransformsKeys;
    TMap<FGuid, TSharedPtr<FMetaFloatChannel>>          mPlanesTransformsMetaKeys;
    TMap<FGuid, TArray<double>>         mPlaneMaterialsKeys;

private:

    /** Add board takes menu */
    //void AddTakesMenu( FMenuBuilder& ioMenuBuilder );

private:
    UCameraComponent* FindCameraCutComponentRecursive( FFrameNumber iGlobalTime, FInnerSequenceData& iInnerSequenceData );

private:
    TSharedPtr<SCinematicBoardSectionLayout> mWidgetLayout;
    TSharedPtr<SCinematicBoardSectionTitle> mWidgetTitle;

public:
    /** Get the painter (named root) provided by OnPaintSection()
        FPaintArgs parameter must be given to try to force the call of this function only inside SWidget::OnPaint() functions
    */
    const FSequencerSectionPainter* GetRootPainter( const FPaintArgs& ) const;

    // To be able to call GetSectionObjectAs()/GetSequencer() inside sub-widgets (because GetSectionObjectAs()/GetSequencer() are protected in the parent)
    TSharedPtr<ISequencer> GetSequencer() const { return TSubSectionMixin::GetSequencer(); }
    UMovieSceneSubSection& GetSubSectionObject() { return TSubSectionMixin::GetSubSectionObject(); }
    const UMovieSceneSubSection& GetSubSectionObject() const { return TSubSectionMixin::GetSubSectionObject(); }

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
