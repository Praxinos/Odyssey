// IDDN.FR.001.220036.002.S.P.2021.000.00000
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
UEposNamingConventionBlueprintLibrary::GenerateNoteAssetPathName( FString& oPath, FString& oName )
{
    oPath = TEXT( "" );
    oName = TEXT( "" );

    if( !CurrentSequencer.IsValid() )
        return TEXT( "" );

    ISequencer* sequencer = CurrentSequencer.Pin().Get();

    UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( sequencer->GetFocusedMovieSceneSequence() );
    if( !epos_sequence )
        return TEXT( "" );

    return NamingConvention::GenerateNoteAssetPathName( *sequencer, *epos_sequence, sequencer->GetFocusedTemplateID(), oPath, oName );
}

//---

//static
UNamingConventionSettings*
UEposNamingConventionBlueprintLibrary::GetEposNamingConventionDefaultSettings()
{
    return GetMutableDefault<UNamingConventionSettings>();
}
