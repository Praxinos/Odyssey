// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "BoardTrack/BoardSection.h"
#include "BoardTrack/MovieSceneBoardSection.h"
#include "Rendering/DrawElements.h"
#include "Textures/SlateIcon.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ScopedTransaction.h"
#include "MovieSceneTrack.h"
#include "MovieScene.h"
#include "BoardTrack/BoardTrackEditor.h"
#include "SequencerSectionPainter.h"
#include "EditorStyleSet.h"
#include "MovieSceneToolHelpers.h"
#include "MovieSceneTimeHelpers.h"

#include "Tracks/MovieSceneCameraCutTrack.h"
#include "Sections/MovieSceneCameraCutSection.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "CommonMovieSceneTools.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "FBoardSection"


/* FBoardSection structors
 *****************************************************************************/

FBoardSection::FCinematicSectionCache::FCinematicSectionCache( UMovieSceneBoardSection* Section )
    : InnerFrameRate( 1, 1 )
    , InnerFrameOffset( 0 )
    , SectionStartFrame( 0 )
    , TimeScale( 1.f )
{
    if( Section )
    {
        UMovieSceneSequence* InnerSequence = Section->GetSequence();
        if( InnerSequence )
        {
            InnerFrameRate = InnerSequence->GetMovieScene()->GetTickResolution();
        }

        InnerFrameOffset = Section->Parameters.StartFrameOffset;
        SectionStartFrame = Section->HasStartFrame() ? Section->GetInclusiveStartFrame() : 0;
        TimeScale = Section->Parameters.TimeScale;
    }
}


FBoardSection::FBoardSection( TSharedPtr<ISequencer> InSequencer, UMovieSceneBoardSection& InSection, TSharedPtr<FBoardTrackEditor> InBoardTrackEditor, TSharedPtr<FTrackEditorThumbnailPool> InThumbnailPool )
    : TSubSectionMixin( InSequencer, InSection, InSequencer, InThumbnailPool, InSection )
    , BoardTrackEditor( InBoardTrackEditor )
    , ThumbnailCacheData( &InSection )
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
FBoardSection::SetSingleTime( double GlobalTime )
{
    UMovieSceneBoardSection& SectionObject = GetSectionObjectAs<UMovieSceneBoardSection>();
    double ReferenceOffsetSeconds = SectionObject.HasStartFrame() ? SectionObject.GetInclusiveStartFrame() / SectionObject.GetTypedOuter<UMovieScene>()->GetTickResolution() : 0;
    SectionObject.SetThumbnailReferenceOffset( GlobalTime - ReferenceOffsetSeconds );
}

