// IDDN FR.001.250001.004.S.X.2019.000.00000
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
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposMovieSceneSequence.h"
#include "Helpers/SectionsHelpersConvert.h"
#include "Helpers/SectionsHelpersResize.h"
#include "Shot/ShotSequence.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutHelpers.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "CinematicBoardWidgets/SCinematicBoardSectionCamera.h"
#include "CinematicBoardWidgets/SCinematicBoardSectionLayout.h"
#include "CinematicBoardWidgets/SCinematicBoardSectionPlanes.h"
#include "CinematicBoardWidgets/SCinematicBoardSectionThumbnails.h"
#include "CinematicBoardWidgets/SCinematicBoardSectionTitle.h"

#define LOCTEXT_NAMESPACE "FCinematicBoardSection"


/* FCinematicBoardSection structors
 *****************************************************************************/

FCinematicBoardSection::FCinematicSectionCache::FCinematicSectionCache( UMovieSceneCinematicBoardSection* iSection )
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
FCinematicBoardSection::FCinematicSectionCache::operator!=( const FCinematicSectionCache& iRHS ) const
{
    return mInnerFrameRate != iRHS.mInnerFrameRate
        || mInnerFrameOffset != iRHS.mInnerFrameOffset
        || mSectionStartFrame != iRHS.mSectionStartFrame
        || mTimeScale != iRHS.mTimeScale;
}

//---

FCinematicBoardSection::FCinematicBoardSection( TSharedPtr<ISequencer> iSequencer, UMovieSceneCinematicBoardSection& iSection, TSharedPtr<FCinematicBoardTrackEditor> iCinematicBoardTrackEditor, TSharedPtr<FTrackEditorThumbnailPool> iThumbnailPool )
    : TSubSectionMixin( iSequencer, iSection, iSequencer, iThumbnailPool, iSection )
    , mCinematicBoardTrackEditor( iCinematicBoardTrackEditor )
    , mThumbnailCacheData( &iSection )
{
    AdditionalDrawEffect = ESlateDrawEffect::NoGamma;
}


FCinematicBoardSection::~FCinematicBoardSection()
{
}

FText
FCinematicBoardSection::GetSectionTitle() const
{
    return FText::GetEmpty(); // Now manage inside Title widget and don't want to be displayed at a position defined by the 'mother' SSequencerSection
    //return GetRenameVisibility() == EVisibility::Visible ? FText::GetEmpty() : HandleThumbnailTextBlockText();
}

