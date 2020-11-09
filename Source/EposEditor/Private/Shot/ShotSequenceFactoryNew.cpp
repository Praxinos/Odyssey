// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Shot/ShotSequenceFactoryNew.h"

#include "MovieScene.h"
#include "MovieSceneToolsProjectSettings.h"
#include "ISequencer.h"

#include "Shot/ShotSequence.h"

#define LOCTEXT_NAMESPACE "ShotSequenceFactory"

//---

UShotSequenceFactoryNew::UShotSequenceFactoryNew( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = UShotSequence::StaticClass();
}

UObject* UShotSequenceFactoryNew::FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn )
{
    auto NewShotSequence = NewObject<UShotSequence>( iParent, iName, iFlags | RF_Transactional );
    NewShotSequence->Initialize();

    // Set up some sensible defaults
    const UMovieSceneToolsProjectSettings* ProjectSettings = GetDefault<UMovieSceneToolsProjectSettings>();

    FFrameRate TickResolution = NewShotSequence->GetMovieScene()->GetTickResolution();
    NewShotSequence->GetMovieScene()->SetPlaybackRange( ( ProjectSettings->DefaultStartTime*TickResolution ).FloorToFrame(), ( ProjectSettings->DefaultDuration*TickResolution ).FloorToFrame().Value );

    return NewShotSequence;
}

bool UShotSequenceFactoryNew::ShouldShowInNewMenu() const
{
    return true;
}

#undef LOCTEXT_NAMESPACE
