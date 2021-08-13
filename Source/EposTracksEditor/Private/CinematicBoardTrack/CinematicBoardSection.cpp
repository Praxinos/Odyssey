// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardTrack/CinematicBoardSection.h"

#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "Rendering/DrawElements.h"
#include "Textures/SlateIcon.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ScopedTransaction.h"
#include "MovieSceneTrack.h"
#include "MovieScene.h"
#include "SequencerSectionPainter.h"
#include "EditorStyleSet.h"
#include "MovieSceneToolHelpers.h"
#include "MovieSceneTimeHelpers.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "CommonMovieSceneTools.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Editor.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Engine/StaticMeshActor.h"
#include "CineCameraActor.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/CinematicBoardTrackEditor.h"
#include "CinematicBoardTrack/MetaChannelProxy.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposMovieSceneSequence.h"
#include "EposSequenceHelpers.h"
#include "Helpers/SectionHelpersConvert.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Shot/ShotSequence.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "CinematicBoardWidgets/SCinematicBoardSectionContent.h"

#define LOCTEXT_NAMESPACE "FCinematicBoardSection"


/* FCinematicBoardSection structors
 *****************************************************************************/

FCinematicBoardSection::FCinematicSectionCacheForThumbnail::FCinematicSectionCacheForThumbnail( UMovieSceneCinematicBoardSection* iSection )
    : mInnerFrameRate( 1, 1 )
    , mInnerFrameOffset( 0 )
    , mSectionStartFrame( 0 )
    , mTimeScale( 1.f )
{
    if( iSection )
    {
        UMovieSceneSequence* innerSequence = iSection->GetSequence();
        if( innerSequence )
        {
            mInnerFrameRate = innerSequence->GetMovieScene()->GetTickResolution();
        }

        mInnerFrameOffset = iSection->Parameters.StartFrameOffset;
        mSectionStartFrame = iSection->HasStartFrame() ? iSection->GetInclusiveStartFrame() : 0;
        mTimeScale = iSection->Parameters.TimeScale;
    }
}

bool
FCinematicBoardSection::FCinematicSectionCacheForThumbnail::operator!=( const FCinematicSectionCacheForThumbnail& iRHS ) const
{
    return mInnerFrameRate != iRHS.mInnerFrameRate
        || mInnerFrameOffset != iRHS.mInnerFrameOffset
        || mSectionStartFrame != iRHS.mSectionStartFrame
        || mTimeScale != iRHS.mTimeScale;
}

//---

FCinematicBoardSection::FViewCachedState::FViewCachedState( const UMovieSceneCinematicBoardSection& iSection, TSharedPtr<ISequencer> iSequencer )
{
    check( iSequencer );

    //if( iSequencer->GetTopTimeSliderWidget()->GetTickSpaceGeometry().GetLocalSize().IsNearlyZero() ) // In the first tick(s), geometry is empty, but it's not the better way to manage this
    //    return;

    //FTimeToPixel TimeToPixelConverter = ...; // Get as parameter

    //const UMovieScene* MovieScene = iSection.GetTypedOuter<UMovieScene>();

    // Gather keys for a region larger than the view range to ensure we draw keys that are only just offscreen.
    // Compute visible range taking into account a half-frame offset for keys, plus half a key width for keys that are partially offscreen
    //TRange<FFrameNumber> SectionRange = iSection.GetRange();
    //const double         HalfKeyWidth = 0.5f * ( TimeToPixelConverter.PixelToSeconds( SequencerSectionConstants::KeySize.X ) - TimeToPixelConverter.PixelToSeconds( 0 ) );
    //TRange<double>       VisibleRange = UE::MovieScene::DilateRange( iSequencer->GetViewRange(), -HalfKeyWidth, HalfKeyWidth );
    //TRange<FFrameNumber> ValidKeyRange = iSequencer->GetSubSequenceRange().Get( MovieScene->GetPlaybackRange() ); // ?

    //ValidPlayRangeMin = UE::MovieScene::DiscreteInclusiveLower( ValidKeyRange );
    //ValidPlayRangeMax = UE::MovieScene::DiscreteExclusiveUpper( ValidKeyRange );
    //PaddedViewRange = TRange<double>::Intersection( SectionRange / MovieScene->GetTickResolution(), VisibleRange );
    //SelectionSerial = Sequencer->GetSelection().GetSerialNumber();
    //SelectionPreviewHash = Sequencer->GetSelectionPreview().GetSelectionHash();

    mPaddedViewRange = iSequencer->GetViewRange();
}