float
FCinematicBoardSection::GetSectionHeight() const
{
    if( !mWidgetLayout.IsValid() )
        return 50.f; // Arbitrary value which should only be used for one (or some) tick(s) waiting the creation of the layout widget in the section

    return mWidgetLayout->GetDesiredSize().Y;
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

FCinematicBoardSection::FInnerSequenceResult::FInnerSequenceResult()
    : mInnerSequenceID()
    , mHierarchy( nullptr )
    , mPlayer( nullptr )
    , mNode( nullptr )
    , mSubData( nullptr )
    , mInnerMovieSceneSequence( nullptr )
    , mInnerMovieScene( nullptr )
{
}

FCinematicBoardSection::FInnerSequenceResult::FInnerSequenceResult( const FMovieSceneSequenceID& iID, const FMovieSceneSequenceHierarchy* iHierarchy, IMovieScenePlayer* ioPlayer )
    : mInnerSequenceID( iID )
    , mHierarchy( iHierarchy )
    , mPlayer( ioPlayer )
    , mNode( nullptr )
    , mSubData( nullptr )
    , mInnerMovieSceneSequence( nullptr )
    , mInnerMovieScene( nullptr )
{
}

bool
FCinematicBoardSection::FInnerSequenceResult::IsValid() const
{
    return mInnerSequenceID.IsValid();
}

bool
FCinematicBoardSection::FInnerSequenceResult::IsFilled() const
{
    return !!mInnerMovieScene;
}

//-

FCinematicBoardSection::FInnerSequenceResult
FCinematicBoardSection::GetInnerSequenceID( const UMovieSceneSubSection* iSubSection ) const
{
    TSharedPtr<ISequencer> sequencer = GetSequencer();
    if( !sequencer.IsValid() )
        return FInnerSequenceResult();

    const UMovieSceneSubSection&            sectionObject = iSubSection ? *iSubSection : GetSectionObjectAs<UMovieSceneSubSection>();
    const FMovieSceneSequenceID             thisSequenceID = sequencer->GetFocusedTemplateID();
    const FMovieSceneSequenceID             targetSequenceID = sectionObject.GetSequenceID();
    const FMovieSceneSequenceHierarchy*     hierarchy = sequencer->GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( sequencer->GetEvaluationTemplate().GetCompiledDataID() );

    if( !hierarchy )
        return FInnerSequenceResult();

    const FMovieSceneSequenceHierarchyNode* thisSequenceNode = hierarchy->FindNode( thisSequenceID );

    check( thisSequenceNode );

    // Find the TargetSequenceID by comparing deterministic sequence IDs for all children of the current node
    const FMovieSceneSequenceID* innerSequenceID = Algo::FindByPredicate( thisSequenceNode->Children,
        [hierarchy, targetSequenceID]( FMovieSceneSequenceID iSequenceID )
        {
            const FMovieSceneSubSequenceData* subData = hierarchy->FindSubData( iSequenceID );
            return subData && subData->DeterministicSequenceID == targetSequenceID;
        }
    );

    if( !innerSequenceID )
        return FInnerSequenceResult();

    FInnerSequenceResult result( *innerSequenceID, hierarchy, sequencer.Get() );
    return result;
}

void
FCinematicBoardSection::FillInnerSequenceResult( FInnerSequenceResult& ioInnerSequenceResult ) const
{
    if( !ioInnerSequenceResult.IsValid() )
        return;

    const FMovieSceneSequenceHierarchyNode* Node = ioInnerSequenceResult.mHierarchy->FindNode( ioInnerSequenceResult.mInnerSequenceID );
    const FMovieSceneSubSequenceData*       SubData = ioInnerSequenceResult.mHierarchy->FindSubData( ioInnerSequenceResult.mInnerSequenceID );
    if( !ensure( SubData && Node ) )
        return;

    UMovieSceneSequence* InnerSequence = SubData->GetSequence();
    UMovieScene*         InnerMovieScene = InnerSequence ? InnerSequence->GetMovieScene() : nullptr;
    if( !InnerMovieScene )
        return;

    ioInnerSequenceResult.mNode = Node;
    ioInnerSequenceResult.mSubData = SubData;
    ioInnerSequenceResult.mInnerMovieSceneSequence = InnerSequence;
    ioInnerSequenceResult.mInnerMovieScene = InnerMovieScene;
}

UCameraComponent*
FCinematicBoardSection::FindCameraCutComponentRecursive( FFrameNumber iGlobalTime, FInnerSequenceResult iInnerSequenceResult )
{
    FillInnerSequenceResult( iInnerSequenceResult );
    if( !iInnerSequenceResult.IsFilled() )
        return nullptr;

    FFrameNumber InnerTime = ( iGlobalTime * iInnerSequenceResult.mSubData->RootToSequenceTransform ).FloorToFrame();
    if( !iInnerSequenceResult.mSubData->PlayRange.Value.Contains( InnerTime ) )
    {
        return nullptr;
    }

    int32 LowestRow = TNumericLimits<int32>::Max();
    int32 HighestOverlap = 0;

    UMovieSceneSingleCameraCutSection* ActiveSection = nullptr;

    if( UMovieSceneSingleCameraCutTrack* CutTrack = Cast<UMovieSceneSingleCameraCutTrack>( iInnerSequenceResult.mInnerMovieScene->GetCameraCutTrack() ) )
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
        return ActiveSection->GetFirstCamera( *iInnerSequenceResult.mPlayer, iInnerSequenceResult.mInnerSequenceID );
    }

    for( FMovieSceneSequenceID Child : iInnerSequenceResult.mNode->Children )
    {
        FInnerSequenceResult result( Child, iInnerSequenceResult.mHierarchy, iInnerSequenceResult.mPlayer );

        UCameraComponent* CameraComponent = FindCameraCutComponentRecursive( iGlobalTime, result );
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
    FInnerSequenceResult result = GetInnerSequenceID();
    if( !result.IsValid() )
        return nullptr;

    UCameraComponent* cameraComponent = FindCameraCutComponentRecursive( GetSequencer()->GetGlobalTime().Time.FrameNumber, result );
    if( cameraComponent )
        return cameraComponent;

    return nullptr;
}

//---

static
TArray<FFrameTime>
FindCameraTransformKeys( UMovieSceneCinematicBoardSection* iBoardSection )
{
    TArray<FFrameTime> keys;

    UMovieSceneSequence* innerMovieSceneSequence = iBoardSection->GetSequence();
    if( !innerMovieSceneSequence )
        return keys;

    // if we are on a shot subsequence
    if( innerMovieSceneSequence->IsA<UShotSequence>() )
    {
        TArray<FFrameTime> subkeys = MovieSceneSingleCameraCutHelpers::GetCameraTransformKeys( innerMovieSceneSequence );

        keys = SectionsHelpersConvert::InnerToOuter( iBoardSection, subkeys );

        return keys;
    }

    return keys;
}

static
TArray<FFrameTime>
FindCameraTransformKeysRecursive( UMovieSceneCinematicBoardSection* iBoardSection )
{
    TArray<FFrameTime> keys;

    UMovieSceneSequence* innerMovieSceneSequence = iBoardSection->GetSequence();
    if( !innerMovieSceneSequence )
        return keys;

    // if we are on a shot subsequence
    if( innerMovieSceneSequence->IsA<UShotSequence>() )
    {
        TArray<FFrameTime> subkeys = MovieSceneSingleCameraCutHelpers::GetCameraTransformKeys( innerMovieSceneSequence );

        keys = SectionsHelpersConvert::InnerToOuter( iBoardSection, subkeys );

        return keys;
    }

    // if we are on a board subsequence
    if( innerMovieSceneSequence->IsA<UBoardSequence>() )
    {
        UMovieScene* innerMovieScene = innerMovieSceneSequence->GetMovieScene();
        if( !innerMovieScene )
            return keys;

        UMovieSceneCinematicBoardTrack* board_track = innerMovieScene->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
        if( !board_track )
            return keys;

        TArray<FFrameTime> subkeys;
        for( auto section : board_track->GetAllSections() )
        {
            UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( section );
            TArray<FFrameTime> section_keys;
            section_keys = FindCameraTransformKeysRecursive( board_section );

            subkeys.Append( section_keys );
        }

        keys = SectionsHelpersConvert::InnerToOuter( iBoardSection, subkeys );

        return keys;
    }

    return keys;
}

void
FCinematicBoardSection::BuildThumbnailKeys() //override
{
    UMovieSceneCinematicBoardSection* BoardSection = Cast<UMovieSceneCinematicBoardSection>( Section );

    check( TimeSpace == ETimeSpace::Global ); // Otherwise, TimeSpace must be add as a parameter

    mThumbnailKeys.Empty( mThumbnailKeys.Num() );

    TArray<FFrameTime> keys_as_frame = FindCameraTransformKeysRecursive( BoardSection );
    mThumbnailKeys = SectionsHelpersConvert::FrameToSecond( Section, keys_as_frame );
}

TArray<double>
FCinematicBoardSection::GetThumbnailKeys() const //override
{
    return mThumbnailKeys;
}

void
FCinematicBoardSection::BuildCameraTransformKeys()
{
    UMovieSceneCinematicBoardSection* BoardSection = Cast<UMovieSceneCinematicBoardSection>( Section );

    check( TimeSpace == ETimeSpace::Global ); // Otherwise, TimeSpace must be add as a parameter

    mCameraTransformKeys.Empty( mCameraTransformKeys.Num() );

    TArray<FFrameTime> keys_as_frame = FindCameraTransformKeys( BoardSection );
    mCameraTransformKeys = SectionsHelpersConvert::FrameToSecond( Section, keys_as_frame );
}

TArray<double>
FCinematicBoardSection::GetCameraTransformKeys() const
{
    return mCameraTransformKeys;
}

static
TArray<FFrameTime>
FindPlaneTransformKeys( UMovieSceneCinematicBoardSection* iBoardSection, FMovieScenePossessable iPossessable )
{
    TArray<FFrameTime> keys;

    UMovieSceneSequence* innerMovieSceneSequence = iBoardSection->GetSequence();
    if( !innerMovieSceneSequence )
        return keys;

    // if we are on a shot subsequence
    if( innerMovieSceneSequence->IsA<UShotSequence>() )
    {
        TArray<FFrameTime> subkeys = MovieSceneSingleCameraCutHelpers::GetPlaneTransformKeys( innerMovieSceneSequence, iPossessable );

        keys = SectionsHelpersConvert::InnerToOuter( iBoardSection, subkeys );

        return keys;
    }

    return keys;
}

void
FCinematicBoardSection::BuildPlaneTransformsKeys()
{
    UMovieSceneCinematicBoardSection* BoardSection = Cast<UMovieSceneCinematicBoardSection>( Section );

    mPlaneTransformsKeys.Empty( mPlaneTransformsKeys.Num() );

    TArray<FMovieScenePossessable> possessables = GetPlaneBindings();
    for( auto possessable : possessables )
    {
        TArray<FFrameTime> keys_as_frame = FindPlaneTransformKeys( BoardSection, possessable );
        mPlaneTransformsKeys.Add( possessable.GetGuid() ) = SectionsHelpersConvert::FrameToSecond( Section, keys_as_frame );
    }
}

TArray<double>
FCinematicBoardSection::GetPlaneTransformKeys( FMovieScenePossessable iPossessable ) const
{
    return mPlaneTransformsKeys[iPossessable.GetGuid()];
}

static
TArray<FFrameTime>
FindPlaneMaterialKeys( UMovieSceneCinematicBoardSection* iBoardSection, FMovieScenePossessable iPossessable )
{
    TArray<FFrameTime> keys;

    UMovieSceneSequence* innerMovieSceneSequence = iBoardSection->GetSequence();
    if( !innerMovieSceneSequence )
        return keys;

    // if we are on a shot subsequence
    if( innerMovieSceneSequence->IsA<UShotSequence>() )
    {
        TArray<FFrameTime> subkeys = MovieSceneSingleCameraCutHelpers::GetPlaneMaterialKeys( innerMovieSceneSequence, iPossessable );

        keys = SectionsHelpersConvert::InnerToOuter( iBoardSection, subkeys );

        return keys;
    }

    return keys;
}

void
FCinematicBoardSection::BuildPlaneMaterialsKeys()
{
    UMovieSceneCinematicBoardSection* BoardSection = Cast<UMovieSceneCinematicBoardSection>( Section );

    mPlaneMaterialsKeys.Empty( mPlaneMaterialsKeys.Num() );

    TArray<FMovieScenePossessable> possessables = GetPlaneBindings();
    for( auto possessable : possessables )
    {
        TArray<FFrameTime> keys_as_frame = FindPlaneMaterialKeys( BoardSection, possessable );
        mPlaneMaterialsKeys.Add( possessable.GetGuid() ) = SectionsHelpersConvert::FrameToSecond( Section, keys_as_frame );
    }
}

TArray<double>
FCinematicBoardSection::GetPlaneMaterialKeys( FMovieScenePossessable iPossessable ) const
{
    return mPlaneMaterialsKeys[iPossessable.GetGuid()];
}

void
FCinematicBoardSection::BuildKeys() //override
{
    FKeyThumbnailSection::BuildKeys();
    BuildCameraTransformKeys();
    BuildPlaneTransformsKeys();
    BuildPlaneMaterialsKeys();
}

//---

int
FCinematicBoardSection::GetMaxPlaneBindings() const
{
    UMovieSceneTrack* track = GetSubSectionObject().GetTypedOuter<UMovieSceneTrack>();

    int count = 0;
    for( auto section : track->GetAllSections() )
    {
        UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );
        TArray<FMovieScenePossessable> bindings = GetPlaneBindings( *subsection );

        count = FMath::Max( count, bindings.Num() );
    }

    return count;
}

