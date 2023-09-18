#pragma once

#include <Core/Core.h>
#include "OdysseyVector.h"
#include "OdysseyImport.h"
#include "AnimationCellImageVector/OdysseyExportAnimationCellImageVector.h"

namespace FOdysseyImportAnimationCellImageVector
{
    void ODYSSEYIMPORT_API Read( FOdysseyAnimationCellImageVector* iAnimationCellImageVector
                               , FArchive &Ar
                               , uint64 iChunkEnd );
}
