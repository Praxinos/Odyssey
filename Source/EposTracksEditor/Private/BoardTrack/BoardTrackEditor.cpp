// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "BoardTrack/BoardTrackEditor.h"

#include "Misc/Paths.h"
#include "Widgets/SBoxPanel.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GameFramework/Actor.h"
#include "Factories/Factory.h"
#include "Tracks/MovieSceneSubTrack.h"
#include "BoardTrack/MovieSceneBoardTrack.h"
#include "BoardTrack/MovieSceneBoardSection.h"
#include "Modules/ModuleManager.h"
#include "Application/ThrottleManager.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "EditorStyleSet.h"
#include "LevelEditorViewport.h"
#include "MovieSceneToolHelpers.h"
#include "FCPXML/FCPXMLMovieSceneTranslator.h"
#include "BoardTrack/BoardSection.h"
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
#include "EposTracksEditorHelpers.h"
#include "Shot/ShotSequence.h"
#include "Styles/EposEditorStyle.h"

#define LOCTEXT_NAMESPACE "FBoardTrackEditor"

/* FBoardTrackEditor structors
 *****************************************************************************/

FBoardTrackEditor::FBoardTrackEditor( TSharedRef<ISequencer> InSequencer )
    : FMovieSceneTrackEditor( InSequencer )
{
    ThumbnailPool = MakeShareable( new FTrackEditorThumbnailPool( InSequencer ) );
}


TSharedRef<ISequencerTrackEditor>
FBoardTrackEditor::CreateTrackEditor( TSharedRef<ISequencer> InSequencer )
{
    return MakeShareable( new FBoardTrackEditor( InSequencer ) );
}


void
FBoardTrackEditor::OnInitialize()
{
    OnCameraCutHandle = GetSequencer()->OnCameraCut().AddSP( this, &FBoardTrackEditor::OnUpdateCameraCut );
}


void
FBoardTrackEditor::OnRelease()
{
    if( OnCameraCutHandle.IsValid() && GetSequencer().IsValid() )
    {
        GetSequencer()->OnCameraCut().Remove( OnCameraCutHandle );
    }
}


/* ISequencerTrackEditor interface
 *****************************************************************************/

void
FBoardTrackEditor::BuildAddTrackMenu( FMenuBuilder& MenuBuilder )
{
    MenuBuilder.AddMenuEntry(
        LOCTEXT( "AddBoardTrack", "Board Track" ),
        LOCTEXT( "AddBoardTooltip", "Adds a board track." ),
        FSlateIcon( FEposEditorStyle::Get()->GetStyleSetName(), "Sequencer.Tracks.Board" ),
        FUIAction(
            FExecuteAction::CreateRaw( this, &FBoardTrackEditor::HandleAddBoardTrackMenuEntryExecute ),
            FCanExecuteAction::CreateRaw( this, &FBoardTrackEditor::HandleAddBoardTrackMenuEntryCanExecute )
        )
    );
}


