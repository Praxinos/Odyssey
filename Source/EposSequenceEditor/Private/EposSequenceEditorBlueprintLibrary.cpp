// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "EposSequenceEditorBlueprintLibrary.h"

#include "Channels/MovieSceneChannelProxy.h"
//For custom colors on channels, stored in editor pref's
#include "CurveEditorSettings.h"
#include "IKeyArea.h"
#include "ISequencer.h"
#include "Modules/ModuleManager.h"
#include "MovieSceneCommonHelpers.h"
#include "MovieSceneSequencePlayer.h"
#include "MovieSceneSection.h"
#include "MovieSceneTimeHelpers.h"
#include "LevelEditor.h"
#include "LevelEditorViewport.h"
#include "Sections/MovieSceneSubSection.h"
#include "Subsystems/AssetEditorSubsystem.h"

#include "Board/BoardSequence.h"
#include "Export/ExportConverter.h"
#include "Export/ImageSequence/ExportImageSequenceExporter.h"
#include "Export/PDF/ExportPDFExporter.h"
#include "Tools/EposSequenceTools.h"
#include "Tools/LighttableTools.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EposSequenceEditorBlueprintLibrary)

//---

namespace
{
    static TWeakPtr<ISequencer> CurrentSequencer;
}

//---

//static
UBoardSequence*
UBoardSequenceEditorBlueprintLibrary::GetRootBoardSequence()
{
    if( !CurrentSequencer.IsValid() )
        return nullptr;

    return Cast<UBoardSequence>(CurrentSequencer.Pin()->GetRootMovieSceneSequence());
}


//static
UBoardSequence*
UBoardSequenceEditorBlueprintLibrary::GetFocusedBoardSequence()
{
    if( !CurrentSequencer.IsValid() )
        return nullptr;

    return Cast<UBoardSequence>(CurrentSequencer.Pin()->GetFocusedMovieSceneSequence());
}

//static
UMovieSceneSubSection*
UBoardSequenceEditorBlueprintLibrary::InsertBoardSequence( int32 iStartFrame, int32 iEndFrame )
{
    if( !CurrentSequencer.IsValid() )
        return nullptr;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    FFrameRate DisplayRate = sequencer->GetFocusedDisplayRate();
    FFrameRate TickResolution = sequencer->GetFocusedTickResolution();
    TRange<FFrameNumber> range( ConvertFrameTime( iStartFrame, DisplayRate, TickResolution ).GetFrame(), ConvertFrameTime( iEndFrame, DisplayRate, TickResolution ).GetFrame() );
    int32 length = UE::MovieScene::DiscreteSize( range );

    return CinematicBoardTrackTools::InsertBoard( sequencer, range.GetLowerBoundValue(), length );
}

//static
UMovieSceneSubSection*
UBoardSequenceEditorBlueprintLibrary::InsertShotSequence( int32 iStartFrame, int32 iEndFrame )
{
    if( !CurrentSequencer.IsValid() )
        return nullptr;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    FFrameRate DisplayRate = sequencer->GetFocusedDisplayRate();
    FFrameRate TickResolution = sequencer->GetFocusedTickResolution();
    TRange<FFrameNumber> range( ConvertFrameTime( iStartFrame, DisplayRate, TickResolution ).GetFrame(), ConvertFrameTime( iEndFrame, DisplayRate, TickResolution ).GetFrame() );
    int32 length = UE::MovieScene::DiscreteSize( range );

    return CinematicBoardTrackTools::InsertShot( sequencer, range.GetLowerBoundValue(), length );
}

//static
UMovieSceneSubSection*
UBoardSequenceEditorBlueprintLibrary::CloneSection( UMovieSceneSubSection* iSubSection, int32 iFrameNumber )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    if( !CurrentSequencer.IsValid() )
        return nullptr;

    if( !board_section )
        return nullptr;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    FFrameRate DisplayRate = sequencer->GetFocusedDisplayRate();
    FFrameRate TickResolution = sequencer->GetFocusedTickResolution();
    FFrameNumber frame_in_tick = ConvertFrameTime( iFrameNumber, DisplayRate, TickResolution ).GetFrame();

    return CinematicBoardTrackTools::CloneSection( sequencer, board_section, frame_in_tick, false );
}

//-

