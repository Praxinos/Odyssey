// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

/** Internal function used by the UI_COMMAND macros to build the command. Do not call this directly as only the macros are gathered for localization; instead use FUICommandInfo::MakeCommandInfo for dynamic content */
ODYSSEYCORE_API void OdysseyMakeUIBundleCommand_InternalUseOnly(FBindingContext* This, TSharedPtr< FUICommandInfo >& OutCommand, const FName InBundle, const TCHAR* InSubNamespace, const TCHAR* InCommandName, const TCHAR* InCommandNameUnderscoreTooltip, const ANSICHAR* DotCommandName, const TCHAR* FriendlyName, const TCHAR* InDescription, const EUserInterfaceActionType CommandType, const FInputChord& InDefaultChord, const FInputChord& InAlternateDefaultChord = FInputChord());

#define UI_BUNDLE_COMMAND( CommandId, Bundle, FriendlyName, InDescription, CommandType, InDefaultChord, ... ) \
	OdysseyMakeUIBundleCommand_InternalUseOnly( this, CommandId, Bundle, TEXT(LOCTEXT_NAMESPACE), TEXT(#CommandId), TEXT(#CommandId) TEXT("_ToolTip"), "." #CommandId, TEXT(FriendlyName), TEXT(InDescription), CommandType, InDefaultChord, ## __VA_ARGS__ );