TSharedPtr<SWidget>
FBoardTrackEditor::BuildOutlinerEditWidget( const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params )
{
    // Create a container edit box
    return SNew( SHorizontalBox )

        // Add the camera combo box
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign( VAlign_Center )
        [
            FSequencerUtilities::MakeAddButton( LOCTEXT( "BoardText", "Board" ), FOnGetContent::CreateSP( this, &FBoardTrackEditor::HandleAddBoardComboButtonGetMenuContent ), Params.NodeIsHovered, GetSequencer() )
        ]

    + SHorizontalBox::Slot()
        .VAlign( VAlign_Center )
        .HAlign( HAlign_Right )
        .AutoWidth()
        .Padding( 4, 0, 0, 0 )
        [
            SNew( SCheckBox )
            .IsFocusable( false )
        .IsChecked( this, &FBoardTrackEditor::AreBoardsLocked )
        .OnCheckStateChanged( this, &FBoardTrackEditor::OnLockBoardsClicked )
        .ToolTipText( this, &FBoardTrackEditor::GetLockBoardsToolTip )
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
FBoardTrackEditor::MakeSectionInterface( UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding )
{
    check( SupportsType( SectionObject.GetOuter()->GetClass() ) );

    UMovieSceneBoardSection& SectionObjectImpl = *CastChecked<UMovieSceneBoardSection>( &SectionObject );
    return MakeShareable( new FBoardSection( GetSequencer(), SectionObjectImpl, SharedThis( this ), ThumbnailPool ) );
}


bool
FBoardTrackEditor::HandleAssetAdded( UObject* Asset, const FGuid& TargetObjectGuid )
{
    UMovieSceneSequence* Sequence = Cast<UMovieSceneSequence>( Asset );

    if( Sequence == nullptr )
    {
        return false;
    }

    if( !SupportsSequence( Sequence ) )
    {
        return false;
    }

    //@todo If there's already a subscenes track, allow that track to handle this asset
    UMovieScene* FocusedMovieScene = GetFocusedMovieScene();

    if( FocusedMovieScene != nullptr && FocusedMovieScene->FindMasterTrack<UMovieSceneSubTrack>() != nullptr )
    {
        return false;
    }

    if( Sequence->GetMovieScene()->GetPlaybackRange().IsEmpty() )
    {
        FNotificationInfo Info( FText::Format( LOCTEXT( "InvalidSequenceDuration", "Invalid level sequence {0}. The sequence has no duration." ), Sequence->GetDisplayName() ) );
        Info.bUseLargeFont = false;
        FSlateNotificationManager::Get().AddNotification( Info );
        return false;
    }

    if( CanAddSubSequence( *Sequence ) )
    {
        const FScopedTransaction Transaction( LOCTEXT( "AddBoard_Transaction", "Add Board" ) );

        int32 RowIndex = INDEX_NONE;
        AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FBoardTrackEditor::HandleSequenceAdded, Sequence, RowIndex ) );

        return true;
    }

    FNotificationInfo Info( FText::Format( LOCTEXT( "InvalidSequence", "Invalid level sequence {0}. There could be a circular dependency." ), Sequence->GetDisplayName() ) );
    Info.bUseLargeFont = false;
    FSlateNotificationManager::Get().AddNotification( Info );

    return false;
}


bool
FBoardTrackEditor::SupportsSequence( UMovieSceneSequence* InSequence ) const
{
    return ( InSequence != nullptr ) && ( ( InSequence->GetClass()->GetName() == TEXT( "BoardSequence" ) ) || ( InSequence->GetClass()->GetName() == TEXT( "ShotSequence" ) ) );
}


bool
FBoardTrackEditor::SupportsType( TSubclassOf<UMovieSceneTrack> Type ) const
{
    return ( Type == UMovieSceneBoardTrack::StaticClass() );
}


void
FBoardTrackEditor::Tick( float DeltaTime )
{
    TSharedPtr<ISequencer> SequencerPin = GetSequencer();
    if( !SequencerPin.IsValid() )
    {
        return;
    }

    EMovieScenePlayerStatus::Type PlaybackState = SequencerPin->GetPlaybackStatus();

    if( FSlateThrottleManager::Get().IsAllowingExpensiveTasks() && PlaybackState != EMovieScenePlayerStatus::Playing && PlaybackState != EMovieScenePlayerStatus::Scrubbing )
    {
        SequencerPin->EnterSilentMode();

        FFrameTime SavedTime = SequencerPin->GetGlobalTime().Time;

        if( DeltaTime > 0.f && ThumbnailPool->DrawThumbnails() )
        {
            SequencerPin->SetGlobalTime( SavedTime );
        }

        SequencerPin->ExitSilentMode();
    }
}


