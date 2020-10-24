// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "CinematicBoardTrack/CinematicBoardTrackEditor.h"

#include "Misc/Paths.h"
#include "Widgets/SBoxPanel.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GameFramework/Actor.h"
#include "Factories/Factory.h"
#include "Tracks/MovieSceneSubTrack.h"
#include "Modules/ModuleManager.h"
#include "Application/ThrottleManager.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "EditorStyleSet.h"
#include "LevelEditorViewport.h"
#include "CommonMovieSceneTools.h"
#include "MovieSceneToolHelpers.h"
#include "FCPXML/FCPXMLMovieSceneTranslator.h"
#include "SequencerUtilities.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "UObject/UObjectHash.h"
#include "UObject/UObjectIterator.h"
#include "AssetToolsModule.h"
#include "TrackEditorThumbnail/TrackEditorThumbnailPool.h"
#include "MovieSceneToolsProjectSettings.h"
#include "Editor.h"
#include "DragAndDrop/AssetDragDropOp.h"
#include "MovieSceneTimeHelpers.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposTracksEditorHelpers.h"
#include "Shot/ShotSequence.h"
#include "Styles/EposEditorStyle.h"

#define LOCTEXT_NAMESPACE "FCinematicBoardTrackEditor"

/* FCinematicBoardTrackEditor structors
 *****************************************************************************/

FCinematicBoardTrackEditor::FCinematicBoardTrackEditor( TSharedRef<ISequencer> iSequencer )
    : FMovieSceneTrackEditor( iSequencer )
{
    mThumbnailPool = MakeShareable( new FTrackEditorThumbnailPool( iSequencer ) );
}


TSharedRef<ISequencerTrackEditor>
FCinematicBoardTrackEditor::CreateTrackEditor( TSharedRef<ISequencer> iSequencer )
{
    return MakeShareable( new FCinematicBoardTrackEditor( iSequencer ) );
}


TWeakObjectPtr<AActor>
FCinematicBoardTrackEditor::GetBoardCamera() const
{
    return mBoardCamera;
}


void
FCinematicBoardTrackEditor::OnInitialize()
{
    mOnCameraCutHandle = GetSequencer()->OnCameraCut().AddSP( this, &FCinematicBoardTrackEditor::OnUpdateCameraCut );
}


void
FCinematicBoardTrackEditor::OnRelease()
{
    if( mOnCameraCutHandle.IsValid() && GetSequencer().IsValid() )
    {
        GetSequencer()->OnCameraCut().Remove( mOnCameraCutHandle );
    }
}


/* ISequencerTrackEditor interface
 *****************************************************************************/

void
FCinematicBoardTrackEditor::BuildAddTrackMenu( FMenuBuilder& ioMenuBuilder )
{
    ioMenuBuilder.AddMenuEntry(
        LOCTEXT( "AddCinematicBoardTrack", "Board Track" ),
        LOCTEXT( "AddCinematicBoardTooltip", "Adds a board track." ),
        FSlateIcon( FEposEditorStyle::Get()->GetStyleSetName(), "Sequencer.Tracks.CinematicBoard" ),
        FUIAction(
            FExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::HandleAddCinematicBoardTrackMenuEntryExecute ),
            FCanExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::HandleAddCinematicBoardTrackMenuEntryCanExecute )
        )
    );
}


TSharedPtr<SWidget>
FCinematicBoardTrackEditor::BuildOutlinerEditWidget( const FGuid& iObjectBinding, UMovieSceneTrack* iTrack, const FBuildEditWidgetParams& iParams )
{
    // Create a container edit box
    return SNew( SHorizontalBox )

        // Add the camera combo box
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign( VAlign_Center )
        [
            FSequencerUtilities::MakeAddButton( LOCTEXT( "BoardText", "Board" ), FOnGetContent::CreateSP( this, &FCinematicBoardTrackEditor::HandleAddBoardComboButtonGetMenuContent ), iParams.NodeIsHovered, GetSequencer() )
        ]

    + SHorizontalBox::Slot()
        .VAlign( VAlign_Center )
        .HAlign( HAlign_Right )
        .AutoWidth()
        .Padding( 4, 0, 0, 0 )
        [
            SNew( SCheckBox )
            .IsFocusable( false )
        .IsChecked( this, &FCinematicBoardTrackEditor::AreBoardsLocked )
        .OnCheckStateChanged( this, &FCinematicBoardTrackEditor::OnLockBoardsClicked )
        .ToolTipText( this, &FCinematicBoardTrackEditor::GetLockBoardsToolTip )
        .ForegroundColor( FLinearColor::White )
        .CheckedImage( FEditorStyle::GetBrush( "Sequencer.LockCamera" ) )
        .CheckedHoveredImage( FEditorStyle::GetBrush( "Sequencer.LockCamera" ) )
        .CheckedPressedImage( FEditorStyle::GetBrush( "Sequencer.LockCamera" ) )
        .UncheckedImage( FEditorStyle::GetBrush( "Sequencer.UnlockCamera" ) )
        .UncheckedHoveredImage( FEditorStyle::GetBrush( "Sequencer.UnlockCamera" ) )
        .UncheckedPressedImage( FEditorStyle::GetBrush( "Sequencer.UnlockCamera" ) )
        ];
}


