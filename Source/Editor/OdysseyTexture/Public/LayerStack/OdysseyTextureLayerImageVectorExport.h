#pragma once

#include "CoreMinimal.h"

class UOdysseyTextureLayerImageVector;

class FOdysseyTextureLayerImageVectorExport
{
public:
    static void ODYSSEYTEXTURE_API Write( UOdysseyTextureLayerImageVector* iTextureLayerImageVector, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteVectorBlock( UOdysseyTextureLayerImageVector* iTextureLayerImageVector, FArchive &Ar );
};
