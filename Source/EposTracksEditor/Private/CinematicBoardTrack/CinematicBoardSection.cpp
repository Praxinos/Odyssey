// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
#include "MVVM/ViewModels/EditorViewModel.h"
#include "MVVM/ViewModels/TrackAreaViewModel.h"
#include "SequencerSectionPainter.h"
#include "EditorStyleSet.h"
#include "MovieSceneToolHelpers.h"
#include "MovieSceneTimeHelpers.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Editor.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Engine/StaticMeshActor.h"
#include "CineCameraActor.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#include "AnimatedRange.h"
#include "ITimeSlider.h"
#include "MVVM/ViewModels/SequencerEditorViewModel.h"

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
#include "Styles/EposTracksEditorStyle.h"
#include "Tools/EposSequenceTools.h"
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
        if( innerSequence && innerSequence->GetMovieScene() )
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
    // See comment in the FCinematicBoardSection constructor why iSequencer may be nullptr
    //check( iSequencer );

    mPaddedViewRange = iSequencer ? iSequencer->GetViewRange() : TRange<double>( 0.f, 0.f );

    mTimeSliderGeometry = iSequencer ? iSequencer->GetTopTimeSliderWidget()->GetTickSpaceGeometry() : FGeometry();
    //TODO: or maybe: (?)
    // - create and store a FTimeToPixel here and so update it in each tick
    // - set the FTimeToPixel of FCinematicBoardSection with this cached one in the ticks if changed
    // - use it in ConstructConverterForViewRange

    //---

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
}