TSharedRef<ISequencerSection>
FCinematicBoardTrackEditor::MakeSectionInterface( UMovieSceneSection& iSectionObject, UMovieSceneTrack& ioTrack, FGuid iObjectBinding )
{
    check( SupportsType( iSectionObject.GetOuter()->GetClass() ) );

    UMovieSceneCinematicBoardSection& sectionObjectImpl = *CastChecked<UMovieSceneCinematicBoardSection>( &iSectionObject );
    return MakeShareable( new FCinematicBoardSection( GetSequencer(), sectionObjectImpl, SharedThis( this ), mThumbnailPool ) );
}


bool
FCinematicBoardTrackEditor::HandleAssetAdded( UObject* iAsset, const FGuid& iTargetObjectGuid )
{
    UMovieSceneSequence* sequence = Cast<UMovieSceneSequence>( iAsset );

    if( sequence == nullptr )
    {
        return false;
    }

    if( !( ( sequence->GetClass()->GetName() == TEXT( "BoardSequence" ) ) || ( sequence->GetClass()->GetName() == TEXT( "ShotSequence" ) ) ) )
    //if( !SupportsSequence( sequence ) )
    {
        return false;
    }

    //@todo If there's already a subscenes track, allow that track to handle this asset
    UMovieScene* focusedMovieScene = GetFocusedMovieScene();

    if( focusedMovieScene != nullptr && focusedMovieScene->FindMasterTrack<UMovieSceneSubTrack>() != nullptr )
    {
        return false;
    }

    if( sequence->GetMovieScene()->GetPlaybackRange().IsEmpty() )
    {
        FNotificationInfo info( FText::Format( LOCTEXT( "InvalidSequenceDuration", "Invalid level sequence {0}. The sequence has no duration." ), sequence->GetDisplayName() ) );
        info.bUseLargeFont = false;
        FSlateNotificationManager::Get().AddNotification( info );
        return false;
    }

    if( CanAddSubSequence( *sequence ) )
    {
        const FScopedTransaction transaction( LOCTEXT( "AddBoard_Transaction", "Add Board" ) );

        int32 rowIndex = INDEX_NONE;
        AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FCinematicBoardTrackEditor::HandleSequenceAdded, sequence, rowIndex ) );

        return true;
    }

    FNotificationInfo info( FText::Format( LOCTEXT( "InvalidSequence", "Invalid level sequence {0}. There could be a circular dependency." ), sequence->GetDisplayName() ) );
    info.bUseLargeFont = false;
    FSlateNotificationManager::Get().AddNotification( info );

    return false;
}


bool
FCinematicBoardTrackEditor::SupportsSequence( UMovieSceneSequence* iSequence ) const
{
    ETrackSupport TrackSupported = iSequence ? iSequence->IsTrackSupported( UMovieSceneCinematicBoardTrack::StaticClass() ) : ETrackSupport::NotSupported;
    return TrackSupported == ETrackSupport::Supported;
}


bool
FCinematicBoardTrackEditor::SupportsType( TSubclassOf<UMovieSceneTrack> iType ) const
{
    return ( iType == UMovieSceneCinematicBoardTrack::StaticClass() );
}


void
FCinematicBoardTrackEditor::Tick( float iDeltaTime )
{
    TSharedPtr<ISequencer> sequencerPin = GetSequencer();
    if( !sequencerPin.IsValid() )
    {
        return;
    }

    EMovieScenePlayerStatus::Type playbackState = sequencerPin->GetPlaybackStatus();

    if( FSlateThrottleManager::Get().IsAllowingExpensiveTasks() && playbackState != EMovieScenePlayerStatus::Playing && playbackState != EMovieScenePlayerStatus::Scrubbing )
    {
        sequencerPin->EnterSilentMode();

        FFrameTime savedTime = sequencerPin->GetGlobalTime().Time;

        if( iDeltaTime > 0.f && mThumbnailPool->DrawThumbnails() )
        {
            sequencerPin->SetGlobalTime( savedTime );
        }

        sequencerPin->ExitSilentMode();
    }
}


