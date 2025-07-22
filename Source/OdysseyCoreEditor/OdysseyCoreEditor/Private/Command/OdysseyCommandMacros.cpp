// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Command/OdysseyCommandMacros.h"
#include "Styling/ISlateStyle.h"
#include "Framework/Commands/UICommandInfo.h"

#define LOC_DEFINE_REGION

void OdysseyMakeUIBundleCommand_InternalUseOnly( FBindingContext* This, TSharedPtr< FUICommandInfo >& OutCommand, const FName InBundle, const TCHAR* InSubNamespace, const TCHAR* InCommandName, const TCHAR* InCommandNameUnderscoreTooltip, const ANSICHAR* DotCommandName, const TCHAR* FriendlyName, const TCHAR* InDescription, const EUserInterfaceActionType CommandType, const FInputChord& InDefaultChord, const FInputChord& InAlternateDefaultChord)
{
    static const FString UICommandsStr(TEXT("UICommands"));
    const FString Namespace = InSubNamespace && FCString::Strlen(InSubNamespace) > 0 ? UICommandsStr + TEXT(".") + InSubNamespace : UICommandsStr;

    FUICommandInfo::MakeCommandInfo(
        This->AsShared(),
        OutCommand,
        InCommandName,
        FText::AsLocalizable_Advanced( *Namespace, InCommandName, FriendlyName ),
        FText::AsLocalizable_Advanced( *Namespace, InCommandNameUnderscoreTooltip, InDescription ),
        FSlateIcon( This->GetStyleSetName(), ISlateStyle::Join( This->GetContextName(), DotCommandName ) ),
        CommandType,
        InDefaultChord,
        InAlternateDefaultChord,
        InBundle
    );
}

#undef LOC_DEFINE_REGION
