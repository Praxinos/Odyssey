// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "EposSequenceEditorBlueprintLibrary.h"

#include "Channels/MovieSceneChannel.h"
//For custom colors on channels, stored in editor pref's
#include "CurveEditorSettings.h"
#include "Filters/ISequencerTrackFilters.h"
#include "IKeyArea.h"
#include "ISequencer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EposSequenceEditorScriptingAccess.h"
#include "Modules/ModuleManager.h"
#include "MovieSceneCommonHelpers.h"
#include "MovieSceneFolder.h"
#include "MovieSceneSequencePlayer.h"
#include "MovieSceneSection.h"
#include "MovieSceneTimeHelpers.h"
#include "MVVM/ViewModels/ChannelModel.h"
#include "MVVM/ViewModels/SectionModel.h"
#include "MVVM/ViewModels/SequencerEditorViewModel.h"
#include "MVVM/SectionModelStorageExtension.h"
#include "MVVM/Selection/Selection.h"
#include "LevelEditor.h"
#include "LevelEditorViewport.h"
#include "Sections/MovieSceneSubSection.h"
#include "SequencerSettings.h"
#include "Subsystems/AssetEditorSubsystem.h"

#include "Board/BoardSequence.h"
#include "EposSequenceHelpers.h"
#include "Export/ExportConverter.h"
#include "Export/ImageSequence/ExportImageSequenceExporter.h"
#include "Export/PDF/ExportPDFExporter.h"
#include "Import/ImportImageSequenceConverter.h"
#include "Import/ImportImageSequenceImporter.h"
#include "Import/ImportImageSequenceStruct.h"
#include "OdysseyAnimationActor.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrackInstance.h"
#include "Shot/ShotSequence.h"
#include "Tools/EposSequenceTools.h"
#include "Tools/LighttableTools.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EposSequenceEditorBlueprintLibrary)

//---

namespace
{
    static TWeakPtr<ISequencer> CurrentSequencer;
}

using UE::EposSequenceEditor::Private::RaiseScriptingError;

//---

//static
UBoardSequence*
UBoardSequenceEditorBlueprintLibrary::GetRootBoardSequence()
{
    if( !CurrentSequencer.IsValid() )
        return nullptr;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    UEposMovieSceneSequence* root_sequence = EposSequenceHelpers::GetRootEposSequence( *sequencer, sequencer->GetFocusedTemplateID() );

    return Cast<UBoardSequence>( root_sequence );
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
UBoardSequence*
UBoardSequenceEditorBlueprintLibrary::CreateRootBoardSequence( const FString& iBoardPath, const FString& iBoardName )
{
    UBoardSequence* board_sequence = BoardSequenceTools::CreateBoard( iBoardPath, iBoardName );

    return board_sequence;

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

//static
UBoardSequence*
UBoardSequenceEditorBlueprintLibrary::ImportImageSequence( const FString& iBoardPath, const FString& iBoardName, const FImportImageSequenceOptions& iOptions )
{
    FString error;
    FImportImageSequenceImporter image_sequence_importer( iOptions, error );
    if( !error.IsEmpty() )
        return nullptr;

    UBoardSequence* board_sequence = CreateRootBoardSequence( iBoardPath, iBoardName );
    if( !board_sequence )
        return nullptr;

    UEposSequenceEditorBlueprintLibrary::OpenEposSequence( board_sequence );

    // Must be done after opening the new sequence
    if( !CurrentSequencer.IsValid() )
        return nullptr;

    const FImportImageSequenceStruct& image_struct = image_sequence_importer.GetImageSequenceStruct();

    FImportImageSequenceConverter( &image_struct, iOptions.bSpawnable, CurrentSequencer, board_sequence );

    return board_sequence;
}

//-

//static
void
UBoardSequenceEditorBlueprintLibrary::CreateCameraWithAnimation( UMovieSceneSubSection* iSubSection, bool iSpawnable )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    if( !CurrentSequencer.IsValid() )
        return;

    if( !board_section )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    sequencer->SetLocalTimeDirectly( board_section->GetTrueRange().GetLowerBoundValue() );

    FCameraArgs camera_args;
    camera_args.mSpawnable = iSpawnable;
    FAnimationArgs animation_args;
    animation_args.mSpawnable = iSpawnable;
    BoardSequenceTools::CreateCameraWithAnimation( sequencer, *board_section, board_section->GetTrueRange().GetLowerBoundValue(), camera_args, animation_args );
}

//static
void
UBoardSequenceEditorBlueprintLibrary::CreateAnimation( UMovieSceneSubSection* iSubSection, bool iSpawnable )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    if( !CurrentSequencer.IsValid() )
        return;

    if( !board_section )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    sequencer->SetLocalTimeDirectly( board_section->GetTrueRange().GetLowerBoundValue() );

    FAnimationArgs animation_args;
    animation_args.mSpawnable = iSpawnable;
    BoardSequenceTools::CreateAnimation( sequencer, *board_section, board_section->GetTrueRange().GetLowerBoundValue(), animation_args );
}

//static
void
UBoardSequenceEditorBlueprintLibrary::CollapseAnimation( UMovieSceneSubSection* iSubSection, const FMovieSceneBindingProxy& iBinding )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    if( !CurrentSequencer.IsValid() )
        return;

    if( !board_section )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    board_section->SetAnimationKeysAreaVisibility( iBinding.BindingID, false );
}

//static
void
UBoardSequenceEditorBlueprintLibrary::ExpandAnimation( UMovieSceneSubSection* iSubSection, const FMovieSceneBindingProxy& iBinding )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    if( !CurrentSequencer.IsValid() )
        return;

    if( !board_section )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    board_section->SetAnimationKeysAreaVisibility( iBinding.BindingID, true );
}

//static
bool
UBoardSequenceEditorBlueprintLibrary::IsAnimationCollapsed( UMovieSceneSubSection* iSubSection, const FMovieSceneBindingProxy& iBinding )
{
    return !IsAnimationExpanded( iSubSection, iBinding );
}