//void
//FCinematicBoardTrackEditor::BuildTrackContextMenu( FMenuBuilder& MenuBuilder, UMovieSceneTrack* Track )
//{
//    MenuBuilder.BeginSection( "Import/Export", NSLOCTEXT( "Sequencer", "ImportExportMenuSectionName", "Import/Export" ) );
//
//    MenuBuilder.AddMenuEntry(
//        NSLOCTEXT( "Sequencer", "ImportEDL", "Import EDL..." ),
//        NSLOCTEXT( "Sequencer", "ImportEDLTooltip", "Import Edit Decision List (EDL) for non-linear editors." ),
//        FSlateIcon(),
//        FUIAction(
//            FExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::ImportEDL ) ) );
//
//    MenuBuilder.AddMenuEntry(
//        NSLOCTEXT( "Sequencer", "ExportEDL", "Export EDL..." ),
//        NSLOCTEXT( "Sequencer", "ExportEDLTooltip", "Export Edit Decision List (EDL) for non-linear editors." ),
//        FSlateIcon(),
//        FUIAction(
//            FExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::ExportEDL ) ) );
//
//    MenuBuilder.AddMenuEntry(
//        NSLOCTEXT( "Sequencer", "ImportFCPXML", "Import Final Cut Pro 7 XML..." ),
//        NSLOCTEXT( "Sequencer", "ImportFCPXMLTooltip", "Import Final Cut Pro 7 XML file for non-linear editors." ),
//        FSlateIcon(),
//        FUIAction(
//            FExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::ImportFCPXML ) ) );
//
//    MenuBuilder.AddMenuEntry(
//        NSLOCTEXT( "Sequencer", "ExportFCPXML", "Export Final Cut Pro 7 XML..." ),
//        NSLOCTEXT( "Sequencer", "ExportFCPXMLTooltip", "Export Final Cut Pro 7 XML file for non-linear editors." ),
//        FSlateIcon(),
//        FUIAction(
//            FExecuteAction::CreateRaw( this, &FCinematicBoardTrackEditor::ExportFCPXML ) ) );
//
//    MenuBuilder.EndSection();
//}


const FSlateBrush*
FCinematicBoardTrackEditor::GetIconBrush() const
{
    return FEposEditorStyle::Get()->GetBrush( "Sequencer.Tracks.CinematicBoard" );
}

bool
FCinematicBoardTrackEditor::OnAllowDrop( const FDragDropEvent& iDragDropEvent, UMovieSceneTrack* iTrack, int32 iRowIndex, const FGuid& iTargetObjectGuid )
{
    if( !iTrack->IsA( UMovieSceneCinematicBoardTrack::StaticClass() ) )
    {
        return false;
    }

    TSharedPtr<FDragDropOperation> operation = iDragDropEvent.GetOperation();

    if( !operation.IsValid() || !operation->IsOfType<FAssetDragDropOp>() )
    {
        return false;
    }

    TSharedPtr<FAssetDragDropOp> dragDropOp = StaticCastSharedPtr<FAssetDragDropOp>( operation );

    for( const FAssetData& assetData : dragDropOp->GetAssets() )
    {
        if( Cast<UMovieSceneSequence>( assetData.GetAsset() ) )
        {
            return true;
        }
    }

    return false;
}


FReply
FCinematicBoardTrackEditor::OnDrop( const FDragDropEvent& iDragDropEvent, UMovieSceneTrack* iTrack, int32 iRowIndex, const FGuid& iTargetObjectGuid )
{
    if( !iTrack->IsA( UMovieSceneCinematicBoardTrack::StaticClass() ) )
    {
        return FReply::Unhandled();
    }

    TSharedPtr<FDragDropOperation> operation = iDragDropEvent.GetOperation();

    if( !operation.IsValid() || !operation->IsOfType<FAssetDragDropOp>() )
    {
        return FReply::Unhandled();
    }

    TSharedPtr<FAssetDragDropOp> dragDropOp = StaticCastSharedPtr<FAssetDragDropOp>( operation );

    bool anyDropped = false;
    for( const FAssetData& assetData : dragDropOp->GetAssets() )
    {
        UMovieSceneSequence* sequence = Cast<UMovieSceneSequence>( assetData.GetAsset() );

        if( sequence )
        {
            FGeometry geometry( GetSequencer()->GetTopTimeSliderWidget()->GetTickSpaceGeometry() );
            FTimeToPixel converter( geometry, GetSequencer()->GetViewRange(), GetSequencer()->GetFocusedTickResolution() );
            TOptional<FFrameNumber> dropped_frame( converter.PixelToFrame( geometry.AbsoluteToLocal( iDragDropEvent.GetScreenSpacePosition() ).X ).RoundToFrame() );

            AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FCinematicBoardTrackEditor::AddKeyInternal, sequence, iRowIndex, dropped_frame ) );

            anyDropped = true;
        }
    }

    return anyDropped ? FReply::Handled() : FReply::Unhandled();
}

//---

