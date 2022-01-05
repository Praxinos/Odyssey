// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Settings/NamingConventionSettings.h"

//---

FName
UNamingConventionSettings::GetContainerName() const
{
    return TEXT( "Editor" );
}

FName
UNamingConventionSettings::GetCategoryName() const
{
    return TEXT( "Plugins" );
}