//static
bool
UBoardSequenceEditorBlueprintLibrary::IsAnimationExpanded( UMovieSceneSubSection* iSubSection, const FMovieSceneBindingProxy& iBinding )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );

    if( !CurrentSequencer.IsValid() )
        return false;

    if( !board_section )
        return false;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    return board_section->IsAnimationKeysAreaVisible( iBinding.BindingID );
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
UBoardSequenceEditorBlueprintLibrary::CreateAnimationCut( UMovieSceneSubSection* iSubSection, const FMovieSceneBindingProxy& iBinding, int32 iFrame )
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

    //FAnimationCutArgs animationcut_args;
    BoardSequenceTools::CreateAnimationCut( sequencer, *board_section, frame_in_tick, iBinding.BindingID );
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
    return !!iTake.GetSequence();
}

//static
FText
UBoardSequenceEditorBlueprintLibrary::GetDisplayName( FBoardSectionTake iTake )
{
    return iTake.GetSequence() ? iTake.GetSequence()->GetDisplayName() : FText::GetEmpty();
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

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    UEposMovieSceneSequence* root_sequence = EposSequenceHelpers::GetRootEposSequence( *sequencer, sequencer->GetFocusedTemplateID() );

    return Cast<UShotSequence>( root_sequence );
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
UShotSequenceEditorBlueprintLibrary::CreateCameraWithAnimation( bool iSpawnable )
{
    if( !CurrentSequencer.IsValid() )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    FCameraArgs camera_args;
    camera_args.mSpawnable = iSpawnable;
    FAnimationArgs animation_args;
    animation_args.mSpawnable = iSpawnable;
    ShotSequenceTools::CreateCameraWithAnimation( sequencer, camera_args, animation_args );
}

//static
void
UShotSequenceEditorBlueprintLibrary::CreateAnimation( bool iSpawnable )
{
    if( !CurrentSequencer.IsValid() )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    sequencer->SetLocalTimeDirectly( 0 );

    FAnimationArgs animation_args;
    animation_args.mSpawnable = iSpawnable;
    ShotSequenceTools::CreateAnimation( sequencer, 0, animation_args );
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
UShotSequenceEditorBlueprintLibrary::CreateAnimationCut( const FMovieSceneBindingProxy& iBinding, int32 iFrame )
{
    if( !CurrentSequencer.IsValid() )
        return;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    FFrameRate DisplayRate = sequencer->GetFocusedDisplayRate();
    FFrameRate TickResolution = sequencer->GetFocusedTickResolution();
    FFrameNumber frame_in_tick = ConvertFrameTime( iFrame, DisplayRate, TickResolution ).GetFrame();

    //FAnimationCutArgs animation_args;
    ShotSequenceTools::CreateAnimationCut( sequencer, frame_in_tick, iBinding.BindingID );
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

TArray<UObject*> UEposSequenceEditorBlueprintLibrary::LocateBoundObjects_PATCHBefore543( UMovieSceneSequence* Sequence, const FMovieSceneBindingProxy& InBinding, UObject* Context )
{
    if( !Sequence )
    {
        FFrame::KismetExecutionMessage( TEXT( "Cannot call LocateBoundObjects on a null sequence" ), ELogVerbosity::Error );
        return TArray<UObject*>();
    }

    using namespace UE::MovieScene;

    FSharedPlaybackStateCreateParams CreateParams;
    CreateParams.PlaybackContext = Context;
    TSharedRef<FSharedPlaybackState> TransientPlaybackState = MakeShared<FSharedPlaybackState>( *Sequence, CreateParams );

    FMovieSceneEvaluationState State;
    TransientPlaybackState->AddCapabilityRaw( &State );
    State.AssignSequence( MovieSceneSequenceID::Root, *Sequence, TransientPlaybackState );

    TArrayView<TWeakObjectPtr<>> Objects = State.FindBoundObjects( InBinding.BindingID, MovieSceneSequenceID::Root, TransientPlaybackState );

    TArray<UObject*> Result;
    for( TWeakObjectPtr<> WeakObject : Objects )
    {
        if( WeakObject.IsValid() )
        {
            Result.Add( WeakObject.Get() );
        }
    }
    return Result;
}


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
    if( !CurrentSequencer.IsValid() )
        return nullptr;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    UEposMovieSceneSequence* root_sequence = EposSequenceHelpers::GetRootEposSequence( *sequencer, sequencer->GetFocusedTemplateID() );

    return root_sequence;
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
            // Copy the sequence ID since PopToSequenceInstance takes it by reference
            FMovieSceneSequenceID SequenceID = Hierarchy[Hierarchy.Num() - 2];
            CurrentSequencer.Pin()->PopToSequenceInstance( SequenceID );
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
UEposSequenceEditorBlueprintLibrary::MoveAndScaleActor( AActor* ioActor, const ACineCameraActor* iCamera, float iNewDistance, EScaleActor iScaleType )
{
    if( !ioActor || !iCamera )
        return;

    ShotSequenceTools::MoveAndScaleActor( ioActor, iCamera, iNewDistance, iScaleType, CurrentSequencer.Pin() );
}

//static
void
UEposSequenceEditorBlueprintLibrary::SetCameraFocalLengthAndScaleActor( TArray<AActor*> ioActor, ACineCameraActor* ioCamera, float iNewFocalLength, EScaleActor iScaleType )
{
    if( !ioCamera )
        return;

    TArray<TWeakObjectPtr<AActor>> actors( ioActor );

    ShotSequenceTools::SetCameraFocalLengthAndScaleActor( actors, ioCamera, iNewFocalLength, iScaleType, CurrentSequencer.Pin() );
}

//static
void
UEposSequenceEditorBlueprintLibrary::FitActorToCameraView( AActor* ioActor, const ACineCameraActor* iCamera )
{
    if( !ioActor || !iCamera )
        return;

    ShotSequenceTools::FitActorToCameraView( ioActor, iCamera, CurrentSequencer.Pin() );
}

//---

//static
bool
UEposSequenceEditorBlueprintLibrary::ExportAsPDF( const FExportPDFOptions& iOptions )
{
    if( !CurrentSequencer.IsValid() )
        return false;

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    FMovieSceneSequenceID epos_root_sequence_id;
    UEposMovieSceneSequence* epos_root_sequence = EposSequenceHelpers::GetRootEposSequence( *sequencer, sequencer->GetFocusedTemplateID(), epos_root_sequence_id );
    if( !epos_root_sequence )
        return false;

    FExportStruct image_sequence_struct;
    FExportConverter converter( CurrentSequencer, epos_root_sequence_id, &iOptions.MarkSettings, &image_sequence_struct );

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

    FMovieSceneSequenceID epos_root_sequence_id;
    UEposMovieSceneSequence* epos_root_sequence = EposSequenceHelpers::GetRootEposSequence( *sequencer, sequencer->GetFocusedTemplateID(), epos_root_sequence_id );
    if( !epos_root_sequence )
        return false;

    FExportStruct image_sequence_struct;
    FExportConverter converter( CurrentSequencer, epos_root_sequence_id, &iOptions.MarkSettings, &image_sequence_struct );

    image_sequence_struct.mSequencer = CurrentSequencer;

    FExportImageSequenceExporter exporter( CurrentSequencer, &image_sequence_struct, &iOptions );
    return exporter.Export();
}

//---

//static
UEposSequenceEditorSettings*
UEposSequenceEditorBlueprintLibrary::GetEposSequenceEditorDefaultSettings()
{
    return GetMutableDefault<UEposSequenceEditorSettings>();
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

        FFrameTime GlobalTime = ConvertFrameTime( NewFrame, DisplayRate, TickResolution );
        if( GlobalTime == CurrentSequencer.Pin()->GetGlobalTime().Time )
        {
            CurrentSequencer.Pin()->ForceEvaluate();
        }
        else
        {
            CurrentSequencer.Pin()->SetGlobalTime( GlobalTime );
        }
    }
}

void UEposSequenceEditorBlueprintLibrary::SetGlobalPosition( FMovieSceneSequencePlaybackParams PlaybackParams, EMovieSceneTimeUnit TimeUnit )
{
    if( CurrentSequencer.IsValid() )
    {
        FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
        FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

        UMovieSceneSequence* Sequence = CurrentSequencer.Pin()->GetFocusedMovieSceneSequence();

        FFrameTime Position = PlaybackParams.GetPlaybackPosition( Sequence );
        if( TimeUnit == EMovieSceneTimeUnit::DisplayRate )
        {
            Position = ConvertFrameTime( Position, DisplayRate, TickResolution );
        }

        if( Position == CurrentSequencer.Pin()->GetGlobalTime().Time )
        {
            CurrentSequencer.Pin()->ForceEvaluate();
        }
        else
        {
            CurrentSequencer.Pin()->SetGlobalTime( Position );
        }
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

FMovieSceneSequencePlaybackParams UEposSequenceEditorBlueprintLibrary::GetGlobalPosition( EMovieSceneTimeUnit TimeUnit )
{
    FMovieSceneSequencePlaybackParams Params;
    if( CurrentSequencer.IsValid() )
    {
        FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
        FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

        FQualifiedFrameTime GlobalTime = CurrentSequencer.Pin()->GetGlobalTime();

        Params.Frame = TimeUnit == EMovieSceneTimeUnit::DisplayRate ? ConvertFrameTime( GlobalTime.Time, TickResolution, DisplayRate ) : GlobalTime.Time;
        Params.Timecode = GlobalTime.ToTimecode();
        Params.Time = GlobalTime.AsSeconds();

        return Params;
    }
    return Params;
}

void UEposSequenceEditorBlueprintLibrary::SetCurrentLocalTime(int32 NewFrame)
{
    if (CurrentSequencer.IsValid())
    {
        FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
        FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

        FFrameTime LocalTime = ConvertFrameTime( NewFrame, DisplayRate, TickResolution );
        if( LocalTime == CurrentSequencer.Pin()->GetLocalTime().Time )
        {
            CurrentSequencer.Pin()->ForceEvaluate();
        }
        else
        {
            CurrentSequencer.Pin()->SetLocalTime( LocalTime );
        }
    }
}

void UEposSequenceEditorBlueprintLibrary::SetLocalPosition( FMovieSceneSequencePlaybackParams PlaybackParams, EMovieSceneTimeUnit TimeUnit )
{
    if( CurrentSequencer.IsValid() )
    {
        FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
        FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

        UMovieSceneSequence* Sequence = CurrentSequencer.Pin()->GetFocusedMovieSceneSequence();
        FFrameTime Position = PlaybackParams.GetPlaybackPosition( Sequence );
        if( TimeUnit == EMovieSceneTimeUnit::DisplayRate )
        {
            Position = ConvertFrameTime( Position, DisplayRate, TickResolution );
        }

        if( Position == CurrentSequencer.Pin()->GetLocalTime().Time )
        {
            CurrentSequencer.Pin()->ForceEvaluate();
        }
        else
        {
            CurrentSequencer.Pin()->SetLocalTime( Position );
        }
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

FMovieSceneSequencePlaybackParams UEposSequenceEditorBlueprintLibrary::GetLocalPosition( EMovieSceneTimeUnit TimeUnit )
{
    FMovieSceneSequencePlaybackParams Params;
    if( CurrentSequencer.IsValid() )
    {
        FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
        FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

        FQualifiedFrameTime LocalTime = CurrentSequencer.Pin()->GetLocalTime();

        Params.Frame = TimeUnit == EMovieSceneTimeUnit::DisplayRate ? ConvertFrameTime( LocalTime.Time, TickResolution, DisplayRate ) : LocalTime.Time;
        Params.Timecode = LocalTime.ToTimecode();
        Params.Time = LocalTime.AsSeconds();

        return Params;
    }
    return Params;
}

void UEposSequenceEditorBlueprintLibrary::SetPlaybackSpeed( float NewPlaybackSpeed )
{
    // A negative speed plays in reverse; zero would never advance the playhead.
    if( !FMath::IsFinite( NewPlaybackSpeed ) || NewPlaybackSpeed == 0.f )
    {
        RaiseScriptingError( TEXT( "Playback speed must be non-zero and finite" ) );
        return;
    }

    if( CurrentSequencer.IsValid() )
    {
        CurrentSequencer.Pin()->SetPlaybackSpeed( NewPlaybackSpeed );
    }
}

float UEposSequenceEditorBlueprintLibrary::GetPlaybackSpeed()
{
    if( CurrentSequencer.IsValid() )
    {
        return CurrentSequencer.Pin()->GetPlaybackSpeed();
    }

    return 0.f;
}

void UEposSequenceEditorBlueprintLibrary::SetLoopMode( ESequencerLoopMode NewLoopMode )
{
    if( CurrentSequencer.IsValid() )
    {
        CurrentSequencer.Pin()->GetSequencerSettings()->SetLoopMode( NewLoopMode );
    }
}

ESequencerLoopMode UEposSequenceEditorBlueprintLibrary::GetLoopMode()
{
    if( CurrentSequencer.IsValid() )
    {
        return CurrentSequencer.Pin()->GetSequencerSettings()->GetLoopMode();
    }

    return ESequencerLoopMode::SLM_NoLoop;
}

void UEposSequenceEditorBlueprintLibrary::PlayTo( FMovieSceneSequencePlaybackParams PlaybackParams, EMovieSceneTimeUnit TimeUnit )
{
    if( CurrentSequencer.IsValid() )
    {
        FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
        FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

        if( TimeUnit == EMovieSceneTimeUnit::DisplayRate )
        {
            PlaybackParams.Frame = ConvertFrameTime( PlaybackParams.Frame, DisplayRate, TickResolution );
        }

        CurrentSequencer.Pin()->PlayTo( PlaybackParams );
    }
}

FMovieSceneSequencePlaybackParams UEposSequenceEditorBlueprintLibrary::GetPlaybackStartPosition( EMovieSceneTimeUnit TimeUnit )
{
    FMovieSceneSequencePlaybackParams Params;

    if( CurrentSequencer.IsValid() )
    {
        FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
        FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

        UMovieSceneSequence* Sequence = CurrentSequencer.Pin()->GetFocusedMovieSceneSequence();
        TRange<FFrameNumber> PlaybackRange = Sequence->GetMovieScene()->GetPlaybackRange();
        FQualifiedFrameTime StartPosition( UE::MovieScene::DiscreteInclusiveLower( PlaybackRange ), TickResolution );

        Params.Frame = TimeUnit == EMovieSceneTimeUnit::DisplayRate ? ConvertFrameTime( StartPosition.Time, TickResolution, DisplayRate ) : StartPosition.Time;
        Params.Timecode = StartPosition.ToTimecode();
        Params.Time = StartPosition.AsSeconds();
    }

    return Params;
}

FMovieSceneSequencePlaybackParams UEposSequenceEditorBlueprintLibrary::GetPlaybackEndPosition( EMovieSceneTimeUnit TimeUnit )
{
    FMovieSceneSequencePlaybackParams Params;

    if( CurrentSequencer.IsValid() )
    {
        FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
        FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

        UMovieSceneSequence* Sequence = CurrentSequencer.Pin()->GetFocusedMovieSceneSequence();
        TRange<FFrameNumber> PlaybackRange = Sequence->GetMovieScene()->GetPlaybackRange();

        FFrameTime OneFrame = FFrameRate::TransformTime( FFrameTime( 1 ), DisplayRate, TickResolution );
        FQualifiedFrameTime EndPosition( UE::MovieScene::DiscreteExclusiveUpper( PlaybackRange ) - OneFrame, TickResolution );

        Params.Frame = TimeUnit == EMovieSceneTimeUnit::DisplayRate ? ConvertFrameTime( EndPosition.Time, TickResolution, DisplayRate ) : EndPosition.Time;
        Params.Timecode = EndPosition.ToTimecode();
        Params.Time = EndPosition.AsSeconds();
    }

    return Params;
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

TArray<FSequencerChannelProxy> UEposSequenceEditorBlueprintLibrary::GetSelectedChannels()
{
    TArray<FSequencerChannelProxy> OutSelectedChannels;
    if( CurrentSequencer.IsValid() )
    {
        TArray<const IKeyArea*> SelectedKeyAreas;

        CurrentSequencer.Pin()->GetSelectedKeyAreas( SelectedKeyAreas );

        for( const IKeyArea* KeyArea : SelectedKeyAreas )
        {
            if( KeyArea )
            {
                FSequencerChannelProxy ChannelProxy( KeyArea->GetName(), KeyArea->GetOwningSection() );
                OutSelectedChannels.Add( ChannelProxy );
            }
        }
    }
    return OutSelectedChannels;
}

TArray<FSequencerChannelProxy> UEposSequenceEditorBlueprintLibrary::GetChannelsWithSelectedKeys()
{
    using namespace UE::Sequencer;

    TArray<FSequencerChannelProxy> OutSelectedChannels;
    TSet<FChannelModel*> ChannelModels;
    if( CurrentSequencer.IsValid() )
    {
        const FKeySelection KeySelection = CurrentSequencer.Pin()->GetViewModel()->GetSelection()->KeySelection;

        for( FKeyHandle Key : KeySelection )
        {
            TSharedPtr<FChannelModel> Channel = KeySelection.GetModelForKey( Key );
            if( Channel )
            {
                ChannelModels.Add( Channel.Get() );
            }
        }

        for( FChannelModel* Channel : ChannelModels )
        {
            if( Channel )
            {
                FSequencerChannelProxy ChannelProxy( Channel->GetChannelName(), Channel->GetSection() );
                OutSelectedChannels.Add( ChannelProxy );
            }
        }
    }
    return OutSelectedChannels;
}

TArray<int32> UEposSequenceEditorBlueprintLibrary::GetSelectedKeys( const FSequencerChannelProxy& ChannelProxy )
{
    TArray<int32> SelectedKeys;
    using namespace UE::Sequencer;

    if( CurrentSequencer.IsValid() )
    {
        const FKeySelection KeySelection = CurrentSequencer.Pin()->GetViewModel()->GetSelection()->KeySelection;

        for( FKeyHandle Key : KeySelection )
        {
            if( TSharedPtr<FChannelModel> Channel = KeySelection.GetModelForKey( Key ) )
            {
                if( Channel->GetChannelName() == ChannelProxy.ChannelName )
                {
                    int32 Index = Channel->GetChannel()->GetIndex( Key );
                    if( Index != INDEX_NONE )
                    {
                        SelectedKeys.Add( Index );
                    }
                }
            }
        }
    }
    return SelectedKeys;
}

TArray<UMovieSceneFolder*> UEposSequenceEditorBlueprintLibrary::GetSelectedFolders()
{
    TArray<UMovieSceneFolder*> OutSelectedFolders;
    if( CurrentSequencer.IsValid() )
    {
        CurrentSequencer.Pin()->GetSelectedFolders( OutSelectedFolders );
    }
    return OutSelectedFolders;
}

TArray<UMovieSceneFolder*> UEposSequenceEditorBlueprintLibrary::GetDirectlySelectedFolders()
{
    TArray<UMovieSceneFolder*> OutSelectedFolders;
    if( const TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin() )
    {
        Sequencer->GetDirectlySelectedFolders( OutSelectedFolders );
    }
    return OutSelectedFolders;
}

TArray<FMovieSceneBindingProxy> UEposSequenceEditorBlueprintLibrary::GetSelectedBindings()
{
    TArray<FMovieSceneBindingProxy> OutSelectedBindings;
    if( CurrentSequencer.IsValid() )
    {
        UMovieSceneSequence* Sequence = CurrentSequencer.Pin()->GetFocusedMovieSceneSequence();
        if( !Sequence )
        {
            return OutSelectedBindings;
        }

        TArray<FGuid> OutSelectedGuids;
        CurrentSequencer.Pin()->GetSelectedObjects( OutSelectedGuids );

        for( const FGuid& SelectedGuid : OutSelectedGuids )
        {
            OutSelectedBindings.Add( FMovieSceneBindingProxy( SelectedGuid, Sequence ) );
        }
    }
    return OutSelectedBindings;
}

static bool IsSelectableInFocusedMovieScene( const UObject* Object, const UMovieScene* FocusedMovieScene )
{
    return IsValid( Object ) && Object->GetTypedOuter<UMovieScene>() == FocusedMovieScene;
}

/** Reports a failure on the executing script frame. Does not unwind: callers must return immediately afterwards. */
static void RaiseSelectionScriptError( const FString& Message )
{
    if( FFrame::GetThreadLocalTopStackFrame() )
    {
        UKismetSystemLibrary::RaiseScriptError( Message );
    }
    else
    {
        FFrame::KismetExecutionMessage( *Message, ELogVerbosity::Error );
    }
}

/**
 * Whether ISequencer::SelectByChannels would find a node for Name on the section: Name is the metadata name of
 * one of the section's channels or the label of the outliner node that contains a channel group.
 */
static bool HasChannelNamed( ISequencer& Sequencer, const UMovieSceneSection* Section, FName Name )
{
    using namespace UE::Sequencer;

    FSectionModelStorageExtension* SectionModelStorage = Sequencer.GetViewModel()->GetRootModel()->CastDynamic<FSectionModelStorageExtension>();
    const TSharedPtr<FSectionModel> SectionModel = SectionModelStorage ? SectionModelStorage->FindModelForSection( Section ) : nullptr;
    if( !SectionModel )
    {
        return false;
    }

    const TSharedPtr<FViewModel> TrackNode = SectionModel->GetParentSectionOwnerModel().AsModel();
    for( const TViewModelPtr<FChannelGroupOutlinerModel>& KeyAreaNode : TrackNode->GetDescendantsOfType<FChannelGroupOutlinerModel>() )
    {
        const TViewModelPtr<IOutlinerExtension> Parent = KeyAreaNode->FindAncestorOfType<IOutlinerExtension>();
        if( Parent && FName( *Parent->GetLabel().ToString() ) == Name )
        {
            return true;
        }

        for( const TSharedRef<IKeyArea>& KeyArea : KeyAreaNode->GetAllKeyAreas() )
        {
            const FMovieSceneChannelMetaData* MetaData = KeyArea->GetChannel().GetMetaData();
            if( MetaData && MetaData->Name == Name )
            {
                return true;
            }
        }
    }
    return false;
}

/** The focused movie scene that selection entries must belong to; raises and returns null when there is none. */
static UMovieScene* GetSelectionMovieSceneOrRaise( const TSharedPtr<ISequencer>& Sequencer )
{
    if( !Sequencer.IsValid() )
    {
        RaiseSelectionScriptError( TEXT( "No epos sequence is open in Sequencer" ) );
        return nullptr;
    }
    const UMovieSceneSequence* FocusedSequence = Sequencer->GetFocusedMovieSceneSequence();
    UMovieScene* FocusedMovieScene = FocusedSequence ? FocusedSequence->GetMovieScene() : nullptr;
    if( !FocusedMovieScene )
    {
        RaiseSelectionScriptError( TEXT( "Sequencer has no focused movie scene" ) );
    }
    return FocusedMovieScene;
}

void UEposSequenceEditorBlueprintLibrary::SelectTracks(const TArray<UMovieSceneTrack*>& Tracks)
{
    const TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();
    const UMovieScene* FocusedMovieScene = GetSelectionMovieSceneOrRaise( Sequencer );
    if( !FocusedMovieScene )
    {
        return;
    }

    for( UMovieSceneTrack* Track : Tracks )
    {
        if( !IsSelectableInFocusedMovieScene( Track, FocusedMovieScene ) )
        {
            RaiseSelectionScriptError( TEXT( "All tracks must belong to the focused epos sequence" ) );
            return;
        }
    }

    for( UMovieSceneTrack* Track : Tracks )
    {
        Sequencer->SelectTrack( Track );
    }
}

void UEposSequenceEditorBlueprintLibrary::SelectSections(const TArray<UMovieSceneSection*>& Sections)
{
    const TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();
    const UMovieScene* FocusedMovieScene = GetSelectionMovieSceneOrRaise( Sequencer );
    if( !FocusedMovieScene )
    {
        return;
    }

    for( UMovieSceneSection* Section : Sections )
    {
        if( !IsSelectableInFocusedMovieScene( Section, FocusedMovieScene ) )
        {
            RaiseSelectionScriptError( TEXT( "All sections must belong to the focused epos sequence" ) );
            return;
        }
    }

    for( UMovieSceneSection* Section : Sections )
    {
        Sequencer->SelectSection( Section );
    }
}

void UEposSequenceEditorBlueprintLibrary::SelectChannels( const TArray<FSequencerChannelProxy>& Channels )
{
    const TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();
    const UMovieScene* FocusedMovieScene = GetSelectionMovieSceneOrRaise( Sequencer );
    if( !FocusedMovieScene )
    {
        return;
    }

    for( const FSequencerChannelProxy& ChannelProxy : Channels )
    {
        if( !IsSelectableInFocusedMovieScene( ChannelProxy.Section, FocusedMovieScene ) )
        {
            RaiseSelectionScriptError( TEXT( "All channels must belong to the focused epos sequence" ) );
            return;
        }

        if( !HasChannelNamed( *Sequencer, ChannelProxy.Section, ChannelProxy.ChannelName ) )
        {
            RaiseSelectionScriptError( FString::Printf( TEXT( "Channel '%s' does not exist on its section" ), *ChannelProxy.ChannelName.ToString() ) );
            return;
        }
    }

    for( const FSequencerChannelProxy& ChannelProxy : Channels )
    {
        TArray<FName> ChannelNames;
        ChannelNames.Add( ChannelProxy.ChannelName );
        Sequencer->SelectByChannels( ChannelProxy.Section, ChannelNames, false, true );
    }
}

void UEposSequenceEditorBlueprintLibrary::SelectKeys( const FSequencerChannelProxy& ChannelProxy, const TArray<int32>& Indices )
{
    using namespace UE::Sequencer;
    if( CurrentSequencer.IsValid() )
    {
        if( UMovieSceneSection* Section = ChannelProxy.Section )
        {
            FSectionModelStorageExtension* SectionModelStorage = CurrentSequencer.Pin()->GetViewModel()->GetRootModel()->CastDynamic<FSectionModelStorageExtension>();
            check( SectionModelStorage );

            TSharedPtr<FSectionModel> SectionHandle = SectionModelStorage->FindModelForSection( Section );
            if( SectionHandle )
            {
                TParentFirstChildIterator<FChannelGroupModel> KeyAreaNodes = SectionHandle->GetParentSectionOwnerModel().AsModel()->GetDescendantsOfType<FChannelGroupModel>();
                for( const TViewModelPtr<FChannelGroupModel>& KeyAreaNode : KeyAreaNodes )
                {
                    if( KeyAreaNode->GetChannelName() == ChannelProxy.ChannelName )
                    {
                        if( TSharedPtr<FChannelModel> ChannelModel = KeyAreaNode->GetChannel( Section ) )
                        {
                            FMovieSceneChannel* MovieSceneChannel = ChannelModel->GetChannel();
                            FKeySelection& KeySelection = CurrentSequencer.Pin()->GetViewModel()->GetSelection()->KeySelection;
                            for( int32 Index : Indices )
                            {
                                if( Index >= 0 && Index < MovieSceneChannel->GetNumKeys() )
                                {
                                    FKeyHandle KeyHandle = MovieSceneChannel->GetHandle( Index );
                                    KeySelection.Select( ChannelModel, KeyHandle );
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
}

void UEposSequenceEditorBlueprintLibrary::SelectFolders(const TArray<UMovieSceneFolder*>& Folders)
{
    const TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();
    const UMovieScene* FocusedMovieScene = GetSelectionMovieSceneOrRaise( Sequencer );
    if( !FocusedMovieScene )
    {
        return;
    }

    for( UMovieSceneFolder* Folder : Folders )
    {
        if( !IsSelectableInFocusedMovieScene( Folder, FocusedMovieScene ) )
        {
            RaiseSelectionScriptError( TEXT( "All folders must belong to the focused epos sequence" ) );
            return;
        }
    }

    for( UMovieSceneFolder* Folder : Folders )
    {
        Sequencer->SelectFolder( Folder );
    }
}

void UEposSequenceEditorBlueprintLibrary::SelectBindings( const TArray<FMovieSceneBindingProxy>& ObjectBindings )
{
    const TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();
    const UMovieScene* FocusedMovieScene = GetSelectionMovieSceneOrRaise( Sequencer );
    if( !FocusedMovieScene )
    {
        return;
    }
    const UMovieSceneSequence* FocusedSequence = Sequencer->GetFocusedMovieSceneSequence();

    for( const FMovieSceneBindingProxy& ObjectBinding : ObjectBindings )
    {
        if( ObjectBinding.Sequence.Get() != FocusedSequence || !ObjectBinding.BindingID.IsValid()
            || !FocusedMovieScene->FindBinding( ObjectBinding.BindingID ) )
        {
            RaiseSelectionScriptError( TEXT( "All bindings must belong to the focused epos sequence" ) );
            return;
        }
    }

    for( const FMovieSceneBindingProxy& ObjectBinding : ObjectBindings )
    {
        Sequencer->SelectObject( ObjectBinding.BindingID );
    }
}

void UEposSequenceEditorBlueprintLibrary::DeselectBindings(const TArray<FMovieSceneBindingProxy>& ObjectBindings)
{
    if (CurrentSequencer.IsValid())
    {
        for (const FMovieSceneBindingProxy& ObjectBinding : ObjectBindings)
        {
            CurrentSequencer.Pin()->DeselectObject(ObjectBinding.BindingID);
        }
    }
}

static TSharedPtr<UE::Sequencer::FSequencerSelection> GetViewModelSelection()
{
    using namespace UE::Sequencer;

    const TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();
    if( !Sequencer.IsValid() )
    {
        return nullptr;
    }

    const TSharedPtr<FSequencerEditorViewModel> ViewModel = Sequencer->GetViewModel();
    if( !ViewModel.IsValid() )
    {
        return nullptr;
    }

    return ViewModel->GetSelection();
}

TArray<int32> UEposSequenceEditorBlueprintLibrary::GetSelectedMarkedFrames()
{
    const TSharedPtr<UE::Sequencer::FSequencerSelection> Selection = GetViewModelSelection();
    return Selection.IsValid() ? Selection->MarkedFrames.GetSelected().Array() : TArray<int32>{};
}

void UEposSequenceEditorBlueprintLibrary::SelectMarkedFrames( const TArray<int32>& MarkedFrameIndices )
{
    const TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();
    if( !Sequencer.IsValid() )
    {
        return;
    }

    const UMovieSceneSequence* const FocusedSequence = Sequencer->GetFocusedMovieSceneSequence();
    if( !FocusedSequence )
    {
        return;
    }

    const UMovieScene* const MovieScene = FocusedSequence->GetMovieScene();
    if( !MovieScene )
    {
        return;
    }

    const TSharedPtr<UE::Sequencer::FSequencerEditorViewModel> ViewModel = Sequencer->GetViewModel();
    if( !ViewModel.IsValid() )
    {
        return;
    }

    const TSharedPtr<UE::Sequencer::FSequencerSelection> Selection = ViewModel->GetSelection();
    if( !Selection.IsValid() )
    {
        return;
    }

    // Only indices of marked frames that exist on the focused movie scene may
    // enter the selection set, mirroring the per-index validation SelectKeys
    // does above.
    const int32 NumMarkedFrames = MovieScene->GetMarkedFrames().Num();
    for( const int32 MarkedFrameIndex : MarkedFrameIndices )
    {
        if( MarkedFrameIndex >= 0 && MarkedFrameIndex < NumMarkedFrames )
        {
            Selection->MarkedFrames.Select( MarkedFrameIndex );
        }
    }
}

void UEposSequenceEditorBlueprintLibrary::DeselectMarkedFrames( const TArray<int32>& MarkedFrameIndices )
{
    const TSharedPtr<UE::Sequencer::FSequencerSelection> Selection = GetViewModelSelection();
    if( !Selection.IsValid() )
    {
        return;
    }

    for( const int32 MarkedFrameIndex : MarkedFrameIndices )
    {
        Selection->MarkedFrames.Deselect( MarkedFrameIndex );
    }
}

void UEposSequenceEditorBlueprintLibrary::EmptySelection()
{
    if (CurrentSequencer.IsValid())
    {
        CurrentSequencer.Pin()->EmptySelection();
    }
}

void UEposSequenceEditorBlueprintLibrary::EmptyMarkedFrameSelection()
{
    const TSharedPtr<UE::Sequencer::FSequencerSelection> Selection = GetViewModelSelection();
    if( !Selection.IsValid() )
    {
        return;
    }

    Selection->MarkedFrames.Empty();
}

void UEposSequenceEditorBlueprintLibrary::EmptyKeySelection()
{
    const TSharedPtr<UE::Sequencer::FSequencerSelection> Selection = GetViewModelSelection();
    if( !Selection.IsValid() )
    {
        return;
    }

    Selection->KeySelection.Empty();
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

///** GetSelectionMovieSceneOrRaise that also raises when the focused sequence is not visible to scripting. */
//static UMovieScene* GetVisibleSelectionMovieSceneOrRaise( const TSharedPtr<ISequencer>& Sequencer )
//{
//    UMovieScene* MovieScene = GetSelectionMovieSceneOrRaise( Sequencer );
//    FString AccessError;
//    if( MovieScene && !UE::SequencerScripting::IsObjectAccessibleForScripting( MovieScene, /* bRequireWritable */ false, AccessError ) )
//    {
//        RaiseSelectionScriptError( AccessError );
//        return nullptr;
//    }
//    return MovieScene;
//}
//
//bool UEposSequenceEditorBlueprintLibrary::HasSelectionRange()
//{
//    const UMovieScene* MovieScene = GetVisibleSelectionMovieSceneOrRaise( CurrentSequencer.Pin() );
//    return MovieScene && !MovieScene->GetSelectionRange().IsEmpty();
//}
//
//void UEposSequenceEditorBlueprintLibrary::ClearSelectionRange()
//{
//    // Read access only, as for the selection range setters and Sequencer's Clear Selection Range: the selection range is
//    // editor view state that Sequencer lets users change on read-only and locked sequences.
//    UMovieScene* MovieScene = GetVisibleSelectionMovieSceneOrRaise( CurrentSequencer.Pin() );
//    if( !MovieScene || MovieScene->GetSelectionRange().IsEmpty() )
//    {
//        return;
//    }
//
//    // Its own undo step, like the selection range start and end setters, which go through Sequencer's transaction.
//    const FScopedTransaction Transaction( NSLOCTEXT( "LevelSequenceEditorBlueprintLibrary", "ClearSelectionRange", "Clear Selection Range" ) );
//    MovieScene->Modify();
//    MovieScene->SetSelectionRange( TRange<FFrameNumber>::Empty() );
//}

void UEposSequenceEditorBlueprintLibrary::SetSequencer(TSharedRef<ISequencer> InSequencer)
{
    CurrentSequencer = TWeakPtr<ISequencer>(InSequencer);
}

TSharedPtr<ISequencer> UEposSequenceEditorBlueprintLibrary::GetCurrentSequencer()
{
    return CurrentSequencer.Pin();
}

void UEposSequenceEditorBlueprintLibrary::RefreshCurrentEposSequence()
{
    if (CurrentSequencer.IsValid())
    {
        CurrentSequencer.Pin()->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::Unknown);
    }
}

void UEposSequenceEditorBlueprintLibrary::ForceUpdate()
{
    if( CurrentSequencer.IsValid() )
    {
        CurrentSequencer.Pin()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::RefreshAllImmediately );
    }
}

void UEposSequenceEditorBlueprintLibrary::RestorePreAnimatedState()
{
    // RestorePreAnimatedState is inherited from IMovieScenePlayer (ISequencer's
    // base) and restores all captured state globally, regardless of section
    // completion mode.
    if( CurrentSequencer.IsValid() )
    {
        CurrentSequencer.Pin()->RestorePreAnimatedState();
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
    return IsTrackFilterActive( TrackFilterName );
}

bool UEposSequenceEditorBlueprintLibrary::IsTrackFilterActive( const FText& TrackFilterName )
{
    if( const TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin() )
    {
        return Sequencer->GetFilterInterface()->IsFilterActiveByDisplayName( TrackFilterName.ToString() );
    }
    return false;
}

void UEposSequenceEditorBlueprintLibrary::SetTrackFilterEnabled(const FText& TrackFilterName, bool bEnabled)
{
    return SetTrackFilterActive( TrackFilterName, bEnabled );
}

void UEposSequenceEditorBlueprintLibrary::SetTrackFilterActive( const FText& TrackFilterName, bool bActive )
{
    const TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();
    if( !Sequencer )
    {
        RaiseSelectionScriptError( TEXT( "No epos sequence is open in Sequencer" ) );
        return;
    }
    const TSharedRef<ISequencerTrackFilters> Filters = Sequencer->GetFilterInterface();
    const FString FilterName = TrackFilterName.ToString();
    // Filter bars match display names without regard to case.
    auto MatchesFilterName = [&FilterName]( const FText& DisplayName )
        {
            return DisplayName.ToString().Equals( FilterName, ESearchCase::IgnoreCase );
        };
    if( !Filters->GetFilterDisplayNames().ContainsByPredicate( MatchesFilterName )
        && !Filters->GetCustomTextFilterNames().ContainsByPredicate( MatchesFilterName ) )
    {
        RaiseSelectionScriptError( FString::Printf( TEXT( "No track filter is named '%s'" ), *FilterName ) );
        return;
    }
    Filters->SetFilterActiveByDisplayName( FilterName, bActive );
}

TArray<FText> UEposSequenceEditorBlueprintLibrary::GetTrackFilterNames()
{
    if( const TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin() )
    {
        return Sequencer->GetFilterInterface()->GetFilterDisplayNames();
    }
    return {};
}

//FString UEposSequenceEditorBlueprintLibrary::GetTrackFilterText()
//{
//    const TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();
//    if( !Sequencer.IsValid() )
//    {
//        RaiseSelectionScriptError( TEXT( "No epos sequence is open in Sequencer" ) );
//        return FString();
//    }
//
//    return Sequencer->GetFilterInterface()->GetTextFilterString();
//}
//
//void UEposSequenceEditorBlueprintLibrary::SetTrackFilterText( const FString& FilterText )
//{
//    const TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();
//    if( !Sequencer.IsValid() )
//    {
//        RaiseSelectionScriptError( TEXT( "No epos sequence is open in Sequencer" ) );
//        return;
//    }
//
//    Sequencer->GetFilterInterface()->SetTextFilterString( FilterText );
//}

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

void UEposSequenceEditorBlueprintLibrary::DeleteColorForChannels(UClass* Class, const FString& Identifier)
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

        Sequencer->SetPerspectiveViewportCameraCutEnabled( bLock );

        //bool bNeedsRestoreViewport = true;
        //if( const USequencerSettings* SequencerSettings = Sequencer->GetSequencerSettings() )
        //{
        //    bNeedsRestoreViewport = SequencerSettings->GetRestoreOriginalViewportOnCameraCutUnlock();
        //}

        //UMovieSceneEntitySystemLinker* Linker = Sequencer->GetEvaluationTemplate().GetEntitySystemLinker();
        //UMovieSceneSingleCameraCutTrackInstance::ToggleCameraCutLock( Linker, bLock, bNeedsRestoreViewport );

        Sequencer->ForceEvaluate();
    }
}

bool UEposSequenceEditorBlueprintLibrary::IsBoardInnerCameraCutLockedToViewport()
{
    if (CurrentSequencer.IsValid())
    {
        TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();
        return Sequencer->IsPerspectiveViewportCameraCutEnabled();
    }

    return false;
}

void UEposSequenceEditorBlueprintLibrary::SetLockBoardInnerCameraCutToViewport(bool bLock)
{
    if (CurrentSequencer.IsValid())
    {
        TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();

        Sequencer->SetPerspectiveViewportCameraCutEnabled( bLock );

        //bool bNeedsRestoreViewport = true;
        //if( const USequencerSettings* SequencerSettings = Sequencer->GetSequencerSettings() )
        //{
        //    bNeedsRestoreViewport = SequencerSettings->GetRestoreOriginalViewportOnCameraCutUnlock();
        //}

        //UMovieSceneEntitySystemLinker* Linker = Sequencer->GetEvaluationTemplate().GetEntitySystemLinker();
        //UMovieSceneSingleCameraCutTrackInstance::ToggleCameraCutLock( Linker, bLock, bNeedsRestoreViewport );

        Sequencer->ForceEvaluate();
    }
}