bool
FCinematicBoardSection::FViewCachedState::operator!=( const FViewCachedState& iRHS ) const
{
    const double RangeSize = mPaddedViewRange.Size<double>();
    const double OtherRangeSize = iRHS.mPaddedViewRange.Size<double>();

    return !FMath::IsNearlyEqual( RangeSize, OtherRangeSize, RangeSize * 0.001 );

    //ECacheFlags Flags = ECacheFlags::None;

    //if( ValidPlayRangeMin != Other.ValidPlayRangeMin || ValidPlayRangeMax != Other.ValidPlayRangeMax )
    //{
    //    // The valid key ranges for the data has changed
    //    Flags |= ECacheFlags::KeyStateChanged;
    //}

    //if( SelectionSerial != Other.SelectionSerial || SelectionPreviewHash != Other.SelectionPreviewHash )
    //{
    //    // Selection states have changed
    //    Flags |= ECacheFlags::KeyStateChanged;
    //}

    //if( PaddedViewRange != Other.PaddedViewRange )
    //{
    //    Flags |= ECacheFlags::ViewChanged;

    //    const double RangeSize = PaddedViewRange.Size<double>();
    //    const double OtherRangeSize = Other.PaddedViewRange.Size<double>();

    //    if( !FMath::IsNearlyEqual( RangeSize, OtherRangeSize, RangeSize * 0.001 ) )
    //    {
    //        Flags |= ECacheFlags::ViewZoomed;
    //    }
    //}

    //return Flags;
}

//---
//---
//---

FCinematicBoardSection::FCinematicBoardSection( TSharedPtr<ISequencer> iSequencer, UMovieSceneCinematicBoardSection& iSection, TSharedPtr<FCinematicBoardTrackEditor> iCinematicBoardTrackEditor, TSharedPtr<FTrackEditorThumbnailPool> iThumbnailPool )
    : TSubSectionMixin( iSequencer, iSection, iSequencer, iThumbnailPool, iSection )
    , mCinematicBoardTrackEditor( iCinematicBoardTrackEditor )
    , mThumbnailCacheData( &iSection )
    , mViewCacheState( iSection, iSequencer )
{
    AdditionalDrawEffect = ESlateDrawEffect::NoGamma;

    iSection.SetWidgetHeight( MakeAttributeLambda( [this](){ return mWidgetSectionContent.IsValid() ? mWidgetSectionContent->GetDesiredSize().Y : 100.f; } ) );
}


FCinematicBoardSection::~FCinematicBoardSection()
{
    // Doesn't work, because when the gui is rebuild:
    // - first, create the new FCinematicBoardSections
    // - then, delete the old ones
    // In this order, the next line will reset the attribute of the UMovieSceneCinematicBoardSection after it has been set in the constructor for the new ones
    //Cast<UMovieSceneCinematicBoardSection>( Section )->SetWidgetHeight( 0 );
}

FText
FCinematicBoardSection::GetSectionTitle() const
{
    return FText::GetEmpty(); // Now manage inside Title widget and don't want to be displayed at a position defined by the 'mother' SSequencerSection
    //return GetRenameVisibility() == EVisibility::Visible ? FText::GetEmpty() : HandleThumbnailTextBlockText();
}

FText
FCinematicBoardSection::GetSectionToolTip() const
{
    const UMovieSceneCinematicBoardSection& SectionObject = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();
    const UMovieScene* MovieScene = SectionObject.GetTypedOuter<UMovieScene>();
    const UMovieSceneSequence* InnerSequence = SectionObject.GetSequence();
    const UMovieScene* InnerMovieScene = InnerSequence ? InnerSequence->GetMovieScene() : nullptr;

    if( !MovieScene || !InnerMovieScene || !SectionObject.HasStartFrame() || !SectionObject.HasEndFrame() )
    {
        return FText::GetEmpty();
    }

    FFrameRate InnerTickResolution = InnerMovieScene->GetTickResolution();

    // Calculate the length of this section and convert it to the timescale of the sequence's internal sequence
    FFrameTime SectionLength = ConvertFrameTime( SectionObject.GetExclusiveEndFrame() - SectionObject.GetInclusiveStartFrame(), MovieScene->GetTickResolution(), InnerTickResolution );

    // Calculate the inner start time of the sequence in both full tick resolution and frame number
    FFrameTime StartOffset = SectionObject.GetOffsetTime().Get( 0 );
    FFrameTime InnerStartTime = InnerMovieScene->GetPlaybackRange().GetLowerBoundValue() + StartOffset;
    int32 InnerStartFrame = ConvertFrameTime( InnerStartTime, InnerTickResolution, InnerMovieScene->GetDisplayRate() ).RoundToFrame().Value;

    // Calculate the length, which is limited by both the outer section length and internal sequence length, in terms of internal frames
    int32 InnerFrameLength = ConvertFrameTime( FMath::Min( SectionLength, InnerMovieScene->GetPlaybackRange().GetUpperBoundValue() - InnerStartTime ), InnerTickResolution, InnerMovieScene->GetDisplayRate() ).RoundToFrame().Value;

    // Calculate the inner frame number of the end frame
    int32 InnerEndFrame = InnerStartFrame + InnerFrameLength;

    return FText::Format( LOCTEXT( "ToolTipContentFormat", "{0} - {1} ({2} frames @ {3})" ),
                          InnerStartFrame,
                          InnerEndFrame,
                          InnerFrameLength,
                          InnerMovieScene->GetDisplayRate().ToPrettyText()
    );
}