//static
void
UBoardSequenceEditorBlueprintLibrary::CreateCamera( UMovieSceneSubSection* iSubSection )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    if( !CurrentSequencer.IsValid() )
        return;

    if( !board_section )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    FCameraArgs camera_args;
    FPlaneArgs plane_args;
    BoardSequenceTools::CreateCamera( sequencer, *board_section, board_section->GetTrueRange().GetLowerBoundValue() );
}

//static
void
UBoardSequenceEditorBlueprintLibrary::CreatePlane( UMovieSceneSubSection* iSubSection )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    if( !CurrentSequencer.IsValid() )
        return;

    if( !board_section )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    FPlaneArgs plane_args;
    BoardSequenceTools::CreatePlane( sequencer, *board_section, board_section->GetTrueRange().GetLowerBoundValue() );
}

//static
void
UBoardSequenceEditorBlueprintLibrary::ActivateLighttable( UMovieSceneSubSection* iSubSection, const FMovieSceneBindingProxy& iBinding )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    if( !CurrentSequencer.IsValid() )
        return;

    if( !board_section )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    LighttableTools::Activate( sequencer, *board_section, iBinding.BindingID );
}

//static
void
UBoardSequenceEditorBlueprintLibrary::DeactivateLighttable( UMovieSceneSubSection* iSubSection, const FMovieSceneBindingProxy& iBinding )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    if( !CurrentSequencer.IsValid() )
        return;

    if( !board_section )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    LighttableTools::Deactivate( sequencer, *board_section, iBinding.BindingID );
}

//static
int32
UBoardSequenceEditorBlueprintLibrary::GetLighttableState( UMovieSceneSubSection* iSubSection, const FMovieSceneBindingProxy& iBinding )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    if( !CurrentSequencer.IsValid() )
        return -1;

    if( !board_section )
        return -1;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    return LighttableTools::GetState( sequencer, *board_section, iBinding.BindingID );
}

//static
void
UBoardSequenceEditorBlueprintLibrary::CreateDrawing( UMovieSceneSubSection* iSubSection, const FMovieSceneBindingProxy& iBinding, int32 iFrame )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    if( !CurrentSequencer.IsValid() )
        return;

    if( !board_section )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    FFrameRate DisplayRate = sequencer->GetFocusedDisplayRate();
    FFrameRate TickResolution = sequencer->GetFocusedTickResolution();
    FFrameNumber frame_in_tick = ConvertFrameTime( iFrame, DisplayRate, TickResolution ).GetFrame();

    FPlaneArgs plane_args;
    BoardSequenceTools::CreateDrawing( sequencer, *board_section, frame_in_tick, iBinding.BindingID );
}

//static
void
UBoardSequenceEditorBlueprintLibrary::RenameBinding( UMovieSceneSubSection* iSubSection, const FMovieSceneBindingProxy& iBinding, FString iNewLabel )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    if( !CurrentSequencer.IsValid() )
        return;

    if( !board_section )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    BoardSequenceTools::RenameBinding( sequencer, *board_section, iBinding.BindingID, iNewLabel );
}

//-

//static
FBoardSectionTake
UBoardSequenceEditorBlueprintLibrary::CreateTake( UMovieSceneSubSection* iSubSection )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    if( !CurrentSequencer.IsValid() )
        return FBoardSectionTake();

    if( !board_section )
        return FBoardSectionTake();

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    FBoardSectionTake* take = BoardSequenceTools::CreateTake( sequencer, *board_section );

    return take ? *take : FBoardSectionTake();
}

//static
FBoardSectionTake
UBoardSequenceEditorBlueprintLibrary::SwitchTake( UMovieSceneSubSection* iSubSection, FBoardSectionTake iTake )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    if( !CurrentSequencer.IsValid() )
        return FBoardSectionTake();

    if( !board_section )
        return FBoardSectionTake();

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    FBoardSectionTake* take = BoardSequenceTools::SwitchTake( sequencer, *board_section, &iTake );

    return take ? *take : FBoardSectionTake();
}

//static
TArray<FBoardSectionTake>
UBoardSequenceEditorBlueprintLibrary::GetTakes( UMovieSceneSubSection* iSubSection )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    return board_section ? board_section->GetTakes() : TArray<FBoardSectionTake>();
}

