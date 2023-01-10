#pragma once

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroup.h"

namespace FOdysseyVectorImport
{
    void ODYSSEYVECTOR_API ReadChunks( uint64 iChunkEnd, FArchive &Ar, std::function<void(uint32, uint64, FArchive&)> iCallback );

    void ODYSSEYVECTOR_API Read( UOdysseyVectorRoot& iScene, uint64 iChunkEnd, FArchive &Ar );
}
