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
    // Every budget applies at once.
    const FName Property = PropertyChangedEvent.GetMemberPropertyName();
    if (Property == GET_MEMBER_NAME_CHECKED(UOdysseyTileManagerSettings, MaxUncompressedSize)
        || Property == GET_MEMBER_NAME_CHECKED(UOdysseyTileManagerSettings, MaxCompressedSize)
        || Property == GET_MEMBER_NAME_CHECKED(UOdysseyTileManagerSettings, MaxGPUSize))
    {
        FOdysseyTileManager::Get().EvictTiles();
    }

    Super::PostEditChangeProperty( PropertyChangedEvent );
}