//static
FBoardSectionTake
UBoardSequenceEditorBlueprintLibrary::GetCurrentTake( UMovieSceneSubSection* iSubSection )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    if( !board_section )
        return FBoardSectionTake();

    FBoardSectionTake* take = board_section->FindTake( board_section->GetSequence() );

    return take ? *take : FBoardSectionTake();
}

//-

//static
bool
UBoardSequenceEditorBlueprintLibrary::IsValid( FBoardSectionTake iTake )
{
    return iTake.GetSequence().IsValid();
}

//static
FText
UBoardSequenceEditorBlueprintLibrary::GetDisplayName( FBoardSectionTake iTake )
{
    return iTake.GetSequence().IsValid() ? iTake.GetSequence()->GetDisplayName() : FText::GetEmpty();
}

//static
bool
UBoardSequenceEditorBlueprintLibrary::EqualEqual_BoardSectionTakeBoardSectionTake( FBoardSectionTake iA, FBoardSectionTake iB )
{
    return iA == iB;
}

//---

//static
UShotSequence*
UShotSequenceEditorBlueprintLibrary::GetRootShotSequence()
{
    if( !CurrentSequencer.IsValid() )
        return nullptr;

    return Cast<UShotSequence>(CurrentSequencer.Pin()->GetRootMovieSceneSequence());
}

//static
UShotSequence*
UShotSequenceEditorBlueprintLibrary::GetFocusedShotSequence()
{
    if( !CurrentSequencer.IsValid() )
        return nullptr;

    return Cast<UShotSequence>(CurrentSequencer.Pin()->GetFocusedMovieSceneSequence());
}

//static
void
UShotSequenceEditorBlueprintLibrary::StepToNextShot()
{
    if( !CurrentSequencer.IsValid() )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    ShotSequenceTools::StepToNextShot( sequencer );
}

//static
void
UShotSequenceEditorBlueprintLibrary::StepToPreviousShot()
{
    if( !CurrentSequencer.IsValid() )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    ShotSequenceTools::StepToPreviousShot( sequencer );
}

//-

//static
void
UShotSequenceEditorBlueprintLibrary::CreateCamera()
{
    if( !CurrentSequencer.IsValid() )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    FCameraArgs camera_args;
    FPlaneArgs plane_args;
    ShotSequenceTools::CreateCamera( sequencer );
}

//static
void
UShotSequenceEditorBlueprintLibrary::CreatePlane()
{
    if( !CurrentSequencer.IsValid() )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    FPlaneArgs plane_args;
    ShotSequenceTools::CreatePlane( sequencer, 0 );
}

//static
void
UShotSequenceEditorBlueprintLibrary::ActivateLighttable( const FMovieSceneBindingProxy& iBinding )
{
    if( !CurrentSequencer.IsValid() )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    LighttableTools::Activate( sequencer, iBinding.BindingID );
}

//static
void
UShotSequenceEditorBlueprintLibrary::DeactivateLighttable( const FMovieSceneBindingProxy& iBinding )
{
    if( !CurrentSequencer.IsValid() )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    LighttableTools::Deactivate( sequencer, iBinding.BindingID );
}

//static
int32
UShotSequenceEditorBlueprintLibrary::GetLighttableState( const FMovieSceneBindingProxy& iBinding )
{
    if( !CurrentSequencer.IsValid() )
        return -1;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    return LighttableTools::GetState( sequencer, iBinding.BindingID );
}

//static
void
UShotSequenceEditorBlueprintLibrary::CreateDrawing( const FMovieSceneBindingProxy& iBinding, int32 iFrame )
{
    if( !CurrentSequencer.IsValid() )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    FFrameRate DisplayRate = sequencer->GetFocusedDisplayRate();
    FFrameRate TickResolution = sequencer->GetFocusedTickResolution();
    FFrameNumber frame_in_tick = ConvertFrameTime( iFrame, DisplayRate, TickResolution ).GetFrame();

    FPlaneArgs plane_args;
    ShotSequenceTools::CreateDrawing( sequencer, frame_in_tick, iBinding.BindingID );
}

//static
void
UShotSequenceEditorBlueprintLibrary::RenameBinding( const FMovieSceneBindingProxy& iBinding, FString iNewLabel )
{
    if( !CurrentSequencer.IsValid() )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    ShotSequenceTools::RenameBinding( sequencer, iBinding.BindingID, iNewLabel );
}

//---

