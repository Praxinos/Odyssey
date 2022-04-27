// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Board/BoardSequenceFactoryNew.h"

#include "MovieScene.h"
#include "MovieSceneToolsProjectSettings.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "Settings/EposTracksEditorSettings.h"

#define LOCTEXT_NAMESPACE "BoardSequenceFactory"

//---

UBoardSequenceFactoryNew::UBoardSequenceFactoryNew( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = UBoardSequence::StaticClass();
}

UObject* UBoardSequenceFactoryNew::FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn )
{
    auto NewBoardSequence = NewObject<UBoardSequence>( iParent, iName, iFlags | RF_Transactional );

    const UEposSequenceEditorSettings* SequenceSettings = GetDefault<UEposSequenceEditorSettings>();
    NewBoardSequence->Initialize( SequenceSettings->BoardSettings.DefaultTickFrameRate, SequenceSettings->BoardSettings.DefaultDisplayFrameRate );

    const UEposTracksEditorSettings* TrackSettings = GetDefault<UEposTracksEditorSettings>();

    FFrameRate TickResolution = NewBoardSequence->GetMovieScene()->GetTickResolution();
    NewBoardSequence->GetMovieScene()->SetPlaybackRange( 0, ( TrackSettings->DefaultSectionDuration * TickResolution ).FloorToFrame().Value );

    auto newTrack = NewBoardSequence->GetMovieScene()->AddMasterTrack<UMovieSceneCinematicBoardTrack>();

    return NewBoardSequence;
}

bool UBoardSequenceFactoryNew::ShouldShowInNewMenu() const
{
    return true;
}

#undef LOCTEXT_NAMESPACE
