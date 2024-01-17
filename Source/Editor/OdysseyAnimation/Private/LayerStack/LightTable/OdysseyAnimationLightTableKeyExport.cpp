#include "LayerStack/LightTable/OdysseyAnimationLightTableKeyExport.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "OdysseyFile.h"

void
FOdysseyAnimationLightTableKeyExport::WriteIsActivated( FOdysseyAnimationLightTableKey* iKey, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_LIGHTTABLE_KEY_ISACTIVATED
                            , Ar
                            , [iKey](FArchive &Ar) -> void
    {
        Ar << iKey->mIsActivated;
    });
}

void
FOdysseyAnimationLightTableKeyExport::WriteOpacity( FOdysseyAnimationLightTableKey* iKey, FArchive &Ar )
{
    
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_LIGHTTABLE_KEY_OPACITY
                            , Ar
                            , [iKey](FArchive &Ar) -> void
    {
        Ar << iKey->mOpacity;
    });
}


void
FOdysseyAnimationLightTableKeyExport::Write( FOdysseyAnimationLightTableKey* iKey
                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_LIGHTTABLE_KEY
                            , Ar
                            , [iKey](FArchive &Ar) -> void
    {   
        WriteIsActivated(iKey, Ar);
        WriteOpacity(iKey, Ar);
    });
}