UCameraComponent* 
FindCameraCutComponentRecursive( FFrameNumber GlobalTime, FMovieSceneSequenceID InnerSequenceID, const FMovieSceneSequenceHierarchy& Hierarchy, IMovieScenePlayer& Player )
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
    TSharedPtr<ISequencer> Sequencer = GetSequencer();
    if( !Sequencer.IsValid() )
    {
        return nullptr;
    }


    const UMovieSceneBoardSection&          SectionObject = GetSectionObjectAs<UMovieSceneBoardSection>();
    const FMovieSceneSequenceID             ThisSequenceID = Sequencer->GetFocusedTemplateID();
    const FMovieSceneSequenceID             TargetSequenceID = SectionObject.GetSequenceID();
    const FMovieSceneSequenceHierarchy&     Hierarchy = Sequencer->GetEvaluationTemplate().GetHierarchy();
    const FMovieSceneSequenceHierarchyNode* ThisSequenceNode = Hierarchy.FindNode( ThisSequenceID );

    check( ThisSequenceNode );

    // Find the TargetSequenceID by comparing deterministic sequence IDs for all children of the current node
    const FMovieSceneSequenceID* InnerSequenceID = Algo::FindByPredicate( ThisSequenceNode->Children,
                                                                          [&Hierarchy, TargetSequenceID]( FMovieSceneSequenceID InSequenceID )
    {
        const FMovieSceneSubSequenceData* SubData = Hierarchy.FindSubData( InSequenceID );
        return SubData && SubData->DeterministicSequenceID == TargetSequenceID;
    }
    );

    if( InnerSequenceID )
    {
        UCameraComponent* CameraComponent = FindCameraCutComponentRecursive( Sequencer->GetGlobalTime().Time.FrameNumber, *InnerSequenceID, Hierarchy, *Sequencer );
        if( CameraComponent )
        {
            return CameraComponent;
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
FBoardSection::Tick( const FGeometry& AllottedGeometry, const FGeometry& ClippedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    // Set cached data
    UMovieSceneBoardSection& SectionObject = GetSectionObjectAs<UMovieSceneBoardSection>();
    FCinematicSectionCache NewCacheData( &SectionObject );
    if( NewCacheData != ThumbnailCacheData )
    {
        ThumbnailCache.ForceRedraw();
    }
    ThumbnailCacheData = NewCacheData;

    // Update single reference frame settings
    if( GetDefault<UMovieSceneUserThumbnailSettings>()->bDrawSingleThumbnails && SectionObject.HasStartFrame() )
    {
        double ReferenceTime = SectionObject.GetInclusiveStartFrame() / SectionObject.GetTypedOuter<UMovieScene>()->GetTickResolution() + SectionObject.GetThumbnailReferenceOffset();
        ThumbnailCache.SetSingleReferenceFrame( ReferenceTime );
    }
    else
    {
        ThumbnailCache.SetSingleReferenceFrame( TOptional<double>() );
    }

    FViewportThumbnailSection::Tick( AllottedGeometry, ClippedGeometry, InCurrentTime, InDeltaTime );
}

int32
FBoardSection::OnPaintSection( FSequencerSectionPainter& InPainter ) const
{
    static const FSlateBrush* FilmBorder = FEditorStyle::GetBrush( "Sequencer.Section.FilmBorder" );

    InPainter.LayerId = InPainter.PaintSectionBackground();

    FVector2D LocalSectionSize = InPainter.SectionGeometry.GetLocalSize();
    const UMovieSceneBoardSection& SectionObject = GetSectionObjectAs<UMovieSceneBoardSection>();

    // Paint fancy-looking film border.
    FSlateDrawElement::MakeBox(
        InPainter.DrawElements,
        InPainter.LayerId++,
        InPainter.SectionGeometry.ToPaintGeometry( FVector2D( LocalSectionSize.X - 2.f, 7.f ), FSlateLayoutTransform( FVector2D( 1.f, 4.f ) ) ),
        FilmBorder,
        InPainter.bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect
    );

    FSlateDrawElement::MakeBox(
        InPainter.DrawElements,
        InPainter.LayerId++,
        InPainter.SectionGeometry.ToPaintGeometry( FVector2D( LocalSectionSize.X - 2.f, 7.f ), FSlateLayoutTransform( FVector2D( 1.f, LocalSectionSize.Y - 11.f ) ) ),
        FilmBorder,
        InPainter.bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect
    );

    // Paint the thumbnails.
    FViewportThumbnailSection::OnPaintSection( InPainter );

    // Paint the sub-sequence information/looping boundaries/etc.

    FSubSectionPainterParams SubSectionPainterParams( GetContentPadding() );
    SubSectionPainterParams.bShowTrackNum = false;

    FSubSectionPainterUtil::PaintSection( GetSequencer(), SectionObject, InPainter, SubSectionPainterParams );

    return InPainter.LayerId;
}

void
FBoardSection::BuildSectionContextMenu( FMenuBuilder& MenuBuilder, const FGuid& ObjectBinding )
{
    FViewportThumbnailSection::BuildSectionContextMenu( MenuBuilder, ObjectBinding );

    UMovieSceneBoardSection& SectionObject = GetSectionObjectAs<UMovieSceneBoardSection>();

    MenuBuilder.BeginSection( NAME_None, LOCTEXT( "BoardMenuText", "Board" ) );
    {
        //MenuBuilder.AddSubMenu(
        //    LOCTEXT( "TakesMenu", "Takes" ),
        //    LOCTEXT( "TakesMenuTooltip", "Shot takes" ),
        //    FNewMenuDelegate::CreateLambda( [=]( FMenuBuilder& InMenuBuilder )
        //{
        //    AddTakesMenu( InMenuBuilder );
        //} ) );

        //MenuBuilder.AddMenuEntry(
        //    LOCTEXT( "NewTake", "New Take" ),
        //    FText::Format( LOCTEXT( "NewTakeTooltip", "Create a new take for {0}" ), FText::FromString( SectionObject.GetShotDisplayName() ) ),
        //    FSlateIcon(),
        //    FUIAction( FExecuteAction::CreateSP( CinematicShotTrackEditor.Pin().ToSharedRef(), &FCinematicShotTrackEditor::NewTake, &SectionObject ) )
        //);

        MenuBuilder.AddMenuEntry(
            LOCTEXT( "InsertNewBoard", "Insert Board" ),
            LOCTEXT( "InsertNewBoardTooltip", "Insert a new board at the current time" ),
            FSlateIcon(),
            FUIAction( FExecuteAction::CreateSP( BoardTrackEditor.Pin().ToSharedRef(), &FBoardTrackEditor::InsertBoard ) )
        );

        MenuBuilder.AddMenuEntry(
            LOCTEXT( "DuplicateBoard", "Duplicate Board" ),
            FText::Format( LOCTEXT( "DuplicateBoardTooltip", "Duplicate {0} to create a new board" ), FText::FromString( SectionObject.GetBoardDisplayName() ) ),
            FSlateIcon(),
            FUIAction( FExecuteAction::CreateSP( BoardTrackEditor.Pin().ToSharedRef(), &FBoardTrackEditor::DuplicateBoard, &SectionObject ) )
        );

        MenuBuilder.AddMenuEntry(
            LOCTEXT( "RenderBoard", "Render Board" ),
            FText::Format( LOCTEXT( "RenderBoardTooltip", "Render board movie" ), FText::FromString( SectionObject.GetBoardDisplayName() ) ),
            FSlateIcon(),
            FUIAction( FExecuteAction::CreateSP( BoardTrackEditor.Pin().ToSharedRef(), &FBoardTrackEditor::RenderBoard, &SectionObject ) )
        );

        MenuBuilder.AddMenuEntry(
            LOCTEXT( "RenameBoard", "Rename Board" ),
            FText::Format( LOCTEXT( "RenameBoardTooltip", "Rename {0}" ), FText::FromString( SectionObject.GetBoardDisplayName() ) ),
            FSlateIcon(),
            FUIAction( FExecuteAction::CreateSP( this, &FBoardSection::EnterRename ) )
        );
    }
    MenuBuilder.EndSection();
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
    const UMovieSceneBoardSection& SectionObject = GetSectionObjectAs<UMovieSceneBoardSection>();
    return FText::FromString( SectionObject.GetBoardDisplayName() );
}


void
FBoardSection::HandleThumbnailTextBlockTextCommitted( const FText& NewBoardName, ETextCommit::Type CommitType )
{
    if( CommitType == ETextCommit::OnEnter && !HandleThumbnailTextBlockText().EqualTo( NewBoardName ) )
    {
        UMovieSceneBoardSection& SectionObject = GetSectionObjectAs<UMovieSceneBoardSection>();

        SectionObject.Modify();

        const FScopedTransaction Transaction( LOCTEXT( "SetBoardName", "Set Board Name" ) );

        SectionObject.SetBoardDisplayName( NewBoardName.ToString() );
    }
}

#undef LOCTEXT_NAMESPACE