//void
//FBoardTrackEditor::BuildTrackContextMenu( FMenuBuilder& MenuBuilder, UMovieSceneTrack* Track )
//{
//    MenuBuilder.BeginSection( "Import/Export", NSLOCTEXT( "Sequencer", "ImportExportMenuSectionName", "Import/Export" ) );
//
//    MenuBuilder.AddMenuEntry(
//        NSLOCTEXT( "Sequencer", "ImportEDL", "Import EDL..." ),
//        NSLOCTEXT( "Sequencer", "ImportEDLTooltip", "Import Edit Decision List (EDL) for non-linear editors." ),
//        FSlateIcon(),
//        FUIAction(
//            FExecuteAction::CreateRaw( this, &FBoardTrackEditor::ImportEDL ) ) );
//
//    MenuBuilder.AddMenuEntry(
//        NSLOCTEXT( "Sequencer", "ExportEDL", "Export EDL..." ),
//        NSLOCTEXT( "Sequencer", "ExportEDLTooltip", "Export Edit Decision List (EDL) for non-linear editors." ),
//        FSlateIcon(),
//        FUIAction(
//            FExecuteAction::CreateRaw( this, &FBoardTrackEditor::ExportEDL ) ) );
//
//    MenuBuilder.AddMenuEntry(
//        NSLOCTEXT( "Sequencer", "ImportFCPXML", "Import Final Cut Pro 7 XML..." ),
//        NSLOCTEXT( "Sequencer", "ImportFCPXMLTooltip", "Import Final Cut Pro 7 XML file for non-linear editors." ),
//        FSlateIcon(),
//        FUIAction(
//            FExecuteAction::CreateRaw( this, &FBoardTrackEditor::ImportFCPXML ) ) );
//
//    MenuBuilder.AddMenuEntry(
//        NSLOCTEXT( "Sequencer", "ExportFCPXML", "Export Final Cut Pro 7 XML..." ),
//        NSLOCTEXT( "Sequencer", "ExportFCPXMLTooltip", "Export Final Cut Pro 7 XML file for non-linear editors." ),
//        FSlateIcon(),
//        FUIAction(
//            FExecuteAction::CreateRaw( this, &FBoardTrackEditor::ExportFCPXML ) ) );
//
//    MenuBuilder.EndSection();
//}


const FSlateBrush*
FBoardTrackEditor::GetIconBrush() const
{
    return FEposEditorStyle::Get()->GetBrush( "Sequencer.Tracks.Board" );
}

bool
FBoardTrackEditor::OnAllowDrop( const FDragDropEvent& DragDropEvent, UMovieSceneTrack* Track, int32 RowIndex, const FGuid& TargetObjectGuid )
{
    if( !Track->IsA( UMovieSceneBoardTrack::StaticClass() ) )
    {
        return false;
    }

    TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();

    if( !Operation.IsValid() || !Operation->IsOfType<FAssetDragDropOp>() )
    {
        return false;
    }

    TSharedPtr<FAssetDragDropOp> DragDropOp = StaticCastSharedPtr<FAssetDragDropOp>( Operation );

    for( const FAssetData& AssetData : DragDropOp->GetAssets() )
    {
        if( Cast<UMovieSceneSequence>( AssetData.GetAsset() ) )
        {
            return true;
        }
    }

    return false;
}


FReply
FBoardTrackEditor::OnDrop( const FDragDropEvent& DragDropEvent, UMovieSceneTrack* Track, int32 RowIndex, const FGuid& TargetObjectGuid )
{
    if( !Track->IsA( UMovieSceneBoardTrack::StaticClass() ) )
    {
        return FReply::Unhandled();
    }

    TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();

    if( !Operation.IsValid() || !Operation->IsOfType<FAssetDragDropOp>() )
    {
        return FReply::Unhandled();
    }

    TSharedPtr<FAssetDragDropOp> DragDropOp = StaticCastSharedPtr<FAssetDragDropOp>( Operation );

    bool bAnyDropped = false;
    for( const FAssetData& AssetData : DragDropOp->GetAssets() )
    {
        UMovieSceneSequence* Sequence = Cast<UMovieSceneSequence>( AssetData.GetAsset() );

        if( Sequence )
        {
            AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FBoardTrackEditor::AddKeyInternal, Sequence, RowIndex ) );

            bAnyDropped = true;
        }
    }

    return bAnyDropped ? FReply::Handled() : FReply::Unhandled();
}