float
FCinematicBoardSection::GetSectionHeight() const
{
    float height = 100.f; // Arbitrary value which should only be used for one (or some) tick(s) waiting the creation of the layout widget in the section

    UMovieSceneCinematicBoardTrack* track = Section->GetTypedOuter<UMovieSceneCinematicBoardTrack>();
    if( track )
    {
        for( auto section : track->GetAllSections() )
        {
            UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( section );
            height = FMath::Max( height, board_section->GetWidgetHeight() );
        }
    }

    return height;
}

FMargin
FCinematicBoardSection::GetContentPadding() const
{
    return FMargin( 8.f, 15.f );
}

void
FCinematicBoardSection::SetSingleTime( double iGlobalTime )
{
    UMovieSceneCinematicBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();
    double referenceOffsetSeconds = sectionObject.HasStartFrame() ? sectionObject.GetInclusiveStartFrame() / sectionObject.GetTypedOuter<UMovieScene>()->GetTickResolution() : 0;
    sectionObject.SetThumbnailReferenceOffset( iGlobalTime - referenceOffsetSeconds );
}

bool
FCinematicBoardSection::IsReadOnly() const
{
    // Overridden to false regardless of movie scene section read only state so that we can double click into the sub section
    return false;
}

//---

UCameraComponent*
FCinematicBoardSection::FindCameraCutComponentRecursive( FFrameNumber iGlobalTime, FMovieSceneSequenceID InnerSequenceID, const FMovieSceneSequenceHierarchy& Hierarchy, IMovieScenePlayer& Player )
{
    const FMovieSceneSequenceHierarchyNode* Node = Hierarchy.FindNode( InnerSequenceID );
    const FMovieSceneSubSequenceData*       SubData = Hierarchy.FindSubData( InnerSequenceID );
    if( !ensure( SubData && Node ) )
    {
        return nullptr;
    }

    UMovieSceneSequence* InnerSequence = SubData->GetSequence();
    UMovieScene*         InnerMovieScene = InnerSequence ? InnerSequence->GetMovieScene() : nullptr;
    if( !InnerMovieScene )
    {
        return nullptr;
    }

    FFrameNumber InnerTime = ( iGlobalTime * SubData->RootToSequenceTransform ).FloorToFrame();
    if( !SubData->PlayRange.Value.Contains( InnerTime ) )
    {
        return nullptr;
    }

    int32 LowestRow = TNumericLimits<int32>::Max();
    int32 HighestOverlap = 0;

    UMovieSceneSingleCameraCutSection* ActiveSection = nullptr;

    if( UMovieSceneSingleCameraCutTrack* CutTrack = Cast<UMovieSceneSingleCameraCutTrack>( InnerMovieScene->GetCameraCutTrack() ) )
    {
        for( UMovieSceneSection* ItSection : CutTrack->GetAllSections() )
        {
            UMovieSceneSingleCameraCutSection* CutSection = Cast<UMovieSceneSingleCameraCutSection>( ItSection );
            if( CutSection && CutSection->GetRange().Contains( InnerTime ) )
            {
                bool bSectionWins =
                    ( CutSection->GetRowIndex() < LowestRow ) ||
                    ( CutSection->GetRowIndex() == LowestRow && CutSection->GetOverlapPriority() > HighestOverlap );

                if( bSectionWins )
                {
                    HighestOverlap = CutSection->GetOverlapPriority();
                    LowestRow = CutSection->GetRowIndex();
                    ActiveSection = CutSection;
                }
            }
        }
    }

    if( ActiveSection )
    {
        return ActiveSection->GetFirstCamera( Player, InnerSequenceID );
    }

    for( FMovieSceneSequenceID Child : Node->Children )
    {
        UCameraComponent* CameraComponent = FindCameraCutComponentRecursive( iGlobalTime, Child, Hierarchy, Player );
        if( CameraComponent )
        {
            return CameraComponent;
        }
    }

    return nullptr;
}

