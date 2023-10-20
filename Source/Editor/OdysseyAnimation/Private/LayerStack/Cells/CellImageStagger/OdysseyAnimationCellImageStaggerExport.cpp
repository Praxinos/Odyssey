#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStaggerExport.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "OdysseyFile.h"

void
FOdysseyAnimationCellImageStaggerExport::WriteBehaviour( FOdysseyAnimationCellImageStagger* iAnimationCellImageStagger
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELLIMAGESTAGGER_BEHAVIOUR
                            , Ar
                            , [iAnimationCellImageStagger](FArchive &Ar) -> void
    {
        Ar << iAnimationCellImageStagger->mBehaviour;
    });
}

void
FOdysseyAnimationCellImageStaggerExport::WriteReach( FOdysseyAnimationCellImageStagger* iAnimationCellImageStagger
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELLIMAGESTAGGER_REACH
                            , Ar
                            , [iAnimationCellImageStagger](FArchive &Ar) -> void
    {
        Ar << iAnimationCellImageStagger->mReach;
    });
}

void
FOdysseyAnimationCellImageStaggerExport::Write( FOdysseyAnimationCellImageStagger* iAnimationCellImageStagger
                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELLIMAGESTAGGER
                            , Ar
                            , [iAnimationCellImageStagger](FArchive &Ar) -> void
    {
        WriteBehaviour( iAnimationCellImageStagger, Ar );
        WriteReach( iAnimationCellImageStagger, Ar );
    } );
}