UMovieSceneSubSection*
FBoardTrackEditor::CreateBoardInternal( FString& NewBoardName, FFrameNumber NewBoardStartTime, UMovieSceneBoardSection* BoardToDuplicate )
{
    FString NewBoardPath;

    if( BoardToDuplicate != nullptr )
    {
        // If duplicating a board, use that board's path
        NewBoardPath = FPaths::GetPath( BoardToDuplicate->GetSequence()->GetPathName() );
    }
    else
    {
        NewBoardPath = EposTracksEditorHelpers::GenerateNewBoardPath( GetSequencer()->GetFocusedMovieSceneSequence()->GetMovieScene(), NewBoardName );
    }

    // Create a new level sequence asset with the appropriate name
    IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" ).Get();

    UObject* NewAsset = nullptr;
    for( TObjectIterator<UClass> It; It; ++It )
    {
        UClass* CurrentClass = *It;
        if( CurrentClass->IsChildOf( UFactory::StaticClass() ) && !( CurrentClass->HasAnyClassFlags( CLASS_Abstract ) ) )
        {
            UFactory* Factory = Cast<UFactory>( CurrentClass->GetDefaultObject() );
            if( Factory->CanCreateNew() && Factory->ImportPriority >= 0 && ( Factory->SupportedClass == UBoardSequence::StaticClass() || Factory->SupportedClass == UShotSequence::StaticClass() ) )
            {
                if( BoardToDuplicate != nullptr )
                {
                    NewAsset = AssetTools.DuplicateAssetWithDialog( NewBoardName, NewBoardPath, BoardToDuplicate->GetSequence() );
                }
                else
                {
                    if( Factory->SupportedClass == UBoardSequence::StaticClass() )
                        NewAsset = AssetTools.CreateAssetWithDialog( NewBoardName, NewBoardPath, UBoardSequence::StaticClass(), Factory );
                    else
                        NewAsset = AssetTools.CreateAssetWithDialog( NewBoardName, NewBoardPath, UShotSequence::StaticClass(), Factory );

                }
                break;
            }
        }
    }

    if( NewAsset == nullptr )
    {
        return nullptr;
    }

    UMovieSceneSequence* NewSequence = Cast<UMovieSceneSequence>( NewAsset );

    int32 Duration = MovieScene::DiscreteSize( BoardToDuplicate ? BoardToDuplicate->GetRange() : NewSequence->GetMovieScene()->GetPlaybackRange() );

    UMovieSceneBoardTrack* BoardTrack = FindOrCreateBoardTrack();

    // Create a board section. 
    UMovieSceneSubSection* NewSection = BoardTrack->AddSequence( NewSequence, NewBoardStartTime, Duration );
    return NewSection;
}

void
FBoardTrackEditor::InsertBoard()
{
    const FScopedTransaction Transaction( LOCTEXT( "InsertBoard_Transaction", "Insert Board" ) );

    FFrameTime NewBoardStartTime = GetSequencer()->GetLocalTime().Time;

    UMovieSceneBoardTrack* BoardTrack = FindOrCreateBoardTrack();
    FString NewBoardName = EposTracksEditorHelpers::GenerateNewBoardName( BoardTrack->GetAllSections(), NewBoardStartTime.FrameNumber );

    UMovieSceneSubSection* NewBoard = CreateBoardInternal( NewBoardName, NewBoardStartTime.FrameNumber );
    if( NewBoard )
    {
        NewBoard->SetRowIndex( MovieSceneToolHelpers::FindAvailableRowIndex( BoardTrack, NewBoard ) );
    }

    GetSequencer()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    GetSequencer()->EmptySelection();
    GetSequencer()->SelectSection( NewBoard );
    GetSequencer()->ThrobSectionSelection();
}