UCameraComponent*
FCinematicBoardSection::GetViewCamera()
{
    TSharedPtr<ISequencer> sequencer = GetSequencer();
    if( !sequencer.IsValid() )
        return nullptr;


    const UMovieSceneCinematicBoardSection& SectionObject = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();
    const FMovieSceneSequenceID             ThisSequenceID = sequencer->GetFocusedTemplateID();
    const FMovieSceneSequenceID             TargetSequenceID = SectionObject.GetSequenceID();
    const FMovieSceneSequenceHierarchy*     Hierarchy = sequencer->GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( sequencer->GetEvaluationTemplate().GetCompiledDataID() );

    if( !Hierarchy )
        return nullptr;

    const FMovieSceneSequenceHierarchyNode* ThisSequenceNode = Hierarchy->FindNode( ThisSequenceID );

    check( ThisSequenceNode );

    // Find the TargetSequenceID by comparing deterministic sequence IDs for all children of the current node
    const FMovieSceneSequenceID* InnerSequenceID = Algo::FindByPredicate( ThisSequenceNode->Children,
                                                                          [Hierarchy, TargetSequenceID]( FMovieSceneSequenceID InSequenceID )
                                                                          {
                                                                              const FMovieSceneSubSequenceData* SubData = Hierarchy->FindSubData( InSequenceID );
                                                                              return SubData && SubData->DeterministicSequenceID == TargetSequenceID;
                                                                          }
                                                                          );

    if( InnerSequenceID )
    {
        UCameraComponent* CameraComponent = FindCameraCutComponentRecursive( sequencer->GetGlobalTime().Time.FrameNumber, *InnerSequenceID, *Hierarchy, *sequencer );
        if( CameraComponent )
        {
            return CameraComponent;
        }
    }

    return nullptr;
}

//---

FTimeToPixel
FCinematicBoardSection::ConstructConverterForViewRange( FGeometry* oGeometry ) const
{
    check( GetSequencer() );

    FGeometry geometry( GetSequencer()->GetTopTimeSliderWidget()->GetTickSpaceGeometry() );
    if( oGeometry )
        *oGeometry = geometry;

    return FTimeToPixel( geometry, GetSequencer()->GetViewRange(), GetSequencer()->GetFocusedTickResolution() );
}

FTimeToPixel
FCinematicBoardSection::ConstructConverterForSection( const FGeometry& iGeometry ) const
{
    const UMovieSceneCinematicBoardSection& section_object = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();
    FFrameRate     TickResolution = section_object.GetTypedOuter<UMovieScene>()->GetTickResolution();
    double         LowerTime = section_object.GetInclusiveStartFrame() / TickResolution;
    double         UpperTime = section_object.GetExclusiveEndFrame() / TickResolution;

    return FTimeToPixel( iGeometry, TRange<double>( LowerTime, UpperTime ), TickResolution );
}

//---

void
FCinematicBoardSection::BuildKeys() //override
{
    FKeyThumbnailSection::BuildKeys();
    BuildCameraTransformChannelProxy();
    BuildPlanesTransformChannelProxy();
    BuildPlanesMaterialChannelProxy();
}

void
FCinematicBoardSection::BuildThumbnailKeys() //override
{
    check( TimeSpace == ETimeSpace::Global ); // Otherwise, TimeSpace must be add as a parameter

    TArray<FFrameTime> keys_as_frame = BoardSequenceHelpers::GetCameraTransformTimesRecursive( GetSubSectionObject() );
    mThumbnailKeys = SectionHelpersConvert::FrameToSecond( &GetSubSectionObject(), keys_as_frame );
}

TArray<double>
FCinematicBoardSection::GetThumbnailKeys() const //override
{
    return mThumbnailKeys;
}

