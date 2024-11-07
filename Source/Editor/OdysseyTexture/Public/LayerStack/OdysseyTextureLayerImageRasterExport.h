#pragma once

#include "CoreMinimal.h"

class UOdysseyTextureLayerImageRaster;

class FOdysseyTextureLayerImageRasterExport
{
public:
    static void ODYSSEYTEXTURE_API Write( UOdysseyTextureLayerImageRaster* iTextureLayerImageRaster, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteRasterBlock( UOdysseyTextureLayerImageRaster* iTextureLayerImageRaster, FArchive &Ar );
};