void
FBoardTrackEditor::InsertFiller()
{
    const UMovieSceneToolsProjectSettings* ProjectSettings = GetDefault<UMovieSceneToolsProjectSettings>();

    const FScopedTransaction Transaction( LOCTEXT( "InsertFiller_Transaction", "Insert Filler" ) );

    FQualifiedFrameTime CurrentTime = GetSequencer()->GetLocalTime();

    UMovieSceneBoardTrack* BoardTrack = FindOrCreateBoardTrack();

    int32 Duration = ( ProjectSettings->DefaultDuration * CurrentTime.Rate ).FrameNumber.Value;

    UMovieSceneSequence* NullSequence = nullptr;

    UMovieSceneSubSection* NewSection = BoardTrack->AddSequence( NullSequence, CurrentTime.Time.FrameNumber, Duration );

    UMovieSceneBoardSection* NewBoardSection = Cast<UMovieSceneBoardSection>( NewSection );

    NewBoardSection->SetBoardDisplayName( FText( LOCTEXT( "Filler", "Filler" ) ).ToString() );

    GetSequencer()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    GetSequencer()->EmptySelection();
    GetSequencer()->SelectSection( NewSection );
    GetSequencer()->ThrobSectionSelection();
}


void
FBoardTrackEditor::DuplicateBoard( UMovieSceneBoardSection* Section )
{
    const FScopedTransaction Transaction( LOCTEXT( "DuplicateBoard_Transaction", "Duplicate Board" ) );

    UMovieSceneBoardTrack* BoardTrack = FindOrCreateBoardTrack();

    FFrameNumber StartTime = Section->HasStartFrame() ? Section->GetInclusiveStartFrame() : 0;
    FString NewBoardName = EposTracksEditorHelpers::GenerateNewBoardName( BoardTrack->GetAllSections(), StartTime );

    // Duplicate the board and put it on the next available row
    UMovieSceneSubSection* NewBoard = CreateBoardInternal( NewBoardName, StartTime, Section );
    if( NewBoard )
    {
        NewBoard->SetRange( Section->GetRange() );
        NewBoard->SetRowIndex( MovieSceneToolHelpers::FindAvailableRowIndex( BoardTrack, NewBoard ) );
        NewBoard->Parameters.StartFrameOffset = Section->Parameters.StartFrameOffset;
        NewBoard->Parameters.TimeScale = Section->Parameters.TimeScale;
        NewBoard->SetPreRollFrames( Section->GetPreRollFrames() );

        GetSequencer()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
        GetSequencer()->EmptySelection();
        GetSequencer()->SelectSection( NewBoard );
        GetSequencer()->ThrobSectionSelection();
    }
}


void
FBoardTrackEditor::RenderBoard( UMovieSceneBoardSection* Section )
{
    GetSequencer()->RenderMovie( Section );
}


void
FBoardTrackEditor::RenameBoard( UMovieSceneBoardSection* Section )
{
    //@todo
}


//void
//FBoardTrackEditor::NewTake( UMovieSceneBoardSection* Section )
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
//            UMovieSceneCinematicShotTrack* CinematicShotTrack = FindOrCreateBoardTrack();
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
//FBoardTrackEditor::SwitchTake( UObject* TakeObject )
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


/* FBoardTrackEditor callbacks
 *****************************************************************************/

bool
FBoardTrackEditor::HandleAddBoardTrackMenuEntryCanExecute() const
{
    UMovieScene* FocusedMovieScene = GetFocusedMovieScene();

    return ( ( FocusedMovieScene != nullptr ) && ( FocusedMovieScene->FindMasterTrack<UMovieSceneBoardTrack>() == nullptr ) );
}