UMovieSceneSubSection*
FCinematicBoardTrackEditor::CreateBoardInternal( FString& ioNewBoardName, FFrameNumber iNewBoardStartTime, UMovieSceneCinematicBoardSection* iBoardToDuplicate )
{
    FString newBoardPath;

    if( iBoardToDuplicate != nullptr )
    {
        // If duplicating a board, use that board's path
        newBoardPath = FPaths::GetPath( iBoardToDuplicate->GetSequence()->GetPathName() );
    }
    else
    {
        newBoardPath = EposTracksEditorHelpers::GenerateNewBoardPath( GetSequencer()->GetFocusedMovieSceneSequence()->GetMovieScene(), ioNewBoardName );
    }

    // Create a new level sequence asset with the appropriate name
    IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" ).Get();

    UObject* newAsset = nullptr;
    for( TObjectIterator<UClass> It; It; ++It )
    {
        UClass* currentClass = *It;
        if( currentClass->IsChildOf( UFactory::StaticClass() ) && !( currentClass->HasAnyClassFlags( CLASS_Abstract ) ) )
        {
            UFactory* factory = Cast<UFactory>( currentClass->GetDefaultObject() );
            if( factory->CanCreateNew() && factory->ImportPriority >= 0 && ( factory->SupportedClass == UBoardSequence::StaticClass() || factory->SupportedClass == UShotSequence::StaticClass() ) )
            {
                if( iBoardToDuplicate != nullptr )
                {
                    newAsset = assetTools.DuplicateAssetWithDialog( ioNewBoardName, newBoardPath, iBoardToDuplicate->GetSequence() );
                }
                else
                {
                    if( factory->SupportedClass == UBoardSequence::StaticClass() )
                        newAsset = assetTools.CreateAssetWithDialog( ioNewBoardName, newBoardPath, UBoardSequence::StaticClass(), factory );
                    else
                        newAsset = assetTools.CreateAssetWithDialog( ioNewBoardName, newBoardPath, UShotSequence::StaticClass(), factory );

                }
                break;
            }
        }
    }

    if( newAsset == nullptr )
    {
        return nullptr;
    }

    UMovieSceneSequence* newSequence = Cast<UMovieSceneSequence>( newAsset );

    int32 duration = UE::MovieScene::DiscreteSize( iBoardToDuplicate ? iBoardToDuplicate->GetRange() : newSequence->GetMovieScene()->GetPlaybackRange() );

    UMovieSceneCinematicBoardTrack* boardTrack = FindOrCreateCinematicBoardTrack();

    // Create a board section. 
    UMovieSceneSubSection* newSection = boardTrack->AddSequence( newSequence, iNewBoardStartTime, duration );

    return newSection;
}

void
FCinematicBoardTrackEditor::InsertBoard()
{
    const FScopedTransaction transaction( LOCTEXT( "InsertBoard_Transaction", "Insert Board" ) );

    FFrameTime newBoardStartTime = GetSequencer()->GetLocalTime().Time;

    UMovieSceneCinematicBoardTrack* boardTrack = FindOrCreateCinematicBoardTrack();
    FString newBoardName = EposTracksEditorHelpers::GenerateNewBoardName( boardTrack->GetAllSections(), newBoardStartTime.FrameNumber );

    UMovieSceneSubSection* newBoard = CreateBoardInternal( newBoardName, newBoardStartTime.FrameNumber );
    if( newBoard )
    {
        newBoard->SetRowIndex( MovieSceneToolHelpers::FindAvailableRowIndex( boardTrack, newBoard ) );
    }

    GetSequencer()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    GetSequencer()->EmptySelection();
    GetSequencer()->SelectSection( newBoard );
    GetSequencer()->ThrobSectionSelection();
}


void
FCinematicBoardTrackEditor::InsertFiller()
{
    const UMovieSceneToolsProjectSettings* projectSettings = GetDefault<UMovieSceneToolsProjectSettings>();

    const FScopedTransaction transaction( LOCTEXT( "InsertFiller_Transaction", "Insert Filler" ) );

    FQualifiedFrameTime currentTime = GetSequencer()->GetLocalTime();

    UMovieSceneCinematicBoardTrack* boardTrack = FindOrCreateCinematicBoardTrack();

    int32 duration = ( projectSettings->DefaultDuration * currentTime.Rate ).FrameNumber.Value;

    UMovieSceneSequence* nullSequence = nullptr;

    UMovieSceneSubSection* newSection = boardTrack->AddSequence( nullSequence, currentTime.Time.FrameNumber, duration );

    UMovieSceneCinematicBoardSection* newBoardSection = Cast<UMovieSceneCinematicBoardSection>( newSection );

    newBoardSection->SetBoardDisplayName( FText( LOCTEXT( "Filler", "Filler" ) ).ToString() );

    GetSequencer()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    GetSequencer()->EmptySelection();
    GetSequencer()->SelectSection( newSection );
    GetSequencer()->ThrobSectionSelection();
}


