// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "BoardTrack/BoardSection.h"

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
//#include "Tracks/MovieSceneCameraCutTrack.h"
//#include "Sections/MovieSceneCameraCutSection.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "CommonMovieSceneTools.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Editor.h"

#include "BoardTrack/BoardTrackEditor.h"
#include "BoardTrack/MovieSceneBoardSection.h"

#define LOCTEXT_NAMESPACE "FBoardSection"


/* FBoardSection structors
 *****************************************************************************/

FBoardSection::FCinematicSectionCache::FCinematicSectionCache( UMovieSceneBoardSection* iSection )
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
FBoardSection::FCinematicSectionCache::operator!=( const FCinematicSectionCache& iRHS ) const
{
    return mInnerFrameRate != iRHS.mInnerFrameRate 
        || mInnerFrameOffset != iRHS.mInnerFrameOffset 
        || mSectionStartFrame != iRHS.mSectionStartFrame 
        || mTimeScale != iRHS.mTimeScale;
}

//---

FBoardSection::FBoardSection( TSharedPtr<ISequencer> iSequencer, UMovieSceneBoardSection& iSection, TSharedPtr<FBoardTrackEditor> iBoardTrackEditor, TSharedPtr<FTrackEditorThumbnailPool> iThumbnailPool )
    : TSubSectionMixin( iSequencer, iSection, iSequencer, iThumbnailPool, iSection )
    , mBoardTrackEditor( iBoardTrackEditor )
    , mThumbnailCacheData( &iSection )
{
    AdditionalDrawEffect = ESlateDrawEffect::NoGamma;
}


FBoardSection::~FBoardSection()
{
}

FText
FBoardSection::GetSectionTitle() const
{
    return GetRenameVisibility() == EVisibility::Visible ? FText::GetEmpty() : HandleThumbnailTextBlockText();
}

float
FBoardSection::GetSectionHeight() const
{
    return FViewportThumbnailSection::GetSectionHeight() + 2 * 9.f;
}

FMargin
FBoardSection::GetContentPadding() const
{
    return FMargin( 8.f, 15.f );
}

void
FBoardSection::SetSingleTime( double iGlobalTime )
{
    UMovieSceneBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneBoardSection>();
    double referenceOffsetSeconds = sectionObject.HasStartFrame() ? sectionObject.GetInclusiveStartFrame() / sectionObject.GetTypedOuter<UMovieScene>()->GetTickResolution() : 0;
    sectionObject.SetThumbnailReferenceOffset( iGlobalTime - referenceOffsetSeconds );
}

UCameraComponent* 
FindCameraCutComponentRecursive( FFrameNumber iGlobalTime, FMovieSceneSequenceID iInnerSequenceID, const FMovieSceneSequenceHierarchy& iHierarchy, IMovieScenePlayer& ioPlayer )
{
    //const FMovieSceneSequenceHierarchyNode* Node = Hierarchy.FindNode( InnerSequenceID );
    //const FMovieSceneSubSequenceData*       SubData = Hierarchy.FindSubData( InnerSequenceID );
    //if( !ensure( SubData && Node ) )
    //{
    //    return nullptr;
    //}

    //UMovieSceneSequence* InnerSequence = SubData->GetSequence();
    //UMovieScene*         InnerMovieScene = InnerSequence ? InnerSequence->GetMovieScene() : nullptr;
    //if( !InnerMovieScene )
    //{
    //    return nullptr;
    //}

    //FFrameNumber InnerTime = ( GlobalTime * SubData->RootToSequenceTransform ).FloorToFrame();
    //if( !SubData->PlayRange.Value.Contains( InnerTime ) )
    //{
    //    return nullptr;
    //}

    //int32 LowestRow = TNumericLimits<int32>::Max();
    //int32 HighestOverlap = 0;

    //UMovieSceneCameraCutSection* ActiveSection = nullptr;

    //if( UMovieSceneCameraCutTrack* CutTrack = Cast<UMovieSceneCameraCutTrack>( InnerMovieScene->GetCameraCutTrack() ) )
    //{
    //    for( UMovieSceneSection* ItSection : CutTrack->GetAllSections() )
    //    {
    //        UMovieSceneCameraCutSection* CutSection = Cast<UMovieSceneCameraCutSection>( ItSection );
    //        if( CutSection && CutSection->GetRange().Contains( InnerTime ) )
    //        {
    //            bool bSectionWins =
    //                ( CutSection->GetRowIndex() < LowestRow ) ||
    //                ( CutSection->GetRowIndex() == LowestRow && CutSection->GetOverlapPriority() > HighestOverlap );

    //            if( bSectionWins )
    //            {
    //                HighestOverlap = CutSection->GetOverlapPriority();
    //                LowestRow = CutSection->GetRowIndex();
    //                ActiveSection = CutSection;
    //            }
    //        }
    //    }
    //}

    //if( ActiveSection )
    //{
    //    return ActiveSection->GetFirstCamera( Player, InnerSequenceID );
    //}

    //for( FMovieSceneSequenceID Child : Node->Children )
    //{
    //    UCameraComponent* CameraComponent = FindCameraCutComponentRecursive( GlobalTime, Child, Hierarchy, Player );
    //    if( CameraComponent )
    //    {
    //        return CameraComponent;
    //    }
    //}

    return nullptr;
}

