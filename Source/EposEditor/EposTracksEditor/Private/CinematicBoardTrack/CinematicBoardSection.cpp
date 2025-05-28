// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
#include "PropertyEditorModule.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Widgets/Layout/SSpacer.h"
#include "AnimatedRange.h"
#include "ITimeSlider.h"
#include "MVVM/ViewModels/SequencerEditorViewModel.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
#include "ImageCoreUtils.h"

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
#include "OdysseyAnimation.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationTimelineSection.h"

#define LOCTEXT_NAMESPACE "FCinematicBoardSection"


/* FCinematicBoardSection structors
 *****************************************************************************/

FCinematicBoardSection::FCinematicSectionCacheForThumbnail::FCinematicSectionCacheForThumbnail( UMovieSceneCinematicBoardSection* iSection )
    : mInnerFrameRate( 1, 1 )
    , mInnerFrameOffset( 0 )
    , mSectionStartFrame( 0 )
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
    // Only call the parent function to only gather camera transform keys (as everything is set because no link to some widgets like BuildKeys() which needs the sequencer widget to compute channel proxies
    FKeyThumbnailSection::BuildKeys();
}


FCinematicBoardSection::~FCinematicBoardSection()
{
    // Doesn't work, because when the gui is rebuild:
    // - first, create the new FCinematicBoardSections
    // - then, delete the old ones
    // In this order, the next line will reset the attribute of the UMovieSceneCinematicBoardSection after it has been set in the constructor for the new ones
    //Cast<UMovieSceneCinematicBoardSection>( Section )->SetWidgetHeight( 0 );
}

//---

void
FCinematicBoardSection::AddReferencedObjects( FReferenceCollector& Collector ) //override
{
    for( const auto& pair1 : mAnimationsTimelineMetaChannel )
    {
        FGuid guid = pair1.Key;

        if( !mAnimationsTimelineThumbnails.Contains( guid ) )
            continue;

        for( FThumbnailData& thumbnail_data : mAnimationsTimelineThumbnails[guid] )
        {
            //Collector.AddReferencedObject( thumbnail_data.RenderTarget );
            Collector.AddReferencedObject( thumbnail_data.Texture );
        }
    }
}

FString
FCinematicBoardSection::GetReferencerName() const //override
{
    return "FCinematicBoardSection";
}

//---

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
FCinematicBoardSection::GetSectionHeight( const UE::Sequencer::FViewDensityInfo& ViewDensity ) const
{
    if( !IsValid( Section ) )
        return 100.f;

    UMovieSceneCinematicBoardTrack* track = Section->GetTypedOuter<UMovieSceneCinematicBoardTrack>();
    check( track );

    int max_height = 0;
    for( auto section : track->GetAllSections() )
    {
        UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( section );
        int current_height = board_section->GetWidgetHeight();
        max_height = FMath::Max( max_height, current_height );
    }

    int new_height = FMath::Max( 100, max_height );
    track->SetRowHeight( new_height ); // Arbitrary value which should only be used for one (or some) tick(s) waiting the creation of the layout widget in the section

    return track->GetRowHeight();
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
    const FMovieSceneSequenceHierarchy*     Hierarchy = sequencer->GetSharedPlaybackState()->GetHierarchy();

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

    TArray<FFrameTime> default_keys;
    TArray<FFrameTime> keys_as_frame = BoardSequenceHelpers::GetCameraTransformTimesRecursive( GetSubSectionObject(), default_keys );
    keys_as_frame.Append( default_keys );
    keys_as_frame.Sort();
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
    BuildAnimationsTransformChannelProxy();
    BuildAnimationsTimelineChannelProxy();
    BuildAnimationsOpacityChannelProxy();
}

