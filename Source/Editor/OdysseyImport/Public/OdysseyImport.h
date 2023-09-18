#pragma once

#include <Core/Core.h>

namespace FOdysseyImport
{
    void ODYSSEYIMPORT_API ReadChunks( uint64 iChunkEnd, FArchive &Ar, std::function<void(uint32, uint64, FArchive&)> iCallback );
}