UCameraComponent*
FBoardSection::GetViewCamera()
{
    TSharedPtr<ISequencer> sequencer = GetSequencer();
    if( !sequencer.IsValid() )
    {
        return nullptr;
    }


    const UMovieSceneBoardSection&          sectionObject = GetSectionObjectAs<UMovieSceneBoardSection>();
    const FMovieSceneSequenceID             thisSequenceID = sequencer->GetFocusedTemplateID();
    const FMovieSceneSequenceID             targetSequenceID = sectionObject.GetSequenceID();
    const FMovieSceneSequenceHierarchy&     hierarchy = sequencer->GetEvaluationTemplate().GetHierarchy();
    const FMovieSceneSequenceHierarchyNode* thisSequenceNode = hierarchy.FindNode( thisSequenceID );

    check( thisSequenceNode );

    // Find the TargetSequenceID by comparing deterministic sequence IDs for all children of the current node
    const FMovieSceneSequenceID* innerSequenceID = Algo::FindByPredicate( thisSequenceNode->Children,
                                                                          [&hierarchy, targetSequenceID]( FMovieSceneSequenceID iSequenceID )
    {
        const FMovieSceneSubSequenceData* subData = hierarchy.FindSubData( iSequenceID );
        return subData && subData->DeterministicSequenceID == targetSequenceID;
    }
    );

    if( innerSequenceID )
    {
        UCameraComponent* cameraComponent = FindCameraCutComponentRecursive( sequencer->GetGlobalTime().Time.FrameNumber, *innerSequenceID, hierarchy, *sequencer );
        if( cameraComponent )
        {
            return cameraComponent;
        }
    }

    return nullptr;
}

bool
FBoardSection::IsReadOnly() const
{
    // Overridden to false regardless of movie scene section read only state so that we can double click into the sub section
    return false;
}

void
FBoardSection::Tick( const FGeometry& iAllottedGeometry, const FGeometry& iClippedGeometry, const double iCurrentTime, const float iDeltaTime )
{
    // Set cached data
    UMovieSceneBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneBoardSection>();
    FCinematicSectionCache newCacheData( &sectionObject );
    if( newCacheData != mThumbnailCacheData )
    {
        ThumbnailCache.ForceRedraw();
    }
    mThumbnailCacheData = newCacheData;

    // Update single reference frame settings
    if( GetDefault<UMovieSceneUserThumbnailSettings>()->bDrawSingleThumbnails && sectionObject.HasStartFrame() )
    {
        double referenceTime = sectionObject.GetInclusiveStartFrame() / sectionObject.GetTypedOuter<UMovieScene>()->GetTickResolution() + sectionObject.GetThumbnailReferenceOffset();
        ThumbnailCache.SetSingleReferenceFrame( referenceTime );
    }
    else
    {
        ThumbnailCache.SetSingleReferenceFrame( TOptional<double>() );
    }

    FViewportThumbnailSection::Tick( iAllottedGeometry, iClippedGeometry, iCurrentTime, iDeltaTime );
}