void
FCinematicBoardSection::BuildCameraTransformChannelProxy()
{
    check( TimeSpace == ETimeSpace::Global ); // Otherwise, TimeSpace must be add as a parameter

    mCameraTransformKeys = BoardSequenceHelpers::BuildCameraTransformChannelProxy( *GetSequencer(), GetSubSectionObject(), GetSequencer()->GetFocusedTemplateID() );

    ReBuildCameraTransformMetaChannel();
}

TSharedPtr<FMovieSceneChannelProxy>
FCinematicBoardSection::GetCameraTransformChannelProxy() const
{
    return mCameraTransformKeys;
}

void
FCinematicBoardSection::ReBuildCameraTransformMetaChannel()
{
    FTimeToPixel converter( ConstructConverterForViewRange() );

    const FFrameTime HalfKeySizeFrames = converter.PixelDeltaToFrame( SequencerSectionConstants::KeySize.X * .5f );
    const FMovieSceneSequenceTransform OuterToInnerTransform = GetSubSectionObject().OuterToInnerTransform();
    FFrameTime clicked_frame = 0; // As if we are on frame 0
    TRange<FFrameNumber> inner_range_tolerance( ( ( clicked_frame - HalfKeySizeFrames ) * OuterToInnerTransform ).FloorToFrame(), ( ( clicked_frame + HalfKeySizeFrames ) * OuterToInnerTransform ).CeilToFrame() );
    FFrameNumber inner_tolerance = inner_range_tolerance.Size<FFrameNumber>() / 2;

    mCameraTransformMetaKeys = MakeShared<FMetaFloatChannel>( inner_tolerance );
    mCameraTransformMetaKeys->Build( mCameraTransformKeys );
}

TSharedPtr<FMetaFloatChannel>
FCinematicBoardSection::GetCameraTransformMetaChannel() const
{
    return mCameraTransformMetaKeys;
}

//-

void
FCinematicBoardSection::BuildPlanesTransformChannelProxy()
{
    mPlanesTransformsKeys = BoardSequenceHelpers::BuildPlanesTransformChannelProxy( *GetSequencer(), GetSubSectionObject(), GetSequencer()->GetFocusedTemplateID() );

    ReBuildPlanesTransformMetaChannel();
}

TSharedPtr<FMovieSceneChannelProxy>
FCinematicBoardSection::GetPlaneTransformChannelProxy( FMovieScenePossessable iPossessable ) const
{
    if( !mPlanesTransformsKeys.Contains( iPossessable.GetGuid() ) )
        return nullptr;

    return mPlanesTransformsKeys[iPossessable.GetGuid()];
}

void
FCinematicBoardSection::ReBuildPlanesTransformMetaChannel()
{
    FTimeToPixel converter( ConstructConverterForViewRange() );

    const FFrameTime HalfKeySizeFrames = converter.PixelDeltaToFrame( SequencerSectionConstants::KeySize.X * .5f );
    const FMovieSceneSequenceTransform OuterToInnerTransform = GetSubSectionObject().OuterToInnerTransform();
    FFrameTime clicked_frame = 0; // As if we are on frame 0
    TRange<FFrameNumber> inner_range_tolerance( ( ( clicked_frame - HalfKeySizeFrames ) * OuterToInnerTransform ).FloorToFrame(), ( ( clicked_frame + HalfKeySizeFrames ) * OuterToInnerTransform ).CeilToFrame() );
    FFrameNumber inner_tolerance = inner_range_tolerance.Size<FFrameNumber>() / 2;

    mPlanesTransformsMetaKeys.Empty();
    for( const auto& pair : mPlanesTransformsKeys )
    {
        FGuid guid = pair.Key;
        TSharedPtr<FMovieSceneChannelProxy> proxy = pair.Value;

        TSharedPtr<FMetaFloatChannel> meta_channel = MakeShared<FMetaFloatChannel>( inner_tolerance );
        meta_channel->Build( proxy );

        mPlanesTransformsMetaKeys.Add( guid, meta_channel );
    }
}

TSharedPtr<FMetaFloatChannel>
FCinematicBoardSection::GetPlaneTransformMetaChannel( FMovieScenePossessable iPossessable ) const
{
    if( !mPlanesTransformsMetaKeys.Contains( iPossessable.GetGuid() ) )
        return nullptr;

    return mPlanesTransformsMetaKeys[iPossessable.GetGuid()];
}

//-

void
FCinematicBoardSection::BuildPlanesMaterialChannelProxy()
{
    mPlanesMaterialsKeys = BoardSequenceHelpers::BuildPlanesMaterialChannelProxy( *GetSequencer(), GetSubSectionObject(), GetSequencer()->GetFocusedTemplateID() );

    ReBuildPlanesMaterialMetaChannel();
}

