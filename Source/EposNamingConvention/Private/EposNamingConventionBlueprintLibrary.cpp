// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "EposNamingConventionBlueprintLibrary.h"

#include "ISequencer.h"

//#include "EposMovieSceneSequence.h"
#include "NamingConvention.h"
#include "Settings/NamingConventionSettings.h"

//---

namespace
{
    static TWeakPtr<ISequencer> CurrentSequencer;
}

//static
void
UEposNamingConventionBlueprintLibrary::SetSequencer( TSharedRef<ISequencer> InSequencer )
{
    CurrentSequencer = TWeakPtr<ISequencer>( InSequencer );
}

//---

//static
FString
UEposNamingConventionBlueprintLibrary::GenerateNoteAssetPathName( const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    oPath = TEXT( "" );
    oName = TEXT( "" );

    if( !CurrentSequencer.IsValid() )
        return TEXT( "" );

    if( !iSequence )
        return TEXT( "" );

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    //sequencer->GetEvaluationTemplate()->

    checkNoEntry(); //TODO: certainly expose a CreateNote() BP function instead of creating everything with native BP nodes, and then remove this BP function

    return TEXT( "" );
    //return NamingConvention::GenerateNoteAssetPathName( *sequencer, *iSequence, oPath, oName );
}

//---

//static
UNamingConventionSettings*
UEposNamingConventionBlueprintLibrary::GetEposNamingConventionDefaultSettings()
{
    return GetMutableDefault<UNamingConventionSettings>();
}