bool UEposSequenceEditorBlueprintLibrary::OpenEposSequence( UEposMovieSceneSequence* iBoardSequence )
{
    if( iBoardSequence )
    {
        return GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset( iBoardSequence );
    }

    return false;
}

UEposMovieSceneSequence* UEposSequenceEditorBlueprintLibrary::GetRootEposSequence()
{
    if (CurrentSequencer.IsValid())
    {
        return Cast<UEposMovieSceneSequence>(CurrentSequencer.Pin()->GetRootMovieSceneSequence());
    }
    return nullptr;
}

UEposMovieSceneSequence* UEposSequenceEditorBlueprintLibrary::GetFocusedEposSequence()
{
    if (CurrentSequencer.IsValid())
    {
        return Cast<UEposMovieSceneSequence>(CurrentSequencer.Pin()->GetFocusedMovieSceneSequence());
    }
    return nullptr;
}

void UEposSequenceEditorBlueprintLibrary::FocusEposSequence( UMovieSceneSubSection* SubSection )
{
    if( CurrentSequencer.IsValid() && IsValid( SubSection ) )
    {
        CurrentSequencer.Pin()->FocusSequenceInstance( *SubSection );
    }
}

void UEposSequenceEditorBlueprintLibrary::FocusParentSequence()
{
    if( CurrentSequencer.IsValid() )
    {
        const TArray<FMovieSceneSequenceID>& Hierarchy = CurrentSequencer.Pin()->GetSubSequenceHierarchy();

        if( Hierarchy.Num() > 1 )
        {
            CurrentSequencer.Pin()->PopToSequenceInstance( Hierarchy[Hierarchy.Num() - 2] );
        }
    }
}

TArray<UMovieSceneSubSection*> UEposSequenceEditorBlueprintLibrary::GetSubSequenceHierarchy()
{
    TArray<UMovieSceneSubSection*> SectionsHierarchy;

    if( CurrentSequencer.IsValid() )
    {
        const TArray<FMovieSceneSequenceID>& Hierarchy = CurrentSequencer.Pin()->GetSubSequenceHierarchy();

        for( const FMovieSceneSequenceID Item : Hierarchy )
        {
            if( Item != MovieSceneSequenceID::Root )
            {
                // We return the whole hierarchy anyways, leaving it to the user to check for invalid pointers.
                SectionsHierarchy.Add( CurrentSequencer.Pin()->FindSubSection( Item ) );
            }
        }
    }
    return SectionsHierarchy;
}

void UEposSequenceEditorBlueprintLibrary::CloseEposSequence()
{
    if (CurrentSequencer.IsValid())
    {
        GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseAllEditorsForAsset(CurrentSequencer.Pin()->GetRootMovieSceneSequence());
    }
}

//---

//static
void
UEposSequenceEditorBlueprintLibrary::MoveAndScalePlane( APlaneActor* ioPlane, const ACineCameraActor* iCamera, float iNewDistance, EScalePlane iScaleType )
{
    if( !ioPlane || !iCamera )
        return;

    ShotSequenceTools::MoveAndScalePlane( ioPlane, iCamera, iNewDistance, iScaleType );
}

//static
void
UEposSequenceEditorBlueprintLibrary::SetCameraFocalLengthAndScalePlane( TArray<APlaneActor*> ioPlanes, ACineCameraActor* ioCamera, float iNewFocalLength, EScalePlane iScaleType )
{
    if( !ioCamera )
        return;

    TArray<TWeakObjectPtr<APlaneActor>> planes( ioPlanes );

    ShotSequenceTools::SetCameraFocalLengthAndScalePlane( planes, ioCamera, iNewFocalLength, iScaleType );
}

//---

//static
bool
UEposSequenceEditorBlueprintLibrary::ExportAsPDF( const FExportPDFOptions& iOptions )
{
    if( !CurrentSequencer.IsValid() )
        return false;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    FExportStruct image_sequence_struct;
    FExportConverter converter( CurrentSequencer, sequencer->GetRootMovieSceneSequence(), &iOptions.MarkSettings, &image_sequence_struct );

    FExportPDFExporter exporter( CurrentSequencer, &image_sequence_struct, &iOptions );
    return exporter.Export();
}