TSharedPtr<FMovieSceneChannelProxy>
FCinematicBoardSection::GetPlaneMaterialChannelProxy( FMovieScenePossessable iPossessable ) const
{
    if( !mPlanesMaterialsKeys.Contains( iPossessable.GetGuid() ) )
        return nullptr;

    return mPlanesMaterialsKeys[iPossessable.GetGuid()];
}

void
FCinematicBoardSection::ReBuildPlanesMaterialMetaChannel()
{
    FTimeToPixel converter( ConstructConverterForViewRange() );

    const FFrameTime HalfKeySizeFrames = converter.PixelDeltaToFrame( SequencerSectionConstants::KeySize.X * .5f );
    const FMovieSceneSequenceTransform OuterToInnerTransform = GetSubSectionObject().OuterToInnerTransform();
    FFrameTime clicked_frame = 0; // As if we are on frame 0
    TRange<FFrameNumber> inner_range_tolerance( ( ( clicked_frame - HalfKeySizeFrames ) * OuterToInnerTransform ).FloorToFrame(), ( ( clicked_frame + HalfKeySizeFrames ) * OuterToInnerTransform ).CeilToFrame() );
    FFrameNumber inner_tolerance = inner_range_tolerance.Size<FFrameNumber>() / 2;

    mPlanesMaterialsMetaKeys.Empty();
    for( const auto& pair : mPlanesMaterialsKeys )
    {
        FGuid guid = pair.Key;
        TSharedPtr<FMovieSceneChannelProxy> proxy = pair.Value;

        TSharedPtr<FMetaMaterialChannel> meta_channel = MakeShared<FMetaMaterialChannel>( inner_tolerance );
        meta_channel->Build( proxy );

        mPlanesMaterialsMetaKeys.Add( guid, meta_channel );
    }
}

TSharedPtr<FMetaMaterialChannel>
FCinematicBoardSection::GetPlaneMaterialMetaChannel( FMovieScenePossessable iPossessable ) const
{
    if( !mPlanesMaterialsMetaKeys.Contains( iPossessable.GetGuid() ) )
        return nullptr;

    return mPlanesMaterialsMetaKeys[iPossessable.GetGuid()];
}

//---

TSharedRef<SWidget>
FCinematicBoardSection::GenerateSectionWidget()
{
    return SAssignNew( mWidgetSectionContent, SCinematicBoardSectionContent, SharedThis( this ) );
}

void
FCinematicBoardSection::Tick( const FGeometry& iAllottedGeometry, const FGeometry& iClippedGeometry, const double iCurrentTime, const float iDeltaTime )
{
    // Set cached data
    UMovieSceneCinematicBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();
    FCinematicSectionCacheForThumbnail newCacheData( &sectionObject );
    if( newCacheData != mThumbnailCacheData )
    {
        KeyThumbnailCache.ForceRedraw();
    }
    mThumbnailCacheData = newCacheData;

    FViewCachedState newCacheState( sectionObject, GetSequencer() );
    if( newCacheState != mViewCacheState )
    {
        BuildKeys();
    }
    mViewCacheState = newCacheState;

    // Update single reference frame settings
    if( GetDefault<UMovieSceneUserThumbnailSettings>()->bDrawSingleThumbnails && sectionObject.HasStartFrame() )
    {
        double referenceTime = sectionObject.GetInclusiveStartFrame() / sectionObject.GetTypedOuter<UMovieScene>()->GetTickResolution() + sectionObject.GetThumbnailReferenceOffset();
        KeyThumbnailCache.SetSingleReferenceFrame( referenceTime );
    }
    else
    {
        KeyThumbnailCache.SetSingleReferenceFrame( TOptional<double>() );
    }

    FKeyThumbnailSection::Tick( iAllottedGeometry, iClippedGeometry, iCurrentTime, iDeltaTime );
}

const FSequencerSectionPainter*
FCinematicBoardSection::GetRootPainter( const FPaintArgs& ) const
{
    return mRootPainter;
}

int32
FCinematicBoardSection::OnPaintSection( FSequencerSectionPainter& ioPainter ) const
{
    const UMovieSceneCinematicBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();
    const UEposTracksEditorSettings* settings = GetDefault<UEposTracksEditorSettings>();

    if( Cast<UBoardSequence>( sectionObject.GetSequence() ) )
        ioPainter.LayerId = ioPainter.PaintSectionBackground( settings->BoardTrackSettings.BoardSectionColor );
    else if( Cast<UShotSequence>( sectionObject.GetSequence() ) )
        ioPainter.LayerId = ioPainter.PaintSectionBackground( settings->BoardTrackSettings.ShotSectionColor );
    else
        ioPainter.LayerId = ioPainter.PaintSectionBackground();

    //---

    mRootPainter = &ioPainter;

    return ioPainter.LayerId;
}