void
FCinematicBoardTrackEditor::DuplicateBoard( UMovieSceneCinematicBoardSection* iSection )
{
    const FScopedTransaction transaction( LOCTEXT( "DuplicateBoard_Transaction", "Duplicate Board" ) );

    UMovieSceneCinematicBoardTrack* boardTrack = FindOrCreateCinematicBoardTrack();

    FFrameNumber startTime = iSection->HasStartFrame() ? iSection->GetInclusiveStartFrame() : 0;
    FString newBoardName = EposTracksEditorHelpers::GenerateNewBoardName( boardTrack->GetAllSections(), startTime );

    // Duplicate the board and put it on the next available row
    UMovieSceneSubSection* newBoard = CreateBoardInternal( newBoardName, startTime, iSection );
    if( newBoard )
    {
        newBoard->SetRange( iSection->GetRange() );
        newBoard->SetRowIndex( MovieSceneToolHelpers::FindAvailableRowIndex( boardTrack, newBoard ) );
        newBoard->Parameters.StartFrameOffset = iSection->Parameters.StartFrameOffset;
        newBoard->Parameters.TimeScale = iSection->Parameters.TimeScale;
        newBoard->SetPreRollFrames( iSection->GetPreRollFrames() );

        GetSequencer()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
        GetSequencer()->EmptySelection();
        GetSequencer()->SelectSection( newBoard );
        GetSequencer()->ThrobSectionSelection();
    }
}


void
FCinematicBoardTrackEditor::RenderBoard( UMovieSceneCinematicBoardSection* iSection )
{
    GetSequencer()->RenderMovie( iSection );
}


void
FCinematicBoardTrackEditor::RenameBoard( UMovieSceneCinematicBoardSection* iSection )
{
    //@todo
}


//void
//FCinematicBoardTrackEditor::NewTake( UMovieSceneCinematicBoardSection* Section )
//{
//    const FScopedTransaction Transaction( LOCTEXT( "NewTake_Transaction", "New Take" ) );
//
//    FString ShotPrefix;
//    uint32 ShotNumber = INDEX_NONE;
//    uint32 TakeNumber = INDEX_NONE;
//    if( MovieSceneToolHelpers::ParseShotName( Section->GetShotDisplayName(), ShotPrefix, ShotNumber, TakeNumber ) )
//    {
//        TArray<FAssetData> AssetData;
//        uint32 CurrentTakeNumber = INDEX_NONE;
//        MovieSceneToolHelpers::GatherTakes( Section, AssetData, CurrentTakeNumber );
//        uint32 NewTakeNumber = CurrentTakeNumber;
//
//        for( auto ThisAssetData : AssetData )
//        {
//            uint32 ThisTakeNumber = INDEX_NONE;
//            if( MovieSceneToolHelpers::GetTakeNumber( Section, ThisAssetData, ThisTakeNumber ) )
//            {
//                if( ThisTakeNumber >= NewTakeNumber )
//                {
//                    NewTakeNumber = ThisTakeNumber + 1;
//                }
//            }
//        }
//
//        FString NewShotName = MovieSceneToolHelpers::ComposeShotName( ShotPrefix, ShotNumber, NewTakeNumber );
//
//        TRange<FFrameNumber> NewShotRange = Section->GetRange();
//        FFrameNumber         NewShotStartOffset = Section->Parameters.StartFrameOffset;
//        float                NewShotTimeScale = Section->Parameters.TimeScale;
//        int32                NewShotPrerollFrames = Section->GetPreRollFrames();
//        int32                NewRowIndex = Section->GetRowIndex();
//        FFrameNumber         NewShotStartTime = NewShotRange.GetLowerBound().IsClosed() ? MovieScene::DiscreteInclusiveLower( NewShotRange ) : 0;
//
//        UMovieSceneSubSection* NewShot = CreateShotInternal( NewShotName, NewShotStartTime, Section );
//
//        if( NewShot )
//        {
//            UMovieSceneCinematicShotTrack* CinematicShotTrack = FindOrCreateCinematicBoardTrack();
//            CinematicShotTrack->RemoveSection( *Section );
//
//            NewShot->SetRange( NewShotRange );
//            NewShot->Parameters.StartFrameOffset = NewShotStartOffset;
//            NewShot->Parameters.TimeScale = NewShotTimeScale;
//            NewShot->SetPreRollFrames( NewShotPrerollFrames );
//            NewShot->SetRowIndex( NewRowIndex );
//
//            MovieSceneToolHelpers::SetTakeNumber( NewShot, NewTakeNumber );
//
//            GetSequencer()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );
//            GetSequencer()->EmptySelection();
//            GetSequencer()->SelectSection( NewShot );
//            GetSequencer()->ThrobSectionSelection();
//        }
//    }
//}
//
//
//void
//FCinematicBoardTrackEditor::SwitchTake( UObject* TakeObject )
//{
//    bool bSwitchedTake = false;
//
//    const FScopedTransaction Transaction( LOCTEXT( "SwitchTake_Transaction", "Switch Take" ) );
//
//    TArray<UMovieSceneSection*> Sections;
//    GetSequencer()->GetSelectedSections( Sections );
//
//    for( int32 SectionIndex = 0; SectionIndex < Sections.Num(); ++SectionIndex )
//    {
//        if( !Sections[SectionIndex]->IsA<UMovieSceneSubSection>() )
//        {
//            continue;
//        }
//
//        UMovieSceneSubSection* Section = Cast<UMovieSceneSubSection>( Sections[SectionIndex] );
//
//        if( TakeObject && TakeObject->IsA( UMovieSceneSequence::StaticClass() ) )
//        {
//            UMovieSceneSequence* MovieSceneSequence = CastChecked<UMovieSceneSequence>( TakeObject );
//
//            UMovieSceneCinematicShotTrack* CinematicShotTrack = CastChecked<UMovieSceneCinematicShotTrack>( Section->GetOuter() );
//
//            TRange<FFrameNumber> NewShotRange = Section->GetRange();
//            FFrameNumber         NewShotStartOffset = Section->Parameters.StartFrameOffset;
//            float                NewShotTimeScale = Section->Parameters.TimeScale;
//            int32                NewShotPrerollFrames = Section->GetPreRollFrames();
//            int32                NewRowIndex = Section->GetRowIndex();
//            FFrameNumber         NewShotStartTime = NewShotRange.GetLowerBound().IsClosed() ? MovieScene::DiscreteInclusiveLower( NewShotRange ) : 0;
//            int32                NewShotRowIndex = Section->GetRowIndex();
//
//            const int32 Duration = ( NewShotRange.GetLowerBound().IsClosed() && NewShotRange.GetUpperBound().IsClosed() ) ? MovieScene::DiscreteSize( NewShotRange ) : 1;
//            UMovieSceneSubSection* NewShot = CinematicShotTrack->AddSequence( MovieSceneSequence, NewShotStartTime, Duration );
//
//            if( NewShot != nullptr )
//            {
//                CinematicShotTrack->RemoveSection( *Section );
//
//                NewShot->SetRange( NewShotRange );
//                NewShot->Parameters.StartFrameOffset = NewShotStartOffset;
//                NewShot->Parameters.TimeScale = NewShotTimeScale;
//                NewShot->SetPreRollFrames( NewShotPrerollFrames );
//                NewShot->SetRowIndex( NewShotRowIndex );
//                bSwitchedTake = true;
//            }
//        }
//    }
//
//    if( bSwitchedTake )
//    {
//        GetSequencer()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );
//    }
//}


