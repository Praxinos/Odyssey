// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Note/StoryNoteFactoryNew.h"

#include "MovieScene.h"
#include "MovieSceneToolsProjectSettings.h"
#include "ISequencer.h"

#include "StoryNote.h"

#define LOCTEXT_NAMESPACE "StoryNoteFactory"

//---

UStoryNoteFactoryNew::UStoryNoteFactoryNew( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
    bCreateNew = true;
    bEditAfterNew = false;
    SupportedClass = UStoryNote::StaticClass();
}

UObject* UStoryNoteFactoryNew::FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn )
{
    UStoryNote* new_note = NewObject<UStoryNote>( iParent, iName, iFlags | RF_Transactional );

    new_note->Text = TEXT( "Write a note here" ); // default text

    return new_note;
}

bool UStoryNoteFactoryNew::ShouldShowInNewMenu() const
{
    return false;
}

#undef LOCTEXT_NAMESPACE
