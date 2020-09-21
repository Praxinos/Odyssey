// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Models/OdysseyTextureEditorCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorCommands"

FOdysseyTextureEditorCommands::FOdysseyTextureEditorCommands()
    : TCommands<FOdysseyTextureEditorCommands>( "IliadTextureEditor", NSLOCTEXT( "Contexts", "IliadTextureEditor", "Iliad Texture Editor" ), NAME_None, FEditorStyle::GetStyleSetName() )
{
}

void
FOdysseyTextureEditorCommands::RegisterCommands()
{
}

#undef LOCTEXT_NAMESPACE