//static
bool
UEposSequenceEditorBlueprintLibrary::ExportAsImageSequence( const FExportImageSequenceOptions& iOptions )
{
    if( !CurrentSequencer.IsValid() )
        return false;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    FExportStruct image_sequence_struct;
    FExportConverter converter( CurrentSequencer, sequencer->GetRootMovieSceneSequence(), &iOptions.MarkSettings, &image_sequence_struct );

    image_sequence_struct.mSequencer = CurrentSequencer;

    FExportImageSequenceExporter exporter( CurrentSequencer, &image_sequence_struct, &iOptions );
    return exporter.Export();
}

//---

void UEposSequenceEditorBlueprintLibrary::Play()
{
    const bool bTogglePlay = false;
    if (CurrentSequencer.IsValid())
    {
        CurrentSequencer.Pin()->OnPlay(bTogglePlay);
    }
}

void UEposSequenceEditorBlueprintLibrary::Pause()
{
    if (CurrentSequencer.IsValid())
    {
        CurrentSequencer.Pin()->Pause();
    }
}

void UEposSequenceEditorBlueprintLibrary::SetCurrentTime(int32 NewFrame)
{
    if (CurrentSequencer.IsValid())
    {
        FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
        FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

        CurrentSequencer.Pin()->SetGlobalTime(ConvertFrameTime(NewFrame, DisplayRate, TickResolution));
    }
}

int32 UEposSequenceEditorBlueprintLibrary::GetCurrentTime()
{
    if (CurrentSequencer.IsValid())
    {
        FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
        FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

        return ConvertFrameTime(CurrentSequencer.Pin()->GetGlobalTime().Time, TickResolution, DisplayRate).FloorToFrame().Value;
    }
    return 0;
}

void UEposSequenceEditorBlueprintLibrary::SetCurrentLocalTime(int32 NewFrame)
{
    if (CurrentSequencer.IsValid())
    {
        FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
        FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

        CurrentSequencer.Pin()->SetLocalTime(ConvertFrameTime(NewFrame, DisplayRate, TickResolution));
    }
}

int32 UEposSequenceEditorBlueprintLibrary::GetCurrentLocalTime()
{
    if (CurrentSequencer.IsValid())
    {
        FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
        FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

        return ConvertFrameTime(CurrentSequencer.Pin()->GetLocalTime().Time, TickResolution, DisplayRate).FloorToFrame().Value;
    }
    return 0;
}

void UEposSequenceEditorBlueprintLibrary::PlayTo(FMovieSceneSequencePlaybackParams PlaybackParams)
{
    if (CurrentSequencer.IsValid())
    {
        CurrentSequencer.Pin()->PlayTo(PlaybackParams);
    }
}

bool UEposSequenceEditorBlueprintLibrary::IsPlaying()
{
    if (CurrentSequencer.IsValid())
    {
        return CurrentSequencer.Pin()->GetPlaybackStatus() == EMovieScenePlayerStatus::Playing;
    }
    return false;
}

TArray<UMovieSceneTrack*> UEposSequenceEditorBlueprintLibrary::GetSelectedTracks()
{
    TArray<UMovieSceneTrack*> OutSelectedTracks;
    if (CurrentSequencer.IsValid())
    {
        CurrentSequencer.Pin()->GetSelectedTracks(OutSelectedTracks);
    }
    return OutSelectedTracks;
}

TArray<UMovieSceneSection*> UEposSequenceEditorBlueprintLibrary::GetSelectedSections()
{
    TArray<UMovieSceneSection*> OutSelectedSections;
    if (CurrentSequencer.IsValid())
    {
        CurrentSequencer.Pin()->GetSelectedSections(OutSelectedSections);
    }
    return OutSelectedSections;
}

//TArray<FSequencerChannelProxy> UEposSequenceEditorBlueprintLibrary::GetSelectedChannels()
//{
//    TArray<FSequencerChannelProxy> OutSelectedChannels;
//    if (CurrentSequencer.IsValid())
//    {
//        TArray<const IKeyArea*> SelectedKeyAreas;
//
//        CurrentSequencer.Pin()->GetSelectedKeyAreas(SelectedKeyAreas);
//
//        for (const IKeyArea* KeyArea : SelectedKeyAreas)
//        {
//            if (KeyArea)
//            {
//                FSequencerChannelProxy ChannelProxy(KeyArea->GetName(), KeyArea->GetOwningSection());
//                OutSelectedChannels.Add(ChannelProxy);
//            }
//        }
//    }
//    return OutSelectedChannels;
//}

