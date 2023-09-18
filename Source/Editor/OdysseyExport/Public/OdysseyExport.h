#pragma once

#include "CoreMinimal.h"

namespace FOdysseyExport
{
    void ODYSSEYEXPORT_API WriteChunk( uint32 iChunkID, FArchive &Ar, std::function<void(FArchive &Ar)> iCallback );
}
