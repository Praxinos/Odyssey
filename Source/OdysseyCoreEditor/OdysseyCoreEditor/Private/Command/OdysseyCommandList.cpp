// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyCommandList.h"

bool
FOdysseyCommandList::HasActionForKeyEvent( const FKey Key, const FModifierKeysState& ModifierKeysState, const bool bRepeat ) const
{
    return HasActionForKeyEvent(
        Key,
        ModifierKeysState.IsControlDown(),
        ModifierKeysState.IsAltDown(),
        ModifierKeysState.IsShiftDown(),
        ModifierKeysState.IsCommandDown(),
        bRepeat );
}

bool
FOdysseyCommandList::HasActionForKeyEvent( const FKeyEvent& InKeyEvent ) const
{
    return HasActionForKeyEvent( InKeyEvent.GetKey(), InKeyEvent.IsControlDown(), InKeyEvent.IsAltDown(), InKeyEvent.IsShiftDown(), InKeyEvent.IsCommandDown(), InKeyEvent.IsRepeat() );
}


bool
FOdysseyCommandList::HasActionForKeyEvent( const FKey Key, bool bCtrl, bool bAlt, bool bShift, bool bCmd, bool bRepeat ) const
{
    if ( !FSlateApplication::Get().IsDragDropping() )
    {
        FInputChord CheckChord( Key, EModifierKey::FromBools(bCtrl, bAlt, bShift, bCmd) );

        if( CheckChord.IsValidChord() )
        {
            TSet<FName> AllContextsToCheck;
            GatherContextsForList(AllContextsToCheck);

            for( TSet<FName>::TConstIterator It(AllContextsToCheck); It; ++It )
            {
                FName Context = *It;

                // Only active chords process commands
                const bool bCheckDefault = false;

                // Check to see if there is any command in the context activated by the chord
                TSharedPtr<FUICommandInfo> Command = FInputBindingManager::Get().FindCommandInContext( Context, CheckChord, bCheckDefault );

                if( Command.IsValid() && Command->HasActiveChord(CheckChord)  )
                {
                    // Find the bound action for this command
                    const FUIAction* Action = GetActionForCommand(Command);

                    // If there is no Action mapped to this command list, continue to the next context
                    if( Action )
                    {
                        return true;
                    }
                }
            }
        }
    }

    // No action was processed
    return false;
}