/* FCinematicBoardTrackEditor callbacks
 *****************************************************************************/

bool
FCinematicBoardTrackEditor::HandleAddCinematicBoardTrackMenuEntryCanExecute() const
{
    UMovieScene* focusedMovieScene = GetFocusedMovieScene();

    return ( ( focusedMovieScene != nullptr ) && ( focusedMovieScene->FindMasterTrack<UMovieSceneCinematicBoardTrack>() == nullptr ) );
}


void
FCinematicBoardTrackEditor::HandleAddCinematicBoardTrackMenuEntryExecute()
{
    UMovieSceneCinematicBoardTrack* boardTrack = FindOrCreateCinematicBoardTrack();
    if( boardTrack )
    {
        if( GetSequencer().IsValid() )
        {
            // Board Tracks can't be placed in folders, they're only allowed in the root.
            GetSequencer()->OnAddTrack( boardTrack, FGuid() );
        }
    }
}


TSharedRef<SWidget>
FCinematicBoardTrackEditor::HandleAddBoardComboButtonGetMenuContent()
{
    FMenuBuilder menuBuilder( true, nullptr );

    menuBuilder.AddMenuEntry(
        LOCTEXT( "InsertBoard", "Insert Board" ),
        LOCTEXT( "InsertBoardTooltip", "Insert new board at current time" ),
        FSlateIcon(),
        FUIAction( FExecuteAction::CreateSP( this, &FCinematicBoardTrackEditor::InsertBoard ) )
    );

    menuBuilder.AddMenuEntry(
        LOCTEXT( "InsertFiller", "Insert Filler" ),
        LOCTEXT( "InsertFillerTooltip", "Insert filler at current time" ),
        FSlateIcon(),
        FUIAction( FExecuteAction::CreateSP( this, &FCinematicBoardTrackEditor::InsertFiller ) )
    );

    FAssetPickerConfig assetPickerConfig;
    {
        assetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateRaw( this, &FCinematicBoardTrackEditor::HandleAddBoardComboButtonMenuEntryExecute );
        assetPickerConfig.OnAssetEnterPressed = FOnAssetEnterPressed::CreateRaw( this, &FCinematicBoardTrackEditor::HandleAddBoardComboButtonMenuEntryEnterPressed );
        assetPickerConfig.bAllowNullSelection = false;
        assetPickerConfig.InitialAssetViewType = EAssetViewType::Tile;
        assetPickerConfig.Filter.ClassNames.Add( TEXT( "BoardSequence" ) );
        assetPickerConfig.Filter.ClassNames.Add( TEXT( "ShotSequence" ) );
    }

    FContentBrowserModule& contentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>( TEXT( "ContentBrowser" ) );

    TSharedPtr<SBox> menuEntry = SNew( SBox )
        .WidthOverride( 300.0f )
        .HeightOverride( 300.f )
        [
            contentBrowserModule.Get().CreateAssetPicker( assetPickerConfig )
        ];

    menuBuilder.AddWidget( menuEntry.ToSharedRef(), FText::GetEmpty(), true );

    return menuBuilder.MakeWidget();
}


