#pragma once

#include <Core/Core.h>

class FOdysseyAnimationCellImageVector;

namespace FOdysseyAnimationCellImageVectorImport
{
    void ODYSSEYANIMATION_API Read( FOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
}