void
FBoardTrackEditor::HandleAddBoardTrackMenuEntryExecute()
{
    UMovieSceneBoardTrack* BoardTrack = FindOrCreateBoardTrack();
    if( BoardTrack )
    {
        if( GetSequencer().IsValid() )
        {
            // Board Tracks can't be placed in folders, they're only allowed in the root.
            GetSequencer()->OnAddTrack( BoardTrack, FGuid() );
        }
    }
}


TSharedRef<SWidget>
FBoardTrackEditor::HandleAddBoardComboButtonGetMenuContent()
{
    FMenuBuilder MenuBuilder( true, nullptr );

    MenuBuilder.AddMenuEntry(
        LOCTEXT( "InsertBoard", "Insert Board" ),
        LOCTEXT( "InsertBoardTooltip", "Insert new board at current time" ),
        FSlateIcon(),
        FUIAction( FExecuteAction::CreateSP( this, &FBoardTrackEditor::InsertBoard ) )
    );

    MenuBuilder.AddMenuEntry(
        LOCTEXT( "InsertFiller", "Insert Filler" ),
        LOCTEXT( "InsertFillerTooltip", "Insert filler at current time" ),
        FSlateIcon(),
        FUIAction( FExecuteAction::CreateSP( this, &FBoardTrackEditor::InsertFiller ) )
    );

    FAssetPickerConfig AssetPickerConfig;
    {
        AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateRaw( this, &FBoardTrackEditor::HandleAddBoardComboButtonMenuEntryExecute );
        AssetPickerConfig.OnAssetEnterPressed = FOnAssetEnterPressed::CreateRaw( this, &FBoardTrackEditor::HandleAddBoardComboButtonMenuEntryEnterPressed );
        AssetPickerConfig.bAllowNullSelection = false;
        AssetPickerConfig.InitialAssetViewType = EAssetViewType::Tile;
        AssetPickerConfig.Filter.ClassNames.Add( TEXT( "BoardSequence" ) );
        AssetPickerConfig.Filter.ClassNames.Add( TEXT( "ShotSequence" ) );
    }

    FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>( TEXT( "ContentBrowser" ) );

    TSharedPtr<SBox> MenuEntry = SNew( SBox )
        .WidthOverride( 300.0f )
        .HeightOverride( 300.f )
        [
            ContentBrowserModule.Get().CreateAssetPicker( AssetPickerConfig )
        ];

    MenuBuilder.AddWidget( MenuEntry.ToSharedRef(), FText::GetEmpty(), true );

    return MenuBuilder.MakeWidget();
}


void
FBoardTrackEditor::HandleAddBoardComboButtonMenuEntryExecute( const FAssetData& AssetData )
{
    FSlateApplication::Get().DismissAllMenus();

    UObject* SelectedObject = AssetData.GetAsset();

    if( SelectedObject && SelectedObject->IsA( UMovieSceneSequence::StaticClass() ) )
    {
        UMovieSceneSequence* MovieSceneSequence = CastChecked<UMovieSceneSequence>( AssetData.GetAsset() );

        int32 RowIndex = INDEX_NONE;
        AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FBoardTrackEditor::AddKeyInternal, MovieSceneSequence, RowIndex ) );
    }
}

void
FBoardTrackEditor::HandleAddBoardComboButtonMenuEntryEnterPressed( const TArray<FAssetData>& AssetData )
{
    if( AssetData.Num() > 0 )
    {
        HandleAddBoardComboButtonMenuEntryExecute( AssetData[0].GetAsset() );
    }
}