TArray<UMovieSceneFolder*> UEposSequenceEditorBlueprintLibrary::GetSelectedFolders()
{
    TArray<UMovieSceneFolder*> OutSelectedFolders;
    if (CurrentSequencer.IsValid())
    {
        CurrentSequencer.Pin()->GetSelectedFolders(OutSelectedFolders);
    }
    return OutSelectedFolders;
}

TArray<FMovieSceneBindingProxy> UEposSequenceEditorBlueprintLibrary::GetSelectedBindings()
{
    TArray<FMovieSceneBindingProxy> OutSelectedBindings;
    if( CurrentSequencer.IsValid() )
    {
        TArray<FGuid> OutSelectedGuids;
        CurrentSequencer.Pin()->GetSelectedObjects( OutSelectedGuids );

        UMovieSceneSequence* Sequence = CurrentSequencer.Pin()->GetFocusedMovieSceneSequence();

        for( const FGuid& SelectedGuid : OutSelectedGuids )
        {
            OutSelectedBindings.Add( FMovieSceneBindingProxy( SelectedGuid, Sequence ) );
        }
    }
    return OutSelectedBindings;
}

void UEposSequenceEditorBlueprintLibrary::SelectTracks(const TArray<UMovieSceneTrack*>& Tracks)
{
    if (CurrentSequencer.IsValid())
    {
        for (UMovieSceneTrack* Track : Tracks)
        {
            CurrentSequencer.Pin()->SelectTrack(Track);
        }
    }
}

void UEposSequenceEditorBlueprintLibrary::SelectSections(const TArray<UMovieSceneSection*>& Sections)
{
    if (CurrentSequencer.IsValid())
    {
        for (UMovieSceneSection* Section : Sections)
        {
            CurrentSequencer.Pin()->SelectSection(Section);
        }
    }
}

//void UEposSequenceEditorBlueprintLibrary::SelectChannels(const TArray<FSequencerChannelProxy>& Channels)
//{
//    if (CurrentSequencer.IsValid())
//    {
//        for (FSequencerChannelProxy ChannelProxy : Channels)
//        {
//            UMovieSceneSection* Section = ChannelProxy.Section;
//            if (Section)
//            {
//                TArray<FName> ChannelNames;
//                ChannelNames.Add(ChannelProxy.ChannelName);
//                CurrentSequencer.Pin()->SelectByChannels(Section, ChannelNames, false, true);
//            }
//        }
//    }
//}

void UEposSequenceEditorBlueprintLibrary::SelectFolders(const TArray<UMovieSceneFolder*>& Folders)
{
    if (CurrentSequencer.IsValid())
    {
        for (UMovieSceneFolder* Folder : Folders)
        {
            CurrentSequencer.Pin()->SelectFolder(Folder);
        }
    }
}

void UEposSequenceEditorBlueprintLibrary::SelectBindings( const TArray<FMovieSceneBindingProxy>& ObjectBindings )
{
    if( CurrentSequencer.IsValid() )
    {
        for( const FMovieSceneBindingProxy& ObjectBinding : ObjectBindings )
        {
            CurrentSequencer.Pin()->SelectObject( ObjectBinding.BindingID );
        }
    }
}

void UEposSequenceEditorBlueprintLibrary::EmptySelection()
{
    if (CurrentSequencer.IsValid())
    {
        CurrentSequencer.Pin()->EmptySelection();
    }
}

void UEposSequenceEditorBlueprintLibrary::SetSelectionRangeStart(int32 NewFrame)
{
    if (CurrentSequencer.IsValid())
    {
        FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
        FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

        CurrentSequencer.Pin()->SetSelectionRangeStart(ConvertFrameTime(NewFrame, DisplayRate, TickResolution));
    }
}

void UEposSequenceEditorBlueprintLibrary::SetSelectionRangeEnd(int32 NewFrame)
{
    if (CurrentSequencer.IsValid())
    {
        FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
        FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

        CurrentSequencer.Pin()->SetSelectionRangeEnd(ConvertFrameTime(NewFrame, DisplayRate, TickResolution));
    }
}

