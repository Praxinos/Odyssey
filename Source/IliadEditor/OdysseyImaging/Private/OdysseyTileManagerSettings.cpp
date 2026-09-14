// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTileManagerSettings.h"

#include "OdysseyTileManager.h"

//Static
UOdysseyTileManagerSettings*
UOdysseyTileManagerSettings::Get()
{
    return CastChecked<UOdysseyTileManagerSettings>(UOdysseyTileManagerSettings::StaticClass()->GetDefaultObject());
}

void
UOdysseyTileManagerSettings::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if( PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED( UOdysseyTileManagerSettings, MaxUncompressedSize ) )
    {
        FOdysseyTileManager::Get().EvictTiles();
    }
    else if( PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED( UOdysseyTileManagerSettings, MaxCompressedSize ) )
    {
        FOdysseyTileManager::Get().EvictTiles();
    }

    Super::PostEditChangeProperty( PropertyChangedEvent );
}
