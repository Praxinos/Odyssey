// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "Board/BoardSequenceFactoryNew.h"

#include "MovieScene.h"
#include "MovieSceneToolsProjectSettings.h"

#include "Board/BoardSequence.h"

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
    NewBoardSequence->Initialize();

    // Set up some sensible defaults
    const UMovieSceneToolsProjectSettings* ProjectSettings = GetDefault<UMovieSceneToolsProjectSettings>();

    FFrameRate TickResolution = NewBoardSequence->GetMovieScene()->GetTickResolution();
    NewBoardSequence->GetMovieScene()->SetPlaybackRange( ( ProjectSettings->DefaultStartTime*TickResolution ).FloorToFrame(), ( ProjectSettings->DefaultDuration*TickResolution ).FloorToFrame().Value );

    return NewBoardSequence;
}

bool UBoardSequenceFactoryNew::ShouldShowInNewMenu() const
{
    return true;
}

#undef LOCTEXT_NAMESPACE