void
FCinematicBoardSection::BuildSectionContextMenu( FMenuBuilder& ioMenuBuilder, const FGuid& iObjectBinding )
{
    FKeyThumbnailSection::BuildSectionContextMenu( ioMenuBuilder, iObjectBinding );

    UMovieSceneCinematicBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();

    ioMenuBuilder.BeginSection( NAME_None, LOCTEXT( "BoardMenuText", "Board" ) );
    {
        //ioMenuBuilder.AddSubMenu(
        //    LOCTEXT( "TakesMenu", "Takes" ),
        //    LOCTEXT( "TakesMenuTooltip", "Shot takes" ),
        //    FNewMenuDelegate::CreateLambda( [=]( FMenuBuilder& InMenuBuilder )
        //{
        //    AddTakesMenu( InMenuBuilder );
        //} ) );

        //ioMenuBuilder.AddMenuEntry(
        //    LOCTEXT( "NewTake", "New Take" ),
        //    FText::Format( LOCTEXT( "NewTakeTooltip", "Create a new take for {0}" ), FText::FromString( SectionObject.GetShotDisplayName() ) ),
        //    FSlateIcon(),
        //    FUIAction( FExecuteAction::CreateSP( CinematicShotTrackEditor.Pin().ToSharedRef(), &FCinematicShotTrackEditor::NewTake, &SectionObject ) )
        //);

        ioMenuBuilder.AddMenuEntry(
            LOCTEXT( "InsertNewBoard", "Insert Board" ),
            LOCTEXT( "InsertNewBoardTooltip", "Insert a new board at the current time" ),
            FSlateIcon(),
            FUIAction( FExecuteAction::CreateSP( mCinematicBoardTrackEditor.Pin().ToSharedRef(), &FCinematicBoardTrackEditor::InsertBoard ) )
        );

        ioMenuBuilder.AddMenuEntry(
            LOCTEXT( "DuplicateBoard", "Duplicate Board" ),
            FText::Format( LOCTEXT( "DuplicateBoardTooltip", "Duplicate {0} to create a new board" ), FText::FromString( sectionObject.GetBoardDisplayName() ) ),
            FSlateIcon(),
            FUIAction( FExecuteAction::CreateSP( mCinematicBoardTrackEditor.Pin().ToSharedRef(), &FCinematicBoardTrackEditor::DuplicateBoard, &sectionObject ) )
        );

        //ioMenuBuilder.AddMenuEntry(
        //    LOCTEXT( "RenderBoard", "Render Board" ),
        //    FText::Format( LOCTEXT( "RenderBoardTooltip", "Render board movie" ), FText::FromString( sectionObject.GetBoardDisplayName() ) ),
        //    FSlateIcon(),
        //    FUIAction( FExecuteAction::CreateLambda( [this, &sectionObject]()
        //                                             {
        //                                                 TArray<UMovieSceneCinematicBoardSection*> BoardSections;
        //                                                 TArray<UMovieSceneSection*> Sections;
        //                                                 GetSequencer()->GetSelectedSections( Sections );
        //                                                 for( UMovieSceneSection* Section : Sections )
        //                                                 {
        //                                                     if( UMovieSceneCinematicBoardSection* BoardSection = Cast<UMovieSceneCinematicBoardSection>( Section ) )
        //                                                     {
        //                                                         BoardSections.Add( BoardSection );
        //                                                     }
        //                                                 }

        //                                                 if( !BoardSections.Contains( &sectionObject ) )
        //                                                 {
        //                                                     BoardSections.Add( &sectionObject );
        //                                                 }

        //                                                 mCinematicBoardTrackEditor.Pin()->RenderBoards( BoardSections );
        //                                             } ) )
        //);

        ioMenuBuilder.AddMenuEntry(
            LOCTEXT( "RenameBoard", "Rename Board" ),
            FText::Format( LOCTEXT( "RenameBoardTooltip", "Rename {0}" ), FText::FromString( sectionObject.GetBoardDisplayName() ) ),
            FSlateIcon(),
            FUIAction( FExecuteAction::CreateSP( mWidgetSectionContent.ToSharedRef(), &SCinematicBoardSectionContent::EnterRename ) )
            //FUIAction( FExecuteAction::CreateSP( mWidgetTitle.ToSharedRef(), &SCinematicBoardSectionTitle::EnterRename ) )
        );
    }
    ioMenuBuilder.EndSection();
}

