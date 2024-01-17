#include "LayerStack/LightTable/OdysseyAnimationLightTableExport.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableKeyExport.h"
#include "OdysseyFile.h"

void
FOdysseyAnimationLightTableExport::WriteDisplayPosition( FOdysseyAnimationLightTable* iAnimationLightTable, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_LIGHTTABLE_DISPLAY_POSITION
                            , Ar
                            , [iAnimationLightTable](FArchive &Ar) -> void
    {
        Ar << iAnimationLightTable->mDisplayPosition;
    });
}

void
FOdysseyAnimationLightTableExport::WriteColors( FOdysseyAnimationLightTable* iAnimationLightTable, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_LIGHTTABLE_COLORS
                            , Ar
                            , [iAnimationLightTable](FArchive &Ar) -> void
    {
        Ar << iAnimationLightTable->mPreviousKeysColor;
        Ar << iAnimationLightTable->mNextKeysColor;
    });
}

void
FOdysseyAnimationLightTableExport::WriteContrasts( FOdysseyAnimationLightTable* iAnimationLightTable, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_LIGHTTABLE_CONTRAST
                            , Ar
                            , [iAnimationLightTable](FArchive &Ar) -> void
    {
        Ar << iAnimationLightTable->mPreviousKeysContrast;
        Ar << iAnimationLightTable->mNextKeysContrast;
    });
}

void
FOdysseyAnimationLightTableExport::WriteKeys( FOdysseyAnimationLightTable* iAnimationLightTable, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_LIGHTTABLE_KEYS
                            , Ar
                            , [iAnimationLightTable](FArchive &Ar) -> void
    {
        for ( FOdysseyAnimationLightTableKey& key : iAnimationLightTable->mKeys )
        {
            FOdysseyAnimationLightTableKeyExport::Write( &key, Ar );
        }
    });
}

void
FOdysseyAnimationLightTableExport::Write( FOdysseyAnimationLightTable* iAnimationLightTable
                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_LIGHTTABLE
                            , Ar
                            , [iAnimationLightTable](FArchive &Ar) -> void
    {   
        WriteDisplayPosition( iAnimationLightTable, Ar );
        WriteColors( iAnimationLightTable, Ar );
        WriteContrasts( iAnimationLightTable, Ar );
        WriteKeys( iAnimationLightTable, Ar );
    });
}