bool
FCinematicBoardSection::FViewCachedState::operator!=( const FViewCachedState& iRHS ) const
{
    bool is_different = false;

    if( mTimeSliderGeometry != iRHS.mTimeSliderGeometry )
    {
        is_different |= true;
    }

    if( mPaddedViewRange != iRHS.mPaddedViewRange )
    {
        is_different |= true;

        const double RangeSize = mPaddedViewRange.Size<double>();
        const double OtherRangeSize = iRHS.mPaddedViewRange.Size<double>();

        if( !FMath::IsNearlyEqual( RangeSize, OtherRangeSize, RangeSize * 0.001 ) )
        {
            is_different |= true;
        }
    }

    return is_different;

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
    // nullptr is given here to the cache because
    // when this section interface is created, the sequencer exists BUT NOT its corresponding widget
    // (see Sequencer.cpp#510 (creation of the model so this FCinematicBoardSection creation) and only then see Sequencer.cpp#530 (creation of the sequencer widget))
    // so the widget in GetTopTimeSliderWidget() of the sequencer is not valid
    // the cache is naturally updated in the ticks
    , mViewCacheState( iSection, nullptr )
{
    AdditionalDrawEffect = ESlateDrawEffect::NoGamma;

    iSection.SetWidgetHeight( MakeAttributeLambda( [this]() -> float
                                                   {
                                                       if( !mWidgetSectionContent.IsValid() )
                                                           return 0.f;

                                                       return mWidgetSectionContent->GetDesiredSize().Y; // May be 0.f
                                                   } ) );
    auto SequenceChanged = [this]( UMovieSceneSequence* iSequence )
    {
        mNeedRebuild = true;
    };
    iSection.OnSequenceChanged().BindLambda( SequenceChanged );

    // 5.1: BuildKeys() can't be called now, as it relies on ConstructConverterForViewRange(), which calls GetSequencer()->SequencerWidget (inside GetTopTimeSliderWidget())
    // which is not set when opening a board asset
    // When the sequencer is created and initialized, the FCinematicBoardSection is created at line FSequencer::InitSequencer#420: ViewModel->SetSequence(InitParams.RootSequence);
    // But the SequencerWidget is created just below at line FSequencer::InitSequencer#439
    // And as GetSequencer()->GetSequencerWidget() returns a TSharedRef<>, there is no way to know if the SequencerWidget is valid or not
    // So let's try with mNeedRebuild if it's ok now (5.1)
    //BuildKeys();
    mNeedRebuild = true;
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
    FText range_text = TSubSectionMixin<FKeyThumbnailSection>::GetSectionToolTip();

    const UMovieSceneCinematicBoardSection& SectionObject = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();
    //const UMovieScene* MovieScene = SectionObject.GetTypedOuter<UMovieScene>();
    const UMovieSceneSequence* InnerSequence = SectionObject.GetSequence();
    //const UMovieScene* InnerMovieScene = InnerSequence ? InnerSequence->GetMovieScene() : nullptr;

    //---

    TArray<FText> name_elements_texts;

    {
        const UBoardSequence* sequence = Cast<UBoardSequence>( InnerSequence );
        if( sequence )
        {
            name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.Studio", "Studio: {0} | {1}" ), FText::FromString( sequence->NameElements.StudioName ), FText::FromString( sequence->NameElements.StudioAcronym ) ) );
            if( !sequence->NameElements.LicenseName.IsEmpty() )
                name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.License", "License: {0} | {1}" ), FText::FromString( sequence->NameElements.LicenseName ), FText::FromString( sequence->NameElements.LicenseAcronym ) ) );
            name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.Production", "Production: {0} | {1}" ), FText::FromString( sequence->NameElements.ProductionName ), FText::FromString( sequence->NameElements.ProductionAcronym ) ) );
            if( sequence->NameElements.IsSerie )
            {
                name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.Season", "Season: {0}" ), sequence->NameElements.Season ) );
                name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.Episode", "Episode: {0}" ), sequence->NameElements.Episode ) );
            }
            if( !sequence->NameElements.Part.IsEmpty() )
                name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.Part", "Part: {0}" ), FText::FromString( sequence->NameElements.Part ) ) );
            //if( !sequence->NameElements.DepartmentName.IsEmpty() )
            //    name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.Department", "Department: {0} | {1}" ), FText::FromString( sequence->NameElements.DepartmentName ), FText::FromString( sequence->NameElements.DepartmentAcronym ) ) );
            if( !sequence->NameElements.Initials.IsEmpty() )
                name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.Initials", "Initials: {0}" ), FText::FromString( sequence->NameElements.Initials ) ) );

            name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.Index", "Index: {0}" ), sequence->NameElements.Index ) );
        }
    }

    {
        const UShotSequence* sequence = Cast<UShotSequence>( InnerSequence );
        if( sequence )
        {
            name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.Studio", "Studio: {0} | {1}" ), FText::FromString( sequence->NameElements.StudioName ), FText::FromString( sequence->NameElements.StudioAcronym ) ) );
            if( !sequence->NameElements.LicenseName.IsEmpty() )
                name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.License", "License: {0} | {1}" ), FText::FromString( sequence->NameElements.LicenseName ), FText::FromString( sequence->NameElements.LicenseAcronym ) ) );
            name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.Production", "Production: {0} | {1}" ), FText::FromString( sequence->NameElements.ProductionName ), FText::FromString( sequence->NameElements.ProductionAcronym ) ) );
            if( sequence->NameElements.IsSerie )
            {
                name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.Season", "Season: {0}" ), sequence->NameElements.Season ) );
                name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.Episode", "Episode: {0}" ), sequence->NameElements.Episode ) );
            }
            if( !sequence->NameElements.Part.IsEmpty() )
                name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.Part", "Part: {0}" ), FText::FromString( sequence->NameElements.Part ) ) );
            //if( !sequence->NameElements.DepartmentName.IsEmpty() )
            //    name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.Department", "Department: {0} | {1}" ), FText::FromString( sequence->NameElements.DepartmentName ), FText::FromString( sequence->NameElements.DepartmentAcronym ) ) );
            if( !sequence->NameElements.Initials.IsEmpty() )
                name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.Initials", "Initials: {0}" ), FText::FromString( sequence->NameElements.Initials ) ) );

            name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.Index", "Index: {0}" ), sequence->NameElements.Index ) );
            name_elements_texts.Add( FText::Format( LOCTEXT( "ToolTipContentNameElements.TakeIndex", "TakeIndex: {0}" ), sequence->NameElements.TakeIndex ) );
        }
    }

    //---

    FText name_elements_text = FText::Join( FText::FromString( TEXT( "\n" ) ), name_elements_texts );
    FText tooltip_text = FText::Join( FText::FromString( TEXT( "\n\n" ) ), range_text, name_elements_text );

    return tooltip_text;
}

float
FCinematicBoardSection::GetSectionHeight() const
{
    float height = mLastSectionValidHeight; // Use the last known height, this remove some track height flickering when dragging sections

    UMovieSceneCinematicBoardTrack* track = Section->GetTypedOuter<UMovieSceneCinematicBoardTrack>();
    if( track )
    {
        for( auto section : track->GetAllSections() )
        {
            UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( section );
            int current_height = board_section->GetWidgetHeight();
            height = FMath::Max( height, current_height );
        }
    }

    mLastSectionValidHeight = FMath::Max( 100.f, height ); // Arbitrary value which should only be used for one (or some) tick(s) waiting the creation of the layout widget in the section

    return mLastSectionValidHeight;
}

FMargin
FCinematicBoardSection::GetContentPadding() const
{
    // When the rename widget is visible, use less padding so that the widget is visible over the film border (when thumbnails are not shown)
    return FMargin( 8.f, GetRenameVisibility() == EVisibility::Visible ? 10.f : 15.f );
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

    FVector2f local_size = geometry.GetLocalSize();
    local_size = local_size.X > 0 ? local_size : FVector2f( 100.f, 20.f );

    if( oGeometry )
    {
        check( local_size.X > 0 );
        *oGeometry = geometry;
    }

    TSharedPtr<UE::Sequencer::FSequencerEditorViewModel> editor_model = GetSequencer()->GetViewModel();
    TSharedPtr<UE::Sequencer::FTrackAreaViewModel> track_model = editor_model->GetTrackArea();
    return track_model->GetTimeToPixel( local_size.X );
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

    RebuildChannelProxies();
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
FCinematicBoardSection::RebuildChannelProxies()
{
    BuildCameraTransformChannelProxy();
    BuildPlanesTransformChannelProxy();
    BuildPlanesMaterialChannelProxy();
    BuildPlanesOpacityChannelProxy();
}

void
FCinematicBoardSection::RebuildMetaChannels()
{
    ReBuildCameraTransformMetaChannel();
    ReBuildPlanesTransformMetaChannel();
    ReBuildPlanesMaterialMetaChannel();
    ReBuildPlanesOpacityMetaChannel();
}

void
FCinematicBoardSection::BuildCameraTransformChannelProxy()
{
    check( TimeSpace == ETimeSpace::Global ); // Otherwise, TimeSpace must be add as a parameter

    mCameraTransformChannelProxies = BoardSequenceHelpers::BuildCameraTransformChannelProxy( *GetSequencer(), GetSubSectionObject(), GetSequencer()->GetFocusedTemplateID() );

    ReBuildCameraTransformMetaChannel();
}

FChannelProxyBySectionMap
FCinematicBoardSection::GetCameraTransformChannelProxy() const
{
    return mCameraTransformChannelProxies;
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

    mCameraTransformMetaChannel = MakeShared<FMetaChannel>( inner_tolerance );
    mCameraTransformMetaChannel->Build( mCameraTransformChannelProxies );
}

TSharedPtr<FMetaChannel>
FCinematicBoardSection::GetCameraTransformMetaChannel() const
{
    return mCameraTransformMetaChannel;
}

//-

void
FCinematicBoardSection::BuildPlanesTransformChannelProxy()
{
    mPlanesTransformChannelProxies = BoardSequenceHelpers::BuildPlanesTransformChannelProxy( *GetSequencer(), GetSubSectionObject(), GetSequencer()->GetFocusedTemplateID() );

    ReBuildPlanesTransformMetaChannel();
}

FChannelProxyBySectionMap
FCinematicBoardSection::GetPlaneTransformChannelProxy( FMovieScenePossessable iPossessable ) const
{
    if( !mPlanesTransformChannelProxies.Contains( iPossessable.GetGuid() ) )
        return FChannelProxyBySectionMap();

    return mPlanesTransformChannelProxies[iPossessable.GetGuid()];
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

    mPlanesTransformMetaChannel.Empty();
    for( const auto& pair : mPlanesTransformChannelProxies )
    {
        FGuid guid = pair.Key;
        FChannelProxyBySectionMap map = pair.Value;

        TSharedPtr<FMetaChannel> meta_channel = MakeShared<FMetaChannel>( inner_tolerance );
        meta_channel->Build( map );

        mPlanesTransformMetaChannel.Add( guid, meta_channel );
    }
}

TSharedPtr<FMetaChannel>
FCinematicBoardSection::GetPlaneTransformMetaChannel( FMovieScenePossessable iPossessable ) const
{
    if( !mPlanesTransformMetaChannel.Contains( iPossessable.GetGuid() ) )
        return nullptr;

    return mPlanesTransformMetaChannel[iPossessable.GetGuid()];
}

//-

void
FCinematicBoardSection::BuildPlanesMaterialChannelProxy()
{
    mPlanesMaterialChannelProxies = BoardSequenceHelpers::BuildPlanesMaterialChannelProxy( *GetSequencer(), GetSubSectionObject(), GetSequencer()->GetFocusedTemplateID() );

    ReBuildPlanesMaterialMetaChannel();
}

FChannelProxyBySectionMap
FCinematicBoardSection::GetPlaneMaterialChannelProxy( FMovieScenePossessable iPossessable ) const
{
    if( !mPlanesMaterialChannelProxies.Contains( iPossessable.GetGuid() ) )
        return FChannelProxyBySectionMap();

    return mPlanesMaterialChannelProxies[iPossessable.GetGuid()];
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

    mPlanesMaterialMetaChannel.Empty();
    for( const auto& pair : mPlanesMaterialChannelProxies )
    {
        FGuid guid = pair.Key;
        FChannelProxyBySectionMap map = pair.Value;

        TSharedPtr<FMetaChannel> meta_channel = MakeShared<FMetaChannel>( inner_tolerance );
        meta_channel->Build( map );

        mPlanesMaterialMetaChannel.Add( guid, meta_channel );
    }
}

TSharedPtr<FMetaChannel>
FCinematicBoardSection::GetPlaneMaterialMetaChannel( FMovieScenePossessable iPossessable ) const
{
    if( !mPlanesMaterialMetaChannel.Contains( iPossessable.GetGuid() ) )
        return nullptr;

    return mPlanesMaterialMetaChannel[iPossessable.GetGuid()];
}

//-

void
FCinematicBoardSection::BuildPlanesOpacityChannelProxy()
{
    mPlanesOpacityChannelProxies = BoardSequenceHelpers::BuildPlanesOpacityChannelProxy( *GetSequencer(), GetSubSectionObject(), GetSequencer()->GetFocusedTemplateID() );

    ReBuildPlanesOpacityMetaChannel();
}

FChannelProxyBySectionMap
FCinematicBoardSection::GetPlaneOpacityChannelProxy( FMovieScenePossessable iPossessable ) const
{
    if( !mPlanesOpacityChannelProxies.Contains( iPossessable.GetGuid() ) )
        return FChannelProxyBySectionMap();

    return mPlanesOpacityChannelProxies[iPossessable.GetGuid()];
}

void
FCinematicBoardSection::ReBuildPlanesOpacityMetaChannel()
{
    FTimeToPixel converter( ConstructConverterForViewRange() );

    const FFrameTime HalfKeySizeFrames = converter.PixelDeltaToFrame( SequencerSectionConstants::KeySize.X * .5f );
    const FMovieSceneSequenceTransform OuterToInnerTransform = GetSubSectionObject().OuterToInnerTransform();
    FFrameTime clicked_frame = 0; // As if we are on frame 0
    TRange<FFrameNumber> inner_range_tolerance( ( ( clicked_frame - HalfKeySizeFrames ) * OuterToInnerTransform ).FloorToFrame(), ( ( clicked_frame + HalfKeySizeFrames ) * OuterToInnerTransform ).CeilToFrame() );
    FFrameNumber inner_tolerance = inner_range_tolerance.Size<FFrameNumber>() / 2;

    mPlanesOpacityMetaChannel.Empty();
    for( const auto& pair : mPlanesOpacityChannelProxies )
    {
        FGuid guid = pair.Key;
        FChannelProxyBySectionMap map = pair.Value;

        TSharedPtr<FMetaChannel> meta_channel = MakeShared<FMetaChannel>( inner_tolerance );
        meta_channel->Build( map );

        mPlanesOpacityMetaChannel.Add( guid, meta_channel );
    }
}

TSharedPtr<FMetaChannel>
FCinematicBoardSection::GetPlaneOpacityMetaChannel( FMovieScenePossessable iPossessable ) const
{
    if( !mPlanesOpacityMetaChannel.Contains( iPossessable.GetGuid() ) )
        return nullptr;

    return mPlanesOpacityMetaChannel[iPossessable.GetGuid()];
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
        RebuildMetaChannels();
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

    if( mNeedRebuild )
    {
        mNeedRebuild = false;
        RebuildChannelProxies();
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
        ioPainter.LayerId = ioPainter.PaintSectionBackground( sectionObject.GetBackgroundColor() == FLinearColor::Transparent ? settings->BoardTrackSettings.BoardSectionColor : sectionObject.GetBackgroundColor() );
    else if( Cast<UShotSequence>( sectionObject.GetSequence() ) )
        ioPainter.LayerId = ioPainter.PaintSectionBackground( sectionObject.GetBackgroundColor() == FLinearColor::Transparent ? settings->BoardTrackSettings.ShotSectionColor : sectionObject.GetBackgroundColor() );
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

    ioMenuBuilder.BeginSection( NAME_None, LOCTEXT( "BoardMenuText", "Section" ) );
    {
        auto GetColor = [this]()
        {
                                                                    // const_cast is because in the lambda, the compiler doesn't know which this->GetSectionObjectAs() method (const or not) should be used
            const UMovieSceneCinematicBoardSection& sectionObject = const_cast<FCinematicBoardSection*>( this )->GetSectionObjectAs<UMovieSceneCinematicBoardSection>();
            const UEposTracksEditorSettings* settings = GetDefault<UEposTracksEditorSettings>();
            return sectionObject.GetBackgroundColor() == FLinearColor::Transparent ? settings->BoardTrackSettings.BoardSectionColor : sectionObject.GetBackgroundColor();
        };

        auto SetColor = [this]( FLinearColor iNewColor )
        {
            TArray<UMovieSceneSection*> selected_sections;
            GetSequencer()->GetSelectedSections( selected_sections );

            for( auto section : selected_sections )
            {
                UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( section );
                board_section->SetBackgroundColor( iNewColor );
            }
        };

        auto OnGetMenuContent = [=]() -> TSharedRef<SWidget>
        {
            // Open a color picker
            return SNew( SColorPicker )
                .TargetColorAttribute_Lambda( GetColor )
                .UseAlpha( true )
                .DisplayInlineVersion( true )
                .OnColorCommitted_Lambda( SetColor );
        };

        ioMenuBuilder.AddWidget(
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            [
                SNew( SSpacer )
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew( SComboButton )
                .ContentPadding( 0 )
                .HasDownArrow( false )
                .ButtonStyle( FAppStyle::Get(), "Sequencer.AnimationOutliner.ColorStrip" )
                .OnGetMenuContent_Lambda( OnGetMenuContent )
                .CollapseMenuOnParentFocus( true )
                .ToolTipText( LOCTEXT( "SectionBackgroundColorTooltip", "Change the background color of this section\n(set to 0 to use the default (settings) one)" ) )
                .ButtonContent()
                [
                    SNew( SColorBlock )
                    .Color_Lambda( GetColor )
                    .ShowBackgroundForAlpha( true )
                    .Size( FVector2D( 50.0f, 16.0f ) )
                ]
            ],
            LOCTEXT( "SectionBackgroundColor", "Background Color" ) );

        //---

        //ioMenuBuilder.AddMenuEntry(
        //    LOCTEXT( "DuplicateBoard", "Duplicate Board" ),
        //    FText::Format( LOCTEXT( "DuplicateBoardTooltip", "Duplicate {0} to create a new board" ), FText::FromString( sectionObject.GetBoardDisplayName() ) ),
        //    FSlateIcon(),
        //    FUIAction( FExecuteAction::CreateSP( mCinematicBoardTrackEditor.Pin().ToSharedRef(), &FCinematicBoardTrackEditor::DuplicateBoard, &sectionObject ),
        //               FCanExecuteAction::CreateLambda( []() { return false; } ) )
        //);

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

        auto SubMenuAdvanced = [this]( FMenuBuilder& ioMenuBuilder )
        {
            auto BulkEditSubSequence = [this]()
            {
                ISequencer* sequencer = mCinematicBoardTrackEditor.Pin()->GetSequencer().Get();

                TArray<UMovieSceneSection*> sections;
                sequencer->GetSelectedSections( sections );
                TArray<UObject*> objects;
                for( auto section : sections )
                {
                    UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );
                    if( subsection && subsection->GetSequence() )
                        objects.Add( subsection->GetSequence() );
                }

                // PropertyEditorModule.CreatePropertyEditorToolkit seems to dislike empty array ...
                if( !objects.Num() )
                    return;

                FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
                PropertyEditorModule.CreatePropertyEditorToolkit( TSharedPtr<IToolkitHost>(), objects );
            };

            auto CanBulkEditSubSequence = [this]()
            {
                ISequencer* sequencer = mCinematicBoardTrackEditor.Pin()->GetSequencer().Get();

                TArray<UMovieSceneSection*> sections;
                sequencer->GetSelectedSections( sections );
                TArray<UObject*> objects;
                for( auto section : sections )
                {
                    UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );
                    if( subsection && subsection->GetSequence() )
                        objects.Add( subsection->GetSequence() );
                }

                return !!objects.Num();
            };

            // As there is only a FNamingElements editable structure inside the subsequence class (aka UPROPERTY)
            // for now, just name the option "Edit Naming Elements"
            ioMenuBuilder.AddMenuEntry(
                LOCTEXT( "BulkEditNamingElements", "Edit Naming Elements" ),
                LOCTEXT( "BulkEditNamingElementsTooltip", "Edit naming elements of the selected sections" ),
                FSlateIcon(),
                FUIAction(
                    FExecuteAction::CreateLambda( BulkEditSubSequence ),
                    FCanExecuteAction::CreateLambda( CanBulkEditSubSequence )
                )
            );
        };

        ioMenuBuilder.AddSubMenu(
            LOCTEXT( "AdvancedSectionOptions", "Advanced" ),
            FText::GetEmpty(),
            FNewMenuDelegate::CreateLambda( SubMenuAdvanced )
        );
    }
    ioMenuBuilder.EndSection();

    //---

    if( Cast<UShotSequence>( sectionObject.GetSequence() ) )
    {
        ioMenuBuilder.BeginSection( NAME_None, LOCTEXT( "TakeMenuText", "Take" ) );
        {
            ioMenuBuilder.AddSubMenu(
                LOCTEXT( "TakesMenu", "Takes" ),
                LOCTEXT( "TakesMenuTooltip", "Shot takes" ),
                FNewMenuDelegate::CreateSP( this, &FCinematicBoardSection::AddTakesMenu ),
                FUIAction(
                    FExecuteAction(),
                    FCanExecuteAction::CreateLambda( [this, &sectionObject]() { return !BoardSequenceTools::IsDrawingInEditionMode( GetSequencer().Get(), sectionObject ); } )
                ),
                NAME_None,
                EUserInterfaceActionType::Button );

            FText take_tooltip = FText::Format( LOCTEXT( "NewTakeTooltip", "Create a new take for {0}" ), FText::FromString( sectionObject.GetBoardDisplayName() ) );
            if( BoardSequenceTools::IsDrawingInEditionMode( GetSequencer().Get(), sectionObject ) )
                take_tooltip = FText::Format( LOCTEXT( "NewTakeWithWarningTooltip", "Create a new take for {0}\n\nDrawing(s) must not be in edition mode" ), FText::FromString( sectionObject.GetBoardDisplayName() ) );

            ioMenuBuilder.AddMenuEntry(
                LOCTEXT( "NewTake", "New Take" ),
                take_tooltip,
                FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "Take" ),
                FUIAction(
                    FExecuteAction::CreateLambda( [this, &sectionObject]() { BoardSequenceTools::CreateTake( GetSequencer().Get(), sectionObject ); } ),
                    FCanExecuteAction::CreateLambda( [this, &sectionObject]() { return !BoardSequenceTools::IsDrawingInEditionMode( GetSequencer().Get(), sectionObject ); } )
                )
            );
        }
        ioMenuBuilder.EndSection();
    }
}

void
FCinematicBoardSection::AddTakesMenu( FMenuBuilder& MenuBuilder )
{
    UMovieSceneCinematicBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();

    for( auto take : sectionObject.GetTakes() )
    {
        const UMovieSceneSequence* take_sequence = take.GetSequence();
        if( !take_sequence )
            continue;

        MenuBuilder.AddMenuEntry(
            take_sequence->GetDisplayName(),
            FText::Format( LOCTEXT( "TakeNumberTooltip", "Switch to {0}" ), FText::FromString( take_sequence->GetPathName() ) ),
            take_sequence->GetPathName() == sectionObject.GetSequence()->GetPathName() ? FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Sequencer.Star" ) : FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Sequencer.Empty" ),
            FUIAction(
                FExecuteAction::CreateLambda( [this, &sectionObject, take]() { BoardSequenceTools::SwitchTake( GetSequencer().Get(), sectionObject, sectionObject.FindTake( take ) ); } ),
                FCanExecuteAction::CreateLambda( [this, &sectionObject]() { return !BoardSequenceTools::IsDrawingInEditionMode( GetSequencer().Get(), sectionObject ); } )
            )
        );
    }
}

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