void
FCinematicBoardTrackEditor::HandleAddBoardComboButtonMenuEntryExecute( const FAssetData& iAssetData )
{
    FSlateApplication::Get().DismissAllMenus();

    UObject* selectedObject = iAssetData.GetAsset();

    if( selectedObject && selectedObject->IsA( UMovieSceneSequence::StaticClass() ) )
    {
        UMovieSceneSequence* movieSceneSequence = CastChecked<UMovieSceneSequence>( iAssetData.GetAsset() );

        int32 rowIndex = INDEX_NONE;
        TOptional<FFrameNumber> dropped_frame;
        AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FCinematicBoardTrackEditor::AddKeyInternal, movieSceneSequence, rowIndex, dropped_frame ) );
    }
}

void
FCinematicBoardTrackEditor::HandleAddBoardComboButtonMenuEntryEnterPressed( const TArray<FAssetData>& iAssetData )
{
    if( iAssetData.Num() > 0 )
    {
        HandleAddBoardComboButtonMenuEntryExecute( iAssetData[0].GetAsset() );
    }
}

FKeyPropertyResult
FCinematicBoardTrackEditor::AddKeyInternal( FFrameNumber iKeyTime, UMovieSceneSequence* iMovieSceneSequence, int32 iRowIndex, TOptional<FFrameNumber> iDroppedFrame )
{
    FKeyPropertyResult keyPropertyResult;

    if( !( ( iMovieSceneSequence->GetClass()->GetName() == TEXT( "BoardSequence" ) ) || ( iMovieSceneSequence->GetClass()->GetName() == TEXT( "ShotSequence" ) ) ) )
        return keyPropertyResult;

    if( iMovieSceneSequence->GetMovieScene()->GetPlaybackRange().IsEmpty() )
    {
        FNotificationInfo info( FText::Format( LOCTEXT( "InvalidSequenceDuration", "Invalid level sequence {0}. The sequence has no duration." ), iMovieSceneSequence->GetDisplayName() ) );
        info.bUseLargeFont = false;
        FSlateNotificationManager::Get().AddNotification( info );
        return keyPropertyResult;
    }

    if( CanAddSubSequence( *iMovieSceneSequence ) )
    {
        UMovieSceneCinematicBoardTrack* boardTrack = FindOrCreateCinematicBoardTrack();

        const FFrameRate tickResolution = iMovieSceneSequence->GetMovieScene()->GetTickResolution();
        const FQualifiedFrameTime innerDuration = FQualifiedFrameTime(
            UE::MovieScene::DiscreteSize( iMovieSceneSequence->GetMovieScene()->GetPlaybackRange() ),
            tickResolution );

        const FFrameRate outerFrameRate = boardTrack->GetTypedOuter<UMovieScene>()->GetTickResolution();
        const int32      outerDuration = innerDuration.ConvertTo( outerFrameRate ).FrameNumber.Value;

        UMovieSceneSubSection* newSection = boardTrack->AddSequenceOnRow( iMovieSceneSequence, iDroppedFrame.IsSet() ? iDroppedFrame.GetValue() : iKeyTime, outerDuration, iRowIndex );
        keyPropertyResult.bTrackModified = true;

        GetSequencer()->EmptySelection();
        GetSequencer()->SelectSection( newSection );
        GetSequencer()->ThrobSectionSelection();

        if( tickResolution != outerFrameRate )
        {
            FNotificationInfo info( FText::Format( LOCTEXT( "TickResolutionMismatch", "The parent sequence has a different tick resolution {0} than the newly added sequence {1}" ), outerFrameRate.ToPrettyText(), tickResolution.ToPrettyText() ) );
            info.bUseLargeFont = false;
            FSlateNotificationManager::Get().AddNotification( info );
        }

        return keyPropertyResult;
    }

    FNotificationInfo info( FText::Format( LOCTEXT( "InvalidSequence", "Invalid level sequence {0}. There could be a circular dependency." ), iMovieSceneSequence->GetDisplayName() ) );
    info.bUseLargeFont = false;
    FSlateNotificationManager::Get().AddNotification( info );

    return keyPropertyResult;
}


UMovieSceneCinematicBoardTrack*
FCinematicBoardTrackEditor::FindOrCreateCinematicBoardTrack()
{
    UMovieScene* focusedMovieScene = GetFocusedMovieScene();

    if( focusedMovieScene == nullptr )
    {
        return nullptr;
    }

    if( focusedMovieScene->IsReadOnly() )
    {
        return nullptr;
    }

    UMovieSceneCinematicBoardTrack* boardTrack = focusedMovieScene->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
    if( boardTrack != nullptr )
    {
        return boardTrack;
    }

    const FScopedTransaction transaction( LOCTEXT( "AddCinematicBoardTrack_Transaction", "Add Board Track" ) );
    focusedMovieScene->Modify();

    auto newTrack = focusedMovieScene->AddMasterTrack<UMovieSceneCinematicBoardTrack>();
    ensure( newTrack );

    GetSequencer()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );

    return newTrack;
}