int32
FBoardSection::OnPaintSection( FSequencerSectionPainter& ioPainter ) const
{
    static const FSlateBrush* filmBorder = FEditorStyle::GetBrush( "Sequencer.Section.FilmBorder" );

    ioPainter.LayerId = ioPainter.PaintSectionBackground();

    FVector2D localSectionSize = ioPainter.SectionGeometry.GetLocalSize();
    const UMovieSceneBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneBoardSection>();

    // Paint fancy-looking film border.
    FSlateDrawElement::MakeBox(
        ioPainter.DrawElements,
        ioPainter.LayerId++,
        ioPainter.SectionGeometry.ToPaintGeometry( FVector2D( localSectionSize.X - 2.f, 7.f ), FSlateLayoutTransform( FVector2D( 1.f, 4.f ) ) ),
        filmBorder,
        ioPainter.bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect
    );

    FSlateDrawElement::MakeBox(
        ioPainter.DrawElements,
        ioPainter.LayerId++,
        ioPainter.SectionGeometry.ToPaintGeometry( FVector2D( localSectionSize.X - 2.f, 7.f ), FSlateLayoutTransform( FVector2D( 1.f, localSectionSize.Y - 11.f ) ) ),
        filmBorder,
        ioPainter.bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect
    );

    // Paint the thumbnails.
    FViewportThumbnailSection::OnPaintSection( ioPainter );

    // Paint the sub-sequence information/looping boundaries/etc.

    FSubSectionPainterParams subSectionPainterParams( GetContentPadding() );
    subSectionPainterParams.bShowTrackNum = false;

    FSubSectionPainterUtil::PaintSection( GetSequencer(), sectionObject, ioPainter, subSectionPainterParams );

    return ioPainter.LayerId;
}

void
FBoardSection::BuildSectionContextMenu( FMenuBuilder& ioMenuBuilder, const FGuid& iObjectBinding )
{
    FViewportThumbnailSection::BuildSectionContextMenu( ioMenuBuilder, iObjectBinding );

    UMovieSceneBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneBoardSection>();

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
            FUIAction( FExecuteAction::CreateSP( mBoardTrackEditor.Pin().ToSharedRef(), &FBoardTrackEditor::InsertBoard ) )
        );

        ioMenuBuilder.AddMenuEntry(
            LOCTEXT( "DuplicateBoard", "Duplicate Board" ),
            FText::Format( LOCTEXT( "DuplicateBoardTooltip", "Duplicate {0} to create a new board" ), FText::FromString( sectionObject.GetBoardDisplayName() ) ),
            FSlateIcon(),
            FUIAction( FExecuteAction::CreateSP( mBoardTrackEditor.Pin().ToSharedRef(), &FBoardTrackEditor::DuplicateBoard, &sectionObject ) )
        );

        ioMenuBuilder.AddMenuEntry(
            LOCTEXT( "RenderBoard", "Render Board" ),
            FText::Format( LOCTEXT( "RenderBoardTooltip", "Render board movie" ), FText::FromString( sectionObject.GetBoardDisplayName() ) ),
            FSlateIcon(),
            FUIAction( FExecuteAction::CreateSP( mBoardTrackEditor.Pin().ToSharedRef(), &FBoardTrackEditor::RenderBoard, &sectionObject ) )
        );

        ioMenuBuilder.AddMenuEntry(
            LOCTEXT( "RenameBoard", "Rename Board" ),
            FText::Format( LOCTEXT( "RenameBoardTooltip", "Rename {0}" ), FText::FromString( sectionObject.GetBoardDisplayName() ) ),
            FSlateIcon(),
            FUIAction( FExecuteAction::CreateSP( this, &FBoardSection::EnterRename ) )
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

/* FBoardSection callbacks
 *****************************************************************************/

FText
FBoardSection::HandleThumbnailTextBlockText() const
{
    const UMovieSceneBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneBoardSection>();
    return FText::FromString( sectionObject.GetBoardDisplayName() );
}


void
FBoardSection::HandleThumbnailTextBlockTextCommitted( const FText& iNewBoardName, ETextCommit::Type iCommitType )
{
    if( iCommitType == ETextCommit::OnEnter && !HandleThumbnailTextBlockText().EqualTo( iNewBoardName ) )
    {
        UMovieSceneBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneBoardSection>();

        sectionObject.Modify();

        const FScopedTransaction transaction( LOCTEXT( "SetBoardName", "Set Board Name" ) );

        sectionObject.SetBoardDisplayName( iNewBoardName.ToString() );
    }
}

#undef LOCTEXT_NAMESPACE
