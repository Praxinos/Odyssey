// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "CinematicBoardTrack/CinematicBoardSection.h"

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

#include "CinematicBoardTrack/CinematicBoardTrackEditor.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "SectionsHelpersResize.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"

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
    return GetRenameVisibility() == EVisibility::Visible ? FText::GetEmpty() : HandleThumbnailTextBlockText();
}

float
FCinematicBoardSection::GetSectionHeight() const
{
    return FViewportThumbnailSection::GetSectionHeight() + 2 * 9.f;
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

UCameraComponent* 
FindCameraCutComponentRecursive( FFrameNumber iGlobalTime, FMovieSceneSequenceID iInnerSequenceID, const FMovieSceneSequenceHierarchy& iHierarchy, IMovieScenePlayer& ioPlayer )
{
    const FMovieSceneSequenceHierarchyNode* Node = iHierarchy.FindNode( iInnerSequenceID );
    const FMovieSceneSubSequenceData*       SubData = iHierarchy.FindSubData( iInnerSequenceID );
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
        return ActiveSection->GetFirstCamera( ioPlayer, iInnerSequenceID );
    }

    for( FMovieSceneSequenceID Child : Node->Children )
    {
        UCameraComponent* CameraComponent = FindCameraCutComponentRecursive( iGlobalTime, Child, iHierarchy, ioPlayer );
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
    {
        return nullptr;
    }


    const UMovieSceneCinematicBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();
    const FMovieSceneSequenceID             thisSequenceID = sequencer->GetFocusedTemplateID();
    const FMovieSceneSequenceID             targetSequenceID = sectionObject.GetSequenceID();
    const FMovieSceneSequenceHierarchy*     hierarchy = sequencer->GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( sequencer->GetEvaluationTemplate().GetCompiledDataID() );

    if( !hierarchy )
    {
        return nullptr;
    }

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
	
	if( innerSequenceID )
	{
		UCameraComponent* cameraComponent = FindCameraCutComponentRecursive( sequencer->GetGlobalTime().Time.FrameNumber, *innerSequenceID, *hierarchy, *sequencer );
		if( cameraComponent )
		{
			return cameraComponent;
		}
	}

	return nullptr;
}

bool
FCinematicBoardSection::IsReadOnly() const
{
    // Overridden to false regardless of movie scene section read only state so that we can double click into the sub section
    return false;
}

void
FCinematicBoardSection::Tick( const FGeometry& iAllottedGeometry, const FGeometry& iClippedGeometry, const double iCurrentTime, const float iDeltaTime )
{
    // Set cached data
    UMovieSceneCinematicBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();
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
FCinematicBoardSection::OnPaintSection( FSequencerSectionPainter& ioPainter ) const
{
    static const FSlateBrush* filmBorder = FEditorStyle::GetBrush( "Sequencer.Section.FilmBorder" );

    ioPainter.LayerId = ioPainter.PaintSectionBackground();

    FVector2D localSectionSize = ioPainter.SectionGeometry.GetLocalSize();
    const UMovieSceneCinematicBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();

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
FCinematicBoardSection::BuildSectionContextMenu( FMenuBuilder& ioMenuBuilder, const FGuid& iObjectBinding )
{
    FViewportThumbnailSection::BuildSectionContextMenu( ioMenuBuilder, iObjectBinding );

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
            FUIAction( FExecuteAction::CreateSP( this, &FCinematicBoardSection::EnterRename ) )
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
    UMovieSceneCinematicBoardSection& sectionObject = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();
    sectionObject.StartResizing();
}

void
FCinematicBoardSection::ResizeSection( ESequencerSectionResizeMode ResizeMode, FFrameNumber ResizeFrameNumber )
{
    UMovieSceneCinematicBoardSection& section = GetSectionObjectAs<UMovieSceneCinematicBoardSection>();
    section.Resizing();

    //FViewportThumbnailSection::ResizeSection( ResizeMode, ResizeFrameNumber );

    UMovieScene* outer_movie_scene = section.GetTypedOuter<UMovieScene>();
    int32 IntervalSnapThreshold = FMath::RoundToInt( ( outer_movie_scene->GetTickResolution() / outer_movie_scene->GetDisplayRate() ).AsDecimal() );

    if( ResizeMode == ESequencerSectionResizeMode::SSRM_LeadingEdge )
    {
        section.SetRange( MovieSceneHelpersResize::GetValidRangeLeading( outer_movie_scene->GetAllSections(), &section, ResizeFrameNumber, IntervalSnapThreshold ) );
    }
    else
    {
        section.SetRange( MovieSceneHelpersResize::GetValidRangeTrailing( outer_movie_scene->GetAllSections(), &section, ResizeFrameNumber, IntervalSnapThreshold ) );
    }
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
