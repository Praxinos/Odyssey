// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "CoreMinimal.h"
#include "Framework/Commands/UICommandList.h"

class ODYSSEYCOREEDITOR_API FOdysseyCommandList
    : public FUICommandList
{
public:
    bool HasActionForKeyEvent( const FKeyEvent& InKeyEvent ) const;
    bool HasActionForKeyEvent( const FKey Key, const FModifierKeysState& ModifierKeysState, const bool bRepeat ) const;
    bool HasActionForKeyEvent( const FKey Key, bool bCtrl, bool bAlt, bool bShift, bool bCmd, bool bRepeat ) const;
};