TArray<FMovieScenePossessable>
FCinematicBoardSection::GetPlaneBindings() const
{
    return GetPlaneBindings( GetSubSectionObject() );
}

TArray<FMovieScenePossessable>
FCinematicBoardSection::GetPlaneBindings( const UMovieSceneSubSection& iSection ) const
{
    TArray<FMovieScenePossessable> bindings;

    FInnerSequenceResult result = GetInnerSequenceID( &iSection );
    if( !result.IsValid() )
        return bindings;

    FillInnerSequenceResult( result );
    if( !result.IsFilled() )
        return bindings;

    AStaticMeshActor* plane = nullptr;
    for( int i = 0; i < result.mInnerMovieScene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = result.mInnerMovieScene->GetPossessable( i );

        for( auto Object : GetSequencer()->FindBoundObjects( possessable.GetGuid(), result.mInnerSequenceID ) )
        {
            plane = Cast<AStaticMeshActor>( Object.Get() );
            if( plane )
                bindings.Add( possessable );
        }
    }

    return bindings;
}

FMovieScenePossessable
FCinematicBoardSection::GetCameraBinding() const
{
    FMovieScenePossessable binding;

    FInnerSequenceResult result = GetInnerSequenceID();
    if( !result.IsValid() )
        return binding;

    FillInnerSequenceResult( result );
    if( !result.IsFilled() )
        return binding;

    ACineCameraActor* camera = nullptr;
    for( int i = 0; i < result.mInnerMovieScene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable possessable = result.mInnerMovieScene->GetPossessable( i );

        for( auto Object : GetSequencer()->FindBoundObjects( possessable.GetGuid(), result.mInnerSequenceID ) )
        {
            camera = Cast<ACineCameraActor>( Object.Get() );
            if( camera )
                return possessable;
        }
    }

    return binding;
}