int32 UEposSequenceEditorBlueprintLibrary::GetSelectionRangeStart()
{
    if (CurrentSequencer.IsValid())
    {
        FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
        FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

        return ConvertFrameTime(CurrentSequencer.Pin()->GetSelectionRange().GetLowerBoundValue(), TickResolution, DisplayRate).FloorToFrame().Value;
    }

    return 0;
}

int32 UEposSequenceEditorBlueprintLibrary::GetSelectionRangeEnd()
{
    if (CurrentSequencer.IsValid())
    {
        FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
        FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

        return ConvertFrameTime(CurrentSequencer.Pin()->GetSelectionRange().GetUpperBoundValue(), TickResolution, DisplayRate).FloorToFrame().Value;
    }

    return 0;
}

void UEposSequenceEditorBlueprintLibrary::SetSequencer(TSharedRef<ISequencer> InSequencer)
{
    CurrentSequencer = TWeakPtr<ISequencer>(InSequencer);
}

void UEposSequenceEditorBlueprintLibrary::RefreshCurrentEposSequence()
{
    if (CurrentSequencer.IsValid())
    {
        CurrentSequencer.Pin()->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::Unknown);
    }
}

TArray<UObject*> UEposSequenceEditorBlueprintLibrary::GetBoundObjects(FMovieSceneObjectBindingID ObjectBinding)
{
    TArray<UObject*> BoundObjects;
    if (CurrentSequencer.IsValid())
    {
        for (TWeakObjectPtr<> WeakObject : ObjectBinding.ResolveBoundObjects(CurrentSequencer.Pin()->GetFocusedTemplateID(), *CurrentSequencer.Pin()))
        {
            if (WeakObject.IsValid())
            {
                BoundObjects.Add(WeakObject.Get());
            }
        }

    }
    return BoundObjects;
}