ECheckBoxState
FCinematicBoardTrackEditor::AreBoardsLocked() const
{
    if( GetSequencer()->IsPerspectiveViewportCameraCutEnabled() )
    {
        return ECheckBoxState::Checked;
    }
    else
    {
        return ECheckBoxState::Unchecked;
    }
}


void
FCinematicBoardTrackEditor::OnLockBoardsClicked( ECheckBoxState iCheckBoxState )
{
    if( iCheckBoxState == ECheckBoxState::Checked )
    {
        for( FLevelEditorViewportClient* levelVC : GEditor->GetLevelViewportClients() )
        {
            if( levelVC && levelVC->AllowsCinematicControl() && levelVC->GetViewMode() != VMI_Unknown )
            {
                levelVC->SetActorLock( nullptr );
                levelVC->bLockedCameraView = false;
                levelVC->UpdateViewForLockedActor();
                levelVC->Invalidate();
            }
        }
        GetSequencer()->SetPerspectiveViewportCameraCutEnabled( true );
    }
    else
    {
        GetSequencer()->UpdateCameraCut( nullptr, EMovieSceneCameraCutParams() );
        GetSequencer()->SetPerspectiveViewportCameraCutEnabled( false );
    }

    GetSequencer()->ForceEvaluate();
}


FText
FCinematicBoardTrackEditor::GetLockBoardsToolTip() const
{
    return AreBoardsLocked() == ECheckBoxState::Checked ?
        LOCTEXT( "UnlockBoards", "Unlock Viewport from Boards" ) :
        LOCTEXT( "LockBoards", "Lock Viewport to Boards" );
}


bool
FCinematicBoardTrackEditor::CanAddSubSequence( const UMovieSceneSequence& iSequence ) const
{
    // prevent adding ourselves and ensure we have a valid movie scene
    UMovieSceneSequence* focusedSequence = GetSequencer()->GetFocusedMovieSceneSequence();

    if( ( focusedSequence == nullptr ) || ( focusedSequence == &iSequence ) || ( focusedSequence->GetMovieScene() == nullptr ) )
    {
        return false;
    }

    // ensure that the other sequence has a valid movie scene
    UMovieScene* sequenceMovieScene = iSequence.GetMovieScene();

    if( sequenceMovieScene == nullptr )
    {
        return false;
    }

    // make sure we are not contained in the other sequence (circular dependency)
    // @todo sequencer: this check is not sufficient (does not prevent circular dependencies of 2+ levels)
    UMovieSceneSubTrack* sequenceSubTrack = sequenceMovieScene->FindMasterTrack<UMovieSceneSubTrack>();
    if( sequenceSubTrack && sequenceSubTrack->ContainsSequence( *focusedSequence, true ) )
    {
        return false;
    }

    UMovieSceneCinematicBoardTrack* boardTrack = sequenceMovieScene->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
    if( boardTrack && boardTrack->ContainsSequence( *focusedSequence, true ) )
    {
        return false;
    }

    return true;
}


void
FCinematicBoardTrackEditor::OnUpdateCameraCut( UObject* iCameraObject, bool iJumpCut )
{
    // Keep track of the camera when it switches so that the thumbnail can be drawn with the correct camera
    mBoardCamera = Cast<AActor>( iCameraObject );
}


FKeyPropertyResult
FCinematicBoardTrackEditor::HandleSequenceAdded( FFrameNumber iKeyTime, UMovieSceneSequence* iSequence, int32 iRowIndex )
{
    FKeyPropertyResult keyPropertyResult;

    auto boardTrack = FindOrCreateCinematicBoardTrack();

    const FFrameRate tickResolution = iSequence->GetMovieScene()->GetTickResolution();
    const FQualifiedFrameTime innerDuration = FQualifiedFrameTime(
        UE::MovieScene::DiscreteSize( iSequence->GetMovieScene()->GetPlaybackRange() ),
        tickResolution );

    const FFrameRate outerFrameRate = boardTrack->GetTypedOuter<UMovieScene>()->GetTickResolution();
    const int32      outerDuration = innerDuration.ConvertTo( outerFrameRate ).FrameNumber.Value;

    UMovieSceneSubSection* newSection = boardTrack->AddSequenceOnRow( iSequence, iKeyTime, outerDuration, iRowIndex );
    keyPropertyResult.bTrackModified = true;

    GetSequencer()->EmptySelection();
    GetSequencer()->SelectSection( newSection );
    GetSequencer()->ThrobSectionSelection();

    if( tickResolution != outerFrameRate )
    {
        FNotificationInfo info( FText::Format( LOCTEXT( "TickResolutionMismatch", "The parent sequence has a different tick resolution {0} than the newly added sequence {1}" ), outerFrameRate.ToPrettyText(), tickResolution.ToPrettyText() ) );
        info.bUseLargeFont = false;
        FSlateNotificationManager::Get().AddNotification( info );
    }

    return keyPropertyResult;
}

#undef LOCTEXT_NAMESPACE
