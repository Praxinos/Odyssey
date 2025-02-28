// IDDN.FR.001.060015.008.S.X.2019.000.00000
// Odyssey is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyCommands.h"

#include "ISettingsModule.h"

#include "SAboutWindow.h"
#include "Styling/AppStyle.h"
#include "OdysseyStyle.h"
//#include "OdysseyAppStyle.h"

#define LOCTEXT_NAMESPACE "OdysseyCommands"

FOdysseyCommands::FOdysseyCommands()
    : TCommands<FOdysseyCommands>( "OdysseyCommands" /* must match Set() parameter in style*/, LOCTEXT("editor-commands.name", "Odyssey"), NAME_None, FOdysseyStyle::Get().GetStyleSetName() )
{
}

void
FOdysseyCommands::RegisterCommands()
{
    UI_COMMAND( OpenOdysseyDocumentation,              "Odyssey User Documentation...", "Go to User Documentation", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( OpenOdysseyAboutWindow,                    "About Odyssey...", "Open the About window", EUserInterfaceActionType::Button, FInputChord() );
}

#undef LOCTEXT_NAMESPACE
