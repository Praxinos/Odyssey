// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Layout/Margin.h"
#include "Sections/ThumbnailSection.h"
#include "TrackEditors/SubTrackEditorBase.h"

#include "KeyThumbnail/KeyThumbnailSection.h"
#include "EposSequenceHelpers.h"

class FCinematicBoardTrackEditor;
class FMenuBuilder;
class FMetaChannel;
class FSequencerSectionPainter;
class FTrackEditorThumbnailPool;
class SCinematicBoardSectionContent;
class UMovieSceneCinematicBoardSection;
struct FInnerSequenceData;

/**
 * Board section, which paints and ticks the appropriate section.
 */
class FCinematicBoardSection
    : public TSubSectionMixin<FKeyThumbnailSection>
    , public FGCObject
{
public:

    /** Create and initialize a new instance. */
    FCinematicBoardSection( TSharedPtr<ISequencer> iSequencer, UMovieSceneCinematicBoardSection& iSection, TSharedPtr<FCinematicBoardTrackEditor> iCinematicBoardTrackEditor, TSharedPtr<FTrackEditorThumbnailPool> iThumbnailPool );

    /** Virtual destructor. */
    virtual ~FCinematicBoardSection();

public:
    virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
    virtual FString GetReferencerName() const override;

public:

    // ISequencerSection interface
    virtual TSharedRef<SWidget> GenerateSectionWidget() override;
    virtual void    Tick( const FGeometry& iAllottedGeometry, const FGeometry& iClippedGeometry, const double iCurrentTime, const float iDeltaTime ) override;
    virtual int32   OnPaintSection( FSequencerSectionPainter& ioPainter ) const override;
    virtual void    BuildSectionContextMenu( FMenuBuilder& ioMenuBuilder, const FGuid& iObjectBinding ) override;
    virtual FText   GetSectionTitle() const override;
    virtual FText   GetSectionToolTip() const override;
    virtual float   GetSectionHeight( const UE::Sequencer::FViewDensityInfo& ViewDensity ) const override;
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

    virtual void RebuildChannelProxies();
    virtual void RebuildMetaChannels();

    virtual void BuildCameraTransformChannelProxy();
    virtual FChannelProxyBySectionMap GetCameraTransformChannelProxy() const;
    virtual void ReBuildCameraTransformMetaChannel();
    virtual TSharedPtr<FMetaChannel> GetCameraTransformMetaChannel() const;

    virtual void BuildAnimationsTransformChannelProxy();
    virtual FChannelProxyBySectionMap GetAnimationTransformChannelProxy( FMovieScenePossessable iPossessable ) const;
    virtual void ReBuildAnimationsTransformMetaChannel();
    virtual TSharedPtr<FMetaChannel> GetAnimationTransformMetaChannel( FMovieScenePossessable iPossessable ) const;

    virtual void BuildAnimationsTimelineChannelProxy();
    virtual FChannelProxyBySectionMap GetAnimationTimelineChannelProxy( FMovieScenePossessable iPossessable ) const;
    virtual void ReBuildAnimationsTimelineMetaChannel();
    virtual TSharedPtr<FMetaChannel> GetAnimationTimelineMetaChannel( FMovieScenePossessable iPossessable ) const;
    struct FThumbnailData
    {
        FQualifiedFrameTime QTime;
        FIntVector2 Size;
        //TObjectPtr<UTextureRenderTarget2D> RenderTarget;
        TObjectPtr<UTexture2D> Texture;
        FSlateBrush* Brush = nullptr;
        bool MultipleSubkey = false;
    };
    virtual const TArray<FThumbnailData>& GetAnimationTimelineThumbnails( FMovieScenePossessable iPossessable ) const;
    virtual void ReBuildAnimationsTimelineThumbnails( FMovieScenePossessable iPossessable, TOptional<FGuid> iFrameId = TOptional<FGuid>() );

    virtual void BuildAnimationsOpacityChannelProxy();
    virtual FChannelProxyBySectionMap GetAnimationOpacityChannelProxy( FMovieScenePossessable iPossessable ) const;
    virtual void ReBuildAnimationsOpacityMetaChannel();
    virtual TSharedPtr<FMetaChannel> GetAnimationOpacityMetaChannel( FMovieScenePossessable iPossessable ) const;

private:
    virtual void ReBuildAnimationsTimelineThumbnails( FGuid iGuid, TOptional<FGuid> iFrameId = TOptional<FGuid>() );
    virtual FThumbnailData RebuildAnimationThumbnailDataInternal( UOdysseyAnimationTimelineSection* iSection, FGuid iPossessable, FFrameNumber iFrameInSequence, TOptional<FGuid> iFrameId = TOptional<FGuid>() );

private:
    TArray<double> mThumbnailKeys;
    FChannelProxyBySectionMap   mCameraTransformChannelProxies;
    TSharedPtr<FMetaChannel>    mCameraTransformMetaChannel;
    TMap<FGuid, FChannelProxyBySectionMap>  mAnimationsTransformChannelProxies;
    TMap<FGuid, TSharedPtr<FMetaChannel>>   mAnimationsTransformMetaChannel;
    TMap<FGuid, FChannelProxyBySectionMap>  mAnimationsTimelineChannelProxies;
    TMap<FGuid, TSharedPtr<FMetaChannel>>   mAnimationsTimelineMetaChannel;
    TMap<FGuid, TArray<FThumbnailData>>     mAnimationsTimelineThumbnails;
    TMap<FGuid, FChannelProxyBySectionMap>  mAnimationsOpacityChannelProxies;
    TMap<FGuid, TSharedPtr<FMetaChannel>>   mAnimationsOpacityMetaChannel;

    struct FPoolData
    {
        //UTextureRenderTarget2D* RenderTarget = nullptr;
        TObjectPtr<UTexture2D> Texture;
    };
    typedef TArray<FGuid> FRenderingComposition;
    // A pool containing textures corresponding to each thumbnail and referenced by the render composition of the cell
    // ALL textures of ALL cells in ALL Animations of the section
    // The identifier is the rendering composition of a cell (in any animations)
    // The number of entry here should (must ?) be the same as the sum of entries in every mAnimationsTimelineThumbnails entries
    TMap<FRenderingComposition, FPoolData>  mAnimationsTimelineThumbnailPool;
    // A basic pool of raw textures which contains textures no more used and can be get to update them (instead of recreate a new one and let GC clean it every 10min)
    // A basic array to add/peak an unused texture when necessary
    TArray<TWeakObjectPtr<UTexture2D>>  mAnimationsTimelineTexturePool;
    // A single render target used to render an animation and recreate only if some animation parameters are no more compatible
    TObjectPtr<UTextureRenderTarget2D> mBuildRenderTargetTmp;

private:
    /** Add board takes menu */
    void AddTakesMenu( FMenuBuilder& ioMenuBuilder );

private:
    UCameraComponent* FindCameraCutComponentRecursive( FFrameNumber iGlobalTime, FMovieSceneSequenceID InnerSequenceID, const FMovieSceneSequenceHierarchy& Hierarchy, IMovieScenePlayer& Player );

private:
    TSharedPtr<SCinematicBoardSectionContent> mWidgetSectionContent;

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
    mutable const FSequencerSectionPainter* mRootPainter = nullptr;

private:

    /** The board track editor that contains this section */
    TWeakPtr<FCinematicBoardTrackEditor> mCinematicBoardTrackEditor;

    bool mNeedRebuild { false };

    struct FCinematicSectionCacheForThumbnail
    {
        FCinematicSectionCacheForThumbnail( UMovieSceneCinematicBoardSection* iSection = nullptr );

        bool operator!=( const FCinematicSectionCacheForThumbnail& iRHS ) const;

        FFrameRate   mInnerFrameRate;
        FFrameNumber mInnerFrameOffset;
        FFrameNumber mSectionStartFrame;
        FMovieSceneTimeWarpVariant  mTimeScale;
    };

    /** Cached section thumbnail data */
    FCinematicSectionCacheForThumbnail mThumbnailCacheData;

    //---

    // From (inspired by) ...\UE_4.26\Engine\Source\Editor\Sequencer\Private\SequencerKeyRenderer.cpp

    /** Structure that caches the various bits of information upon which our view is dependent */
    struct FViewCachedState
    {
        /** Construction from a section painter and sequencer object - populates the cached values */
        FViewCachedState( const UMovieSceneCinematicBoardSection& iSection, TSharedPtr<ISequencer> iSequencer );

        /** Compare this cache state to another */
        bool operator!=( const FViewCachedState& iRHS ) const;
        //ECacheFlags CompareTo( const FCachedState& Other ) const;

        /** The min/max tick value relating to the FMovieSceneSubSequenceData::ValidPlayRange bounds, or the current playback range */
        //FFrameNumber mValidPlayRangeMin, mValidPlayRangeMax;
        /** The current view range +/- the width of a key */
        TRange<double> mPaddedViewRange;
        /** The value of FSequencerSelection::GetSerialNumber when this cache was created */
        //uint32 mSelectionSerial = 0;
        /** The value of FSequencerSelectionPreview::GetSelectionHash when this cache was created */
        //uint32 mSelectionPreviewHash = 0;

        FGeometry mTimeSliderGeometry;
    };

    /** Cached section data */
    FViewCachedState mViewCacheState;
};