//void FCinematicShotSection::AddTakesMenu( FMenuBuilder& MenuBuilder )
//{
//    TArray<FAssetData> AssetData;
//    uint32 CurrentTakeNumber = INDEX_NONE;
//    const UMovieSceneCinematicShotSection& SectionObject = GetSectionObjectAs<UMovieSceneCinematicShotSection>();
//    MovieSceneToolHelpers::GatherTakes( &SectionObject, AssetData, CurrentTakeNumber );
//
//    AssetData.Sort( [&SectionObject]( const FAssetData &A, const FAssetData &B )
//    {
//        uint32 TakeNumberA = INDEX_NONE;
//        uint32 TakeNumberB = INDEX_NONE;
//        if( MovieSceneToolHelpers::GetTakeNumber( &SectionObject, A, TakeNumberA ) && MovieSceneToolHelpers::GetTakeNumber( &SectionObject, B, TakeNumberB ) )
//        {
//            return TakeNumberA < TakeNumberB;
//        }
//        return true;
//    } );
//
//    for( auto ThisAssetData : AssetData )
//    {
//        uint32 TakeNumber = INDEX_NONE;
//        if( MovieSceneToolHelpers::GetTakeNumber( &SectionObject, ThisAssetData, TakeNumber ) )
//        {
//            UObject* TakeObject = ThisAssetData.GetAsset();
//
//            if( TakeObject )
//            {
//                MenuBuilder.AddMenuEntry(
//                    FText::Format( LOCTEXT( "TakeNumber", "Take {0}" ), FText::AsNumber( TakeNumber ) ),
//                    FText::Format( LOCTEXT( "TakeNumberTooltip", "Switch to {0}" ), FText::FromString( TakeObject->GetPathName() ) ),
//                    TakeNumber == CurrentTakeNumber ? FSlateIcon( FEditorStyle::GetStyleSetName(), "Sequencer.Star" ) : FSlateIcon( FEditorStyle::GetStyleSetName(), "Sequencer.Empty" ),
//                    FUIAction( FExecuteAction::CreateSP( CinematicShotTrackEditor.Pin().ToSharedRef(), &FCinematicShotTrackEditor::SwitchTake, TakeObject ) )
//                );
//            }
//        }
//    }
//}

/* FCinematicBoardSection callbacks
 *****************************************************************************/

FText
FCinematicBoardSection::HandleThumbnailTextBlockText() const
{
    const UMovieSceneCinematicBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();
    return FText::FromString( sectionObject.GetBoardDisplayName() );
}


void
FCinematicBoardSection::HandleThumbnailTextBlockTextCommitted( const FText& iNewBoardName, ETextCommit::Type iCommitType )
{
    if( iCommitType == ETextCommit::OnEnter && !HandleThumbnailTextBlockText().EqualTo( iNewBoardName ) )
    {
        UMovieSceneCinematicBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();

        sectionObject.Modify();

        const FScopedTransaction transaction( LOCTEXT( "SetBoardName", "Set Board Name" ) );

        sectionObject.SetBoardDisplayName( iNewBoardName.ToString() );
    }
}

//---

void
FCinematicBoardSection::BeginResizeSection()
{
    UMovieSceneCinematicBoardSection& section = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();
    section.StartResizing();
}

void
FCinematicBoardSection::ResizeSection( ESequencerSectionResizeMode ResizeMode, FFrameNumber ResizeFrameNumber )
{
    UMovieSceneCinematicBoardSection& section = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();
    section.Resizing();

    if( ResizeMode == ESequencerSectionResizeMode::SSRM_LeadingEdge )
        section.ResizeLeadingEdge( ResizeFrameNumber );
    else
        section.ResizeTrailingEdge( ResizeFrameNumber );
};

void
FCinematicBoardSection::BeginSlipSection()
{
}

void
FCinematicBoardSection::SlipSection( FFrameNumber SlipTime )
{
}

void
FCinematicBoardSection::BeginDilateSection()
{
}

void
FCinematicBoardSection::DilateSection( const TRange<FFrameNumber>& NewRange, float DilationFactor )
{
}


#undef LOCTEXT_NAMESPACE
