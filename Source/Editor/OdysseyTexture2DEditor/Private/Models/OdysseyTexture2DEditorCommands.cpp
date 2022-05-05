// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Models/OdysseyTexture2DEditorCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyTexture2DEditorCommands"

FOdysseyTexture2DEditorCommands::FOdysseyTexture2DEditorCommands()
    : TCommands<FOdysseyTexture2DEditorCommands>( "IliadTexture2DEditor", NSLOCTEXT( "Contexts", "IliadTexture2DEditor", "Iliad Texture2D Editor" ), NAME_None, FEditorStyle::GetStyleSetName() )
{
}

void
FOdysseyTexture2DEditorCommands::RegisterCommands()
{
}

#undef LOCTEXT_NAMESPACE