void
FCinematicBoardSection::RebuildMetaChannels()
{
    ReBuildCameraTransformMetaChannel();
    ReBuildAnimationsTransformMetaChannel();
    ReBuildAnimationsTimelineMetaChannel();
    ReBuildAnimationsOpacityMetaChannel();
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
FCinematicBoardSection::BuildAnimationsTransformChannelProxy()
{
    mAnimationsTransformChannelProxies = BoardSequenceHelpers::BuildAnimationsTransformChannelProxy( *GetSequencer(), GetSubSectionObject(), GetSequencer()->GetFocusedTemplateID() );

    ReBuildAnimationsTransformMetaChannel();
}

FChannelProxyBySectionMap
FCinematicBoardSection::GetAnimationTransformChannelProxy( FMovieScenePossessable iPossessable ) const
{
    if( !mAnimationsTransformChannelProxies.Contains( iPossessable.GetGuid() ) )
        return FChannelProxyBySectionMap();

    return mAnimationsTransformChannelProxies[iPossessable.GetGuid()];
}

void
FCinematicBoardSection::ReBuildAnimationsTransformMetaChannel()
{
    FTimeToPixel converter( ConstructConverterForViewRange() );

    const FFrameTime HalfKeySizeFrames = converter.PixelDeltaToFrame( SequencerSectionConstants::KeySize.X * .5f );
    const FMovieSceneSequenceTransform OuterToInnerTransform = GetSubSectionObject().OuterToInnerTransform();
    FFrameTime clicked_frame = 0; // As if we are on frame 0
    TRange<FFrameNumber> inner_range_tolerance( ( ( clicked_frame - HalfKeySizeFrames ) * OuterToInnerTransform ).FloorToFrame(), ( ( clicked_frame + HalfKeySizeFrames ) * OuterToInnerTransform ).CeilToFrame() );
    FFrameNumber inner_tolerance = inner_range_tolerance.Size<FFrameNumber>() / 2;

    mAnimationsTransformMetaChannel.Empty();
    for( const auto& pair : mAnimationsTransformChannelProxies )
    {
        FGuid guid = pair.Key;
        FChannelProxyBySectionMap map = pair.Value;

        TSharedPtr<FMetaChannel> meta_channel = MakeShared<FMetaChannel>( inner_tolerance );
        meta_channel->Build( map );

        mAnimationsTransformMetaChannel.Add( guid, meta_channel );
    }
}

TSharedPtr<FMetaChannel>
FCinematicBoardSection::GetAnimationTransformMetaChannel( FMovieScenePossessable iPossessable ) const
{
    if( !mAnimationsTransformMetaChannel.Contains( iPossessable.GetGuid() ) )
        return nullptr;

    return mAnimationsTransformMetaChannel[iPossessable.GetGuid()];
}

//-

void
FCinematicBoardSection::BuildAnimationsTimelineChannelProxy()
{
    mAnimationsTimelineChannelProxies = BoardSequenceHelpers::BuildAnimationsTimelineChannelProxy( *GetSequencer(), GetSubSectionObject(), GetSequencer()->GetFocusedTemplateID() );

    ReBuildAnimationsTimelineMetaChannel();
}

FChannelProxyBySectionMap
FCinematicBoardSection::GetAnimationTimelineChannelProxy( FMovieScenePossessable iPossessable ) const
{
    if( !mAnimationsTimelineChannelProxies.Contains( iPossessable.GetGuid() ) )
        return FChannelProxyBySectionMap();

    return mAnimationsTimelineChannelProxies[iPossessable.GetGuid()];
}

void
FCinematicBoardSection::ReBuildAnimationsTimelineMetaChannel()
{
    FTimeToPixel converter( ConstructConverterForViewRange() );

    const FFrameTime HalfKeySizeFrames = converter.PixelDeltaToFrame( SequencerSectionConstants::KeySize.X * .5f );
    const FMovieSceneSequenceTransform OuterToInnerTransform = GetSubSectionObject().OuterToInnerTransform();
    FFrameTime clicked_frame = 0; // As if we are on frame 0
    TRange<FFrameNumber> inner_range_tolerance( ( ( clicked_frame - HalfKeySizeFrames ) * OuterToInnerTransform ).FloorToFrame(), ( ( clicked_frame + HalfKeySizeFrames ) * OuterToInnerTransform ).CeilToFrame() );
    FFrameNumber inner_tolerance = inner_range_tolerance.Size<FFrameNumber>() / 2;

    mAnimationsTimelineMetaChannel.Empty();
    for( const auto& pair : mAnimationsTimelineChannelProxies )
    {
        FGuid guid = pair.Key;
        FChannelProxyBySectionMap map = pair.Value;

        TSharedPtr<FMetaChannel> meta_channel = MakeShared<FMetaChannel>( inner_tolerance );
        meta_channel->Build( map );

        mAnimationsTimelineMetaChannel.Add( guid, meta_channel );
    }

    mAnimationsTimelineThumbnails.Empty();
    for( const auto& pair1 : mAnimationsTimelineMetaChannel )
    {
        FGuid guid = pair1.Key;

        ReBuildAnimationsTimelineThumbnails( guid );
    }
}

TSharedPtr<FMetaChannel>
FCinematicBoardSection::GetAnimationTimelineMetaChannel( FMovieScenePossessable iPossessable ) const
{
    if( !mAnimationsTimelineMetaChannel.Contains( iPossessable.GetGuid() ) )
        return nullptr;

    return mAnimationsTimelineMetaChannel[iPossessable.GetGuid()];
}

const TArray<FCinematicBoardSection::FThumbnailData>&
FCinematicBoardSection::GetAnimationTimelineThumbnails( FMovieScenePossessable iPossessable ) const
{
    if( !mAnimationsTimelineThumbnails.Contains( iPossessable.GetGuid() ) )
    {
        static TArray<FThumbnailData> sEmpty;
        return sEmpty;
    }

    return mAnimationsTimelineThumbnails[iPossessable.GetGuid()];
}

FCinematicBoardSection::FThumbnailData
FCinematicBoardSection::RebuildAnimationThumbnailDataInternal( UOdysseyAnimationTimelineSection* iSection, FFrameNumber iFrameInSequence, TOptional<FGuid> iFrameId )
{
    auto CreateRenderTargetAndImage2 = []( int32 InSizeX, int32 InSizeY, ETextureRenderTargetFormat InFormat, UTextureRenderTarget2D*& oRenderTarget ) -> void
        {
            static UTextureRenderTarget2D* renderTarget = nullptr;
            if( !renderTarget
                || !IsValid( renderTarget )
                || renderTarget->HasAnyFlags( RF_BeginDestroyed | RF_FinishDestroyed ) // IsValid() seems to not be enough. After an auto-save, the uobject is not RF_MirroredGarbage but RF_BeginDestroyed | RF_FinishDestroyed, so check them.
                || renderTarget->SizeX != InSizeX
                || renderTarget->SizeY != InSizeY
                || renderTarget->RenderTargetFormat != InFormat )
            {
                renderTarget = NewObject<UTextureRenderTarget2D>( GetTransientPackage(), NAME_None, RF_Public | RF_Transient );
                renderTarget->RenderTargetFormat = InFormat;
                //renderTarget->ClearColor = FLinearColor( frame_in_timeline.Value / float( 50 ), frame_in_timeline.Value / float( 50 ), frame_in_timeline.Value / float( 50 ) );
                renderTarget->ResizeTarget( InSizeX, InSizeY );
                //renderTarget->InitAutoFormat(
                renderTarget->UpdateResource();
            }

            oRenderTarget = renderTarget;
        };

    //---

    const UMovieSceneSubSection* subsection_object = &GetSubSectionObject();

    FMovieSceneInverseSequenceTransform inner_to_outer_transform = subsection_object->OuterToInnerTransform().Inverse();
    const UMovieScene* movie_scene = subsection_object->GetTypedOuter<UMovieScene>();
    check( movie_scene );

    //---

    FFrameNumber frame_in_timeline = iSection->ConvertFrameFromSequenceToTimeline( iFrameInSequence );

    TOptional<FFrameTime> outer_time = inner_to_outer_transform.TryTransformTime( iFrameInSequence );
    if( !outer_time )
        return FThumbnailData();

    //---

    UOdysseyAnimation* animation = iSection->GetAnimation();

    float ratio = animation->GetWidth() / float( animation->GetHeight() );
    const FIntVector2 thumbnail_size( 200 * ratio, 200 );

    UTexture2D* texture = nullptr;
    UTextureRenderTarget2D* renderTarget = nullptr;

    FRenderingComposition rendering_composition = animation->GetRenderingComposition( EOdysseyRenderingType::Render, frame_in_timeline.Value ); // THIS DOESN'T MANAGE IMAGE CHANGES !!!

    if( !mAnimationsTimelineThumbnailPool.Contains( rendering_composition ) || ( iFrameId.IsSet() && rendering_composition.Contains( *iFrameId ) ) )
    {
        CreateRenderTargetAndImage2( thumbnail_size.X, thumbnail_size.Y, RTF_RGBA16f, renderTarget );

        animation->RenderAndResize_GameThread( renderTarget, frame_in_timeline.Value, EOdysseyRenderingType::Render );

        texture = renderTarget->ConstructTexture2D( GetTransientPackage(), FString::Printf( TEXT( "textureThumbnail-%s" ), *FGuid::NewGuid().ToString() ), RF_Public | RF_Transient );

        //mAnimationsTimelineThumbnailPool.Add( rendering_composition, FPoolData{ renderTarget } );
        mAnimationsTimelineThumbnailPool.Add( rendering_composition, FPoolData{ texture } );
    }
    else
    {
        FPoolData p = mAnimationsTimelineThumbnailPool.FindChecked( rendering_composition );
        //renderTarget = p.RenderTarget;
        texture = p.Texture;
    }

    //// Fence now to ensure the update is processed on the GPU by the end of this function
    //FRenderCommandFence fence;
    //fence.BeginFence();
    //fence.Wait();

    //-

    //FSlateBrush* brush = new FSlateBrush();
    //FSlateImageBrush* brush = new FSlateImageBrush( renderTarget, FVector2f( renderTarget->SizeX, renderTarget->SizeY ) ); // There are problems when using a render target (the brush seems to not update its resource once Render_GameThread())
    FSlateImageBrush* brush = new FSlateImageBrush( texture, FVector2f( texture->GetSizeX(), texture->GetSizeY() ) );

    //brush->SetResourceObject( renderTarget ); // There are problems when using a render target (the brush seems to not update its resource once Render_GameThread())
    //brush->SetResourceObject( texture ); // There are problems when using a render target (the brush seems to not update its resource once Render_GameThread())

    //---

    FThumbnailData thumbnail;
    thumbnail.QTime = FQualifiedFrameTime( *outer_time, movie_scene->GetTickResolution() );
    //thumbnail.RenderTarget = renderTarget;
    thumbnail.Texture = texture;
    thumbnail.Brush = brush;
    thumbnail.Size = thumbnail_size;

    return thumbnail;
}

void
FCinematicBoardSection::ReBuildAnimationsTimelineThumbnails( FMovieScenePossessable iPossessable, TOptional<FGuid> iFrameId )
{
    ReBuildAnimationsTimelineThumbnails( iPossessable.GetGuid(), iFrameId );
}

void
FCinematicBoardSection::ReBuildAnimationsTimelineThumbnails( FGuid iGuid, TOptional<FGuid> iFrameId )
{
    if( mAnimationsTimelineThumbnails.Contains( iGuid ) )
        mAnimationsTimelineThumbnails.Remove( iGuid );

    if( !mAnimationsTimelineMetaChannel.Contains( iGuid ) )
        return;

    TSharedPtr<FMetaChannel> meta_channel = mAnimationsTimelineMetaChannel[iGuid];

    //---

    const UMovieSceneSubSection* subsection_object = &GetSubSectionObject();

    FMovieSceneInverseSequenceTransform inner_to_outer_transform = subsection_object->OuterToInnerTransform().Inverse();
    const UMovieScene* movie_scene = subsection_object->GetTypedOuter<UMovieScene>();
    check( movie_scene );

    TArray<FThumbnailData> thumbnails;

    for( const auto& pair : meta_channel->GetMetaKeys() )
    {
        FFrameNumber meta_frame = pair.Key;
        //FMetaKey meta_key = pair.Value;
        //FKeyDrawParams key_draw_param = meta_key.mMetaKeyDrawParam;

        for( const auto& subkey : pair.Value.mSubKeys )
        {
            UOdysseyAnimationTimelineSection* animation_timeline_section = Cast<UOdysseyAnimationTimelineSection>( subkey.mSection );
            if( !animation_timeline_section )
                continue;

            FThumbnailData thumbnail = RebuildAnimationThumbnailDataInternal( animation_timeline_section, meta_frame, iFrameId );
            //if( !thumbnail.RenderTarget )
            if( !thumbnail.Texture )
                continue;

            thumbnails.Add( thumbnail );

            break; // only the first subkey (of the metakey) is used
        }
    }

    //---

    mAnimationsTimelineThumbnails.Add( iGuid, thumbnails );
}

//-

void
FCinematicBoardSection::BuildAnimationsOpacityChannelProxy()
{
    mAnimationsOpacityChannelProxies = BoardSequenceHelpers::BuildAnimationsOpacityChannelProxy( *GetSequencer(), GetSubSectionObject(), GetSequencer()->GetFocusedTemplateID() );

    ReBuildAnimationsOpacityMetaChannel();
}

FChannelProxyBySectionMap
FCinematicBoardSection::GetAnimationOpacityChannelProxy( FMovieScenePossessable iPossessable ) const
{
    if( !mAnimationsOpacityChannelProxies.Contains( iPossessable.GetGuid() ) )
        return FChannelProxyBySectionMap();

    return mAnimationsOpacityChannelProxies[iPossessable.GetGuid()];
}

void
FCinematicBoardSection::ReBuildAnimationsOpacityMetaChannel()
{
    FTimeToPixel converter( ConstructConverterForViewRange() );

    const FFrameTime HalfKeySizeFrames = converter.PixelDeltaToFrame( SequencerSectionConstants::KeySize.X * .5f );
    const FMovieSceneSequenceTransform OuterToInnerTransform = GetSubSectionObject().OuterToInnerTransform();
    FFrameTime clicked_frame = 0; // As if we are on frame 0
    TRange<FFrameNumber> inner_range_tolerance( ( ( clicked_frame - HalfKeySizeFrames ) * OuterToInnerTransform ).FloorToFrame(), ( ( clicked_frame + HalfKeySizeFrames ) * OuterToInnerTransform ).CeilToFrame() );
    FFrameNumber inner_tolerance = inner_range_tolerance.Size<FFrameNumber>() / 2;

    mAnimationsOpacityMetaChannel.Empty();
    for( const auto& pair : mAnimationsOpacityChannelProxies )
    {
        FGuid guid = pair.Key;
        FChannelProxyBySectionMap map = pair.Value;

        TSharedPtr<FMetaChannel> meta_channel = MakeShared<FMetaChannel>( inner_tolerance );
        meta_channel->Build( map );

        mAnimationsOpacityMetaChannel.Add( guid, meta_channel );
    }
}

TSharedPtr<FMetaChannel>
FCinematicBoardSection::GetAnimationOpacityMetaChannel( FMovieScenePossessable iPossessable ) const
{
    if( !mAnimationsOpacityMetaChannel.Contains( iPossessable.GetGuid() ) )
        return nullptr;

    return mAnimationsOpacityMetaChannel[iPossessable.GetGuid()];
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
                    FCanExecuteAction::CreateLambda( [this, &sectionObject]() { return !BoardSequenceTools::IsAnimationInEditionMode( GetSequencer().Get(), sectionObject ); } )
                ),
                NAME_None,
                EUserInterfaceActionType::Button );

            FText take_tooltip = FText::Format( LOCTEXT( "NewTakeTooltip", "Create a new take for {0}" ), FText::FromString( sectionObject.GetBoardDisplayName() ) );
            if( BoardSequenceTools::IsAnimationInEditionMode( GetSequencer().Get(), sectionObject ) )
                take_tooltip = FText::Format( LOCTEXT( "NewTakeWithWarningTooltip", "Create a new take for {0}\n\nAnimation(s) must not be in edition mode" ), FText::FromString( sectionObject.GetBoardDisplayName() ) );

            ioMenuBuilder.AddMenuEntry(
                LOCTEXT( "NewTake", "New Take" ),
                take_tooltip,
                FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "Take" ),
                FUIAction(
                    FExecuteAction::CreateLambda( [this, &sectionObject]() { BoardSequenceTools::CreateTake( GetSequencer().Get(), sectionObject ); } ),
                    FCanExecuteAction::CreateLambda( [this, &sectionObject]() { return false; } )
                    //FCanExecuteAction::CreateLambda( [this, &sectionObject]() { return !BoardSequenceTools::IsAnimationInEditionMode( GetSequencer().Get(), sectionObject ); } )
                )
            );
        }
        ioMenuBuilder.EndSection();
    }

    //---

    auto MakeUIAction = [this]( EMovieSceneTransformChannel ChannelsToToggle, const TSharedPtr<ISequencer>& Sequencer )
        {

            UMovieSceneSubSection* SubSection = Cast<UMovieSceneSubSection>( Section );
            if( !SubSection )
            {
                return FUIAction();
            }
            return FUIAction(
                FExecuteAction::CreateLambda( [SubSection, ChannelsToToggle, Sequencer]
                                              {
                                                  FScopedTransaction Transaction( LOCTEXT( "SetActiveChannelsTransaction", "Set Active Channels" ) );
                                                  SubSection->Modify();
                                                  EMovieSceneTransformChannel Channels = SubSection->GetMask().GetChannels();

                                                  if( EnumHasAllFlags( Channels, ChannelsToToggle ) || ( Channels & ChannelsToToggle ) == EMovieSceneTransformChannel::None )
                                                  {
                                                      SubSection->SetMask( SubSection->GetMask().GetChannels() ^ ChannelsToToggle );
                                                  }
                                                  else
                                                  {
                                                      SubSection->SetMask( SubSection->GetMask().GetChannels() | ChannelsToToggle );
                                                  }

                                                  Sequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );
                                              }
                ),
                FCanExecuteAction(),
                FGetActionCheckState::CreateLambda( [SubSection, ChannelsToToggle]
                                                    {
                                                        EMovieSceneTransformChannel Channels = SubSection->GetMask().GetChannels();
                                                        if( EnumHasAllFlags( Channels, ChannelsToToggle ) )
                                                        {
                                                            return ECheckBoxState::Checked;
                                                        }
                                                        else if( EnumHasAnyFlags( Channels, ChannelsToToggle ) )
                                                        {
                                                            return ECheckBoxState::Undetermined;
                                                        }
                                                        return ECheckBoxState::Unchecked;
                                                    } )
            );
        };

    TSharedPtr<ISequencer> Sequencer = GetSequencer();

    ioMenuBuilder.BeginSection( NAME_None, LOCTEXT( "OriginChannelsText", "Active Channels" ) );
    ioMenuBuilder.AddSubMenu(
        LOCTEXT( "AllTranslation", "Translation" ), LOCTEXT( "AllTranslation_ToolTip", "Causes this section to affect the translation of the transform" ),
        FNewMenuDelegate::CreateLambda( [Sequencer, MakeUIAction]( FMenuBuilder& SubMenuBuilder )
                                        {
                                            SubMenuBuilder.AddMenuEntry(
                                                LOCTEXT( "TranslationX", "X" ), LOCTEXT( "TranslationX_ToolTip", "Causes this section to affect the X channel of the transform's translation" ),
                                                FSlateIcon(), MakeUIAction( EMovieSceneTransformChannel::TranslationX, Sequencer ), NAME_None, EUserInterfaceActionType::ToggleButton );
                                            SubMenuBuilder.AddMenuEntry(
                                                LOCTEXT( "TranslationY", "Y" ), LOCTEXT( "TranslationY_ToolTip", "Causes this section to affect the Y channel of the transform's translation" ),
                                                FSlateIcon(), MakeUIAction( EMovieSceneTransformChannel::TranslationY, Sequencer ), NAME_None, EUserInterfaceActionType::ToggleButton );
                                            SubMenuBuilder.AddMenuEntry(
                                                LOCTEXT( "TranslationZ", "Z" ), LOCTEXT( "TranslationZ_ToolTip", "Causes this section to affect the Z channel of the transform's translation" ),
                                                FSlateIcon(), MakeUIAction( EMovieSceneTransformChannel::TranslationZ, Sequencer ), NAME_None, EUserInterfaceActionType::ToggleButton );
                                        } ),
        MakeUIAction( EMovieSceneTransformChannel::Translation, Sequencer ),
        NAME_None,
        EUserInterfaceActionType::ToggleButton );

    ioMenuBuilder.AddSubMenu(
        LOCTEXT( "AllRotation", "Rotation" ), LOCTEXT( "AllRotation_ToolTip", "Causes this section to affect the rotation of the transform" ),
        FNewMenuDelegate::CreateLambda( [Sequencer, MakeUIAction]( FMenuBuilder& SubMenuBuilder )
                                        {
                                            SubMenuBuilder.AddMenuEntry(
                                                LOCTEXT( "RotationX", "Roll (X)" ), LOCTEXT( "RotationX_ToolTip", "Causes this section to affect the roll (X) channel the transform's rotation" ),
                                                FSlateIcon(), MakeUIAction( EMovieSceneTransformChannel::RotationX, Sequencer ), NAME_None, EUserInterfaceActionType::ToggleButton );
                                            SubMenuBuilder.AddMenuEntry(
                                                LOCTEXT( "RotationY", "Pitch (Y)" ), LOCTEXT( "RotationY_ToolTip", "Causes this section to affect the pitch (Y) channel the transform's rotation" ),
                                                FSlateIcon(), MakeUIAction( EMovieSceneTransformChannel::RotationY, Sequencer ), NAME_None, EUserInterfaceActionType::ToggleButton );
                                            SubMenuBuilder.AddMenuEntry(
                                                LOCTEXT( "RotationZ", "Yaw (Z)" ), LOCTEXT( "RotationZ_ToolTip", "Causes this section to affect the yaw (Z) channel the transform's rotation" ),
                                                FSlateIcon(), MakeUIAction( EMovieSceneTransformChannel::RotationZ, Sequencer ), NAME_None, EUserInterfaceActionType::ToggleButton );
                                        } ),
        MakeUIAction( EMovieSceneTransformChannel::Rotation, Sequencer ),
        NAME_None,
        EUserInterfaceActionType::ToggleButton );
    ioMenuBuilder.EndSection();
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
                FCanExecuteAction::CreateLambda( [this, &sectionObject]() { return !BoardSequenceTools::IsAnimationInEditionMode( GetSequencer().Get(), sectionObject ); } )
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