TSharedRef<SWidget>
FCinematicBoardSection::GenerateSectionWidget()
{
    TSharedRef<FCinematicBoardSection> me = SharedThis( this );

    return SAssignNew( mWidgetLayout, SCinematicBoardSectionLayout, me )
        .Title()
        [
            SAssignNew( mWidgetTitle, SCinematicBoardSectionTitle, me )
        ]
        .Camera()
        [
            SNew( SCinematicBoardSectionCamera, me )
        ]
        .Thumbnails()
        [
            SNew( SCinematicBoardSectionThumbnails, me )
        ]
        .Planes()
        [
            SNew( SCinematicBoardSectionPlanes, me )
        ];
}

void
FCinematicBoardSection::Tick( const FGeometry& iAllottedGeometry, const FGeometry& iClippedGeometry, const double iCurrentTime, const float iDeltaTime )
{
    // Set cached data
    UMovieSceneCinematicBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();
    FCinematicSectionCache newCacheData( &sectionObject );
    if( newCacheData != mThumbnailCacheData )
    {
        KeyThumbnailCache.ForceRedraw();
    }
    mThumbnailCacheData = newCacheData;

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

    const UEposMovieSceneSequence* subsequence = Cast< UEposMovieSceneSequence>( sectionObject.GetSequence() );
    if( subsequence )
        ioPainter.LayerId = ioPainter.PaintSectionBackground( subsequence->GetColorTint() );
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

        ioMenuBuilder.AddMenuEntry(
            LOCTEXT( "RenderBoard", "Render Board" ),
            FText::Format( LOCTEXT( "RenderBoardTooltip", "Render board movie" ), FText::FromString( sectionObject.GetBoardDisplayName() ) ),
            FSlateIcon(),
            FUIAction( FExecuteAction::CreateSP( mCinematicBoardTrackEditor.Pin().ToSharedRef(), &FCinematicBoardTrackEditor::RenderBoard, &sectionObject ) )
        );

        ioMenuBuilder.AddMenuEntry(
            LOCTEXT( "RenameBoard", "Rename Board" ),
            FText::Format( LOCTEXT( "RenameBoardTooltip", "Rename {0}" ), FText::FromString( sectionObject.GetBoardDisplayName() ) ),
            FSlateIcon(),
            FUIAction( FExecuteAction::CreateSP( mWidgetTitle.ToSharedRef(), &SCinematicBoardSectionTitle::EnterRename ) )
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