FKeyPropertyResult
FBoardTrackEditor::AddKeyInternal( FFrameNumber KeyTime, UMovieSceneSequence* InMovieSceneSequence, int32 RowIndex )
{
    FKeyPropertyResult KeyPropertyResult;

    if( InMovieSceneSequence->GetMovieScene()->GetPlaybackRange().IsEmpty() )
    {
        FNotificationInfo Info( FText::Format( LOCTEXT( "InvalidSequenceDuration", "Invalid level sequence {0}. The sequence has no duration." ), InMovieSceneSequence->GetDisplayName() ) );
        Info.bUseLargeFont = false;
        FSlateNotificationManager::Get().AddNotification( Info );
        return KeyPropertyResult;
    }

    if( CanAddSubSequence( *InMovieSceneSequence ) )
    {
        UMovieSceneBoardTrack* BoardTrack = FindOrCreateBoardTrack();

        const FFrameRate TickResolution = InMovieSceneSequence->GetMovieScene()->GetTickResolution();
        const FQualifiedFrameTime InnerDuration = FQualifiedFrameTime(
            MovieScene::DiscreteSize( InMovieSceneSequence->GetMovieScene()->GetPlaybackRange() ),
            TickResolution );

        const FFrameRate OuterFrameRate = BoardTrack->GetTypedOuter<UMovieScene>()->GetTickResolution();
        const int32      OuterDuration = InnerDuration.ConvertTo( OuterFrameRate ).FrameNumber.Value;

        UMovieSceneSubSection* NewSection = BoardTrack->AddSequenceOnRow( InMovieSceneSequence, KeyTime, OuterDuration, RowIndex );
        KeyPropertyResult.bTrackModified = true;

        GetSequencer()->EmptySelection();
        GetSequencer()->SelectSection( NewSection );
        GetSequencer()->ThrobSectionSelection();

        if( TickResolution != OuterFrameRate )
        {
            FNotificationInfo Info( FText::Format( LOCTEXT( "TickResolutionMismatch", "The parent sequence has a different tick resolution {0} than the newly added sequence {1}" ), OuterFrameRate.ToPrettyText(), TickResolution.ToPrettyText() ) );
            Info.bUseLargeFont = false;
            FSlateNotificationManager::Get().AddNotification( Info );
        }

        return KeyPropertyResult;
    }

    FNotificationInfo Info( FText::Format( LOCTEXT( "InvalidSequence", "Invalid level sequence {0}. There could be a circular dependency." ), InMovieSceneSequence->GetDisplayName() ) );
    Info.bUseLargeFont = false;
    FSlateNotificationManager::Get().AddNotification( Info );

    return KeyPropertyResult;
}


UMovieSceneBoardTrack*
FBoardTrackEditor::FindOrCreateBoardTrack()
{
    UMovieScene* FocusedMovieScene = GetFocusedMovieScene();

    if( FocusedMovieScene == nullptr )
    {
        return nullptr;
    }

    if( FocusedMovieScene->IsReadOnly() )
    {
        return nullptr;
    }

    UMovieSceneBoardTrack* BoardTrack = FocusedMovieScene->FindMasterTrack<UMovieSceneBoardTrack>();
    if( BoardTrack != nullptr )
    {
        return BoardTrack;
    }

    const FScopedTransaction Transaction( LOCTEXT( "AddBoardTrack_Transaction", "Add Board Track" ) );
    FocusedMovieScene->Modify();

    auto NewTrack = FocusedMovieScene->AddMasterTrack<UMovieSceneBoardTrack>();
    ensure( NewTrack );

    GetSequencer()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );

    return NewTrack;
}