bool UEposSequenceEditorBlueprintLibrary::IsEposSequenceLocked()
{
    if (CurrentSequencer.IsValid())
    {
        TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();
        UMovieSceneSequence* FocusedMovieSceneSequence = Sequencer->GetFocusedMovieSceneSequence();
        if (FocusedMovieSceneSequence)
        {
            if (FocusedMovieSceneSequence->GetMovieScene()->IsReadOnly())
            {
                return true;
            }
            else
            {
                TArray<UMovieScene*> DescendantMovieScenes;
                MovieSceneHelpers::GetDescendantMovieScenes(Sequencer->GetFocusedMovieSceneSequence(), DescendantMovieScenes);

                for (UMovieScene* DescendantMovieScene : DescendantMovieScenes)
                {
                    if (DescendantMovieScene && DescendantMovieScene->IsReadOnly())
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void UEposSequenceEditorBlueprintLibrary::SetLockEposSequence(bool bLock)
{
    if (CurrentSequencer.IsValid())
    {
        TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();

        if (Sequencer->GetFocusedMovieSceneSequence())
        {
            UMovieScene* MovieScene = Sequencer->GetFocusedMovieSceneSequence()->GetMovieScene();

            if (bLock != MovieScene->IsReadOnly())
            {
                MovieScene->Modify();
                MovieScene->SetReadOnly(bLock);
            }

            TArray<UMovieScene*> DescendantMovieScenes;
            MovieSceneHelpers::GetDescendantMovieScenes(Sequencer->GetFocusedMovieSceneSequence(), DescendantMovieScenes);

            for (UMovieScene* DescendantMovieScene : DescendantMovieScenes)
            {
                if (DescendantMovieScene && bLock != DescendantMovieScene->IsReadOnly())
                {
                    DescendantMovieScene->Modify();
                    DescendantMovieScene->SetReadOnly(bLock);
                }
            }

            Sequencer->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::Unknown);
        }
    }
}

bool UEposSequenceEditorBlueprintLibrary::IsTrackFilterEnabled(const FText& TrackFilterName)
{
    if (CurrentSequencer.IsValid())
    {
        TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();

        return Sequencer->IsTrackFilterEnabled(TrackFilterName);
    }
    return false;
}

void UEposSequenceEditorBlueprintLibrary::SetTrackFilterEnabled(const FText& TrackFilterName, bool bEnabled)
{
    if (CurrentSequencer.IsValid())
    {
        TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();

        Sequencer->SetTrackFilterEnabled(TrackFilterName, bEnabled);
    }
}

TArray<FText> UEposSequenceEditorBlueprintLibrary::GetTrackFilterNames()
{
    if (CurrentSequencer.IsValid())
    {
        TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();

        return Sequencer->GetTrackFilterNames();
    }

    return TArray<FText>();
}

bool UEposSequenceEditorBlueprintLibrary::HasCustomColorForChannel(UClass* Class, const FString& Identifier)
{
    const UCurveEditorSettings* Settings = GetDefault<UCurveEditorSettings>();
    if (Settings)
    {
        TOptional<FLinearColor> OptColor = Settings->GetCustomColor(Class, Identifier);
        return OptColor.IsSet();
    }
    return false;
}

FLinearColor UEposSequenceEditorBlueprintLibrary::GetCustomColorForChannel(UClass* Class, const FString& Identifier)
{
    FLinearColor Color(FColor::White);
    const UCurveEditorSettings* Settings = GetDefault<UCurveEditorSettings>();
    if (Settings)
    {
        TOptional<FLinearColor> OptColor = Settings->GetCustomColor(Class, Identifier);
        if (OptColor.IsSet())
        {
            return OptColor.GetValue();
        }
    }
    return Color;
}

void UEposSequenceEditorBlueprintLibrary::SetCustomColorForChannel(UClass* Class, const FString& Identifier, const FLinearColor& NewColor)
{
    UCurveEditorSettings* Settings = GetMutableDefault<UCurveEditorSettings>();
    if (Settings)
    {
        Settings->SetCustomColor(Class, Identifier, NewColor);
    }
}

void UEposSequenceEditorBlueprintLibrary::SetCustomColorForChannels(UClass* Class, const TArray<FString>& Identifiers, const TArray<FLinearColor>& NewColors)
{
    if (Identifiers.Num() != NewColors.Num())
    {
        return;
    }
    UCurveEditorSettings* Settings = GetMutableDefault<UCurveEditorSettings>();
    if (Settings)
    {
        for (int32 Index = 0; Index < Identifiers.Num(); ++Index)
        {
            const FString& Identifier = Identifiers[Index];
            const FLinearColor& NewColor = NewColors[Index];
            Settings->SetCustomColor(Class, Identifier, NewColor);
        }
    }
}

void UEposSequenceEditorBlueprintLibrary::DeleteColorForChannels(UClass* Class, FString& Identifier)
{
    UCurveEditorSettings* Settings = GetMutableDefault<UCurveEditorSettings>();
    if (Settings)
    {
        Settings->DeleteCustomColor(Class, Identifier);
    }
}

void UEposSequenceEditorBlueprintLibrary::SetRandomColorForChannels(UClass* Class, const TArray<FString>& Identifiers)
{
    UCurveEditorSettings* Settings = GetMutableDefault<UCurveEditorSettings>();
    if (Settings)
    {
        for (int32 Index = 0; Index < Identifiers.Num(); ++Index)
        {
            const FString& Identifier = Identifiers[Index];
            FLinearColor NewColor = UCurveEditorSettings::GetNextRandomColor();
            Settings->SetCustomColor(Class, Identifier, NewColor);
        }
    }
}

bool UEposSequenceEditorBlueprintLibrary::IsCameraCutLockedToViewport()
{
    if (CurrentSequencer.IsValid())
    {
        TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();
        return Sequencer->IsPerspectiveViewportCameraCutEnabled();
    }

    return false;
}

void UEposSequenceEditorBlueprintLibrary::SetLockCameraCutToViewport(bool bLock)
{
    if (CurrentSequencer.IsValid())
    {
        TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();

        if (bLock)
        {
            for(FLevelEditorViewportClient* LevelVC : GEditor->GetLevelViewportClients())
            {
                if (LevelVC && LevelVC->AllowsCinematicControl() && LevelVC->GetViewMode() != VMI_Unknown)
                {
                    LevelVC->SetActorLock(nullptr);
                    LevelVC->bLockedCameraView = false;
                    LevelVC->UpdateViewForLockedActor();
                    LevelVC->Invalidate();
                }
            }
            Sequencer->SetPerspectiveViewportCameraCutEnabled(true);
        }
        else
        {
            Sequencer->UpdateCameraCut(nullptr, EMovieSceneCameraCutParams());
            Sequencer->SetPerspectiveViewportCameraCutEnabled(false);
        }

        Sequencer->ForceEvaluate();
    }
}