ECheckBoxState
FBoardTrackEditor::AreBoardsLocked() const
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
FBoardTrackEditor::OnLockBoardsClicked( ECheckBoxState CheckBoxState )
{
    if( CheckBoxState == ECheckBoxState::Checked )
    {
        for( FLevelEditorViewportClient* LevelVC : GEditor->GetLevelViewportClients() )
        {
            if( LevelVC && LevelVC->AllowsCinematicControl() && LevelVC->GetViewMode() != VMI_Unknown )
            {
                LevelVC->SetActorLock( nullptr );
                LevelVC->bLockedCameraView = false;
                LevelVC->UpdateViewForLockedActor();
                LevelVC->Invalidate();
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
FBoardTrackEditor::GetLockBoardsToolTip() const
{
    return AreBoardsLocked() == ECheckBoxState::Checked ?
        LOCTEXT( "UnlockBoards", "Unlock Viewport from Boards" ) :
        LOCTEXT( "LockBoards", "Lock Viewport to Boards" );
}


bool
FBoardTrackEditor::CanAddSubSequence( const UMovieSceneSequence& Sequence ) const
{
    // prevent adding ourselves and ensure we have a valid movie scene
    UMovieSceneSequence* FocusedSequence = GetSequencer()->GetFocusedMovieSceneSequence();

    if( ( FocusedSequence == nullptr ) || ( FocusedSequence == &Sequence ) || ( FocusedSequence->GetMovieScene() == nullptr ) )
    {
        return false;
    }

    // ensure that the other sequence has a valid movie scene
    UMovieScene* SequenceMovieScene = Sequence.GetMovieScene();

    if( SequenceMovieScene == nullptr )
    {
        return false;
    }

    // make sure we are not contained in the other sequence (circular dependency)
    // @todo sequencer: this check is not sufficient (does not prevent circular dependencies of 2+ levels)
    UMovieSceneSubTrack* SequenceSubTrack = SequenceMovieScene->FindMasterTrack<UMovieSceneSubTrack>();
    if( SequenceSubTrack && SequenceSubTrack->ContainsSequence( *FocusedSequence, true ) )
    {
        return false;
    }

    UMovieSceneBoardTrack* BoardTrack = SequenceMovieScene->FindMasterTrack<UMovieSceneBoardTrack>();
    if( BoardTrack && BoardTrack->ContainsSequence( *FocusedSequence, true ) )
    {
        return false;
    }

    return true;
}


void
FBoardTrackEditor::OnUpdateCameraCut( UObject* CameraObject, bool bJumpCut )
{
    // Keep track of the camera when it switches so that the thumbnail can be drawn with the correct camera
    BoardCamera = Cast<AActor>( CameraObject );
}


FKeyPropertyResult
FBoardTrackEditor::HandleSequenceAdded( FFrameNumber KeyTime, UMovieSceneSequence* Sequence, int32 RowIndex )
{
    FKeyPropertyResult KeyPropertyResult;

    auto BoardTrack = FindOrCreateBoardTrack();

    const FFrameRate TickResolution = Sequence->GetMovieScene()->GetTickResolution();
    const FQualifiedFrameTime InnerDuration = FQualifiedFrameTime(
        MovieScene::DiscreteSize( Sequence->GetMovieScene()->GetPlaybackRange() ),
        TickResolution );

    const FFrameRate OuterFrameRate = BoardTrack->GetTypedOuter<UMovieScene>()->GetTickResolution();
    const int32      OuterDuration = InnerDuration.ConvertTo( OuterFrameRate ).FrameNumber.Value;

    UMovieSceneSubSection* NewSection = BoardTrack->AddSequenceOnRow( Sequence, KeyTime, OuterDuration, RowIndex );
    KeyPropertyResult.bTrackModified = true;

    GetSequencer()->EmptySelection();
    GetSequencer()->SelectSection( NewSection );
    GetSequencer()->ThrobSectionSelection();

    if( TickResolution != OuterFrameRate )
    {
        FNotificationInfo Info( FText::Format( LOCTEXT( "TickResolutionMismatch", "The parent sequence has a different tick resolution {0} than the newly added sequence {1}" ), OuterFrameRate.ToPrettyText(), TickResolution.ToPrettyText() ) );
        Info.bUseLargeFont = false;
        FSlateNotificationManager::Get().AddNotification( Info );
    }

    return KeyPropertyResult;
}

#undef LOCTEXT_NAMESPACE
