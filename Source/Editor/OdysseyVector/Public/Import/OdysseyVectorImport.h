#pragma once

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVector.h"
#include "Export/OdysseyVectorExport.h"

namespace FOdysseyVectorImport
{
    void ODYSSEYVECTOR_API ReadChunks( uint64 iChunkEnd, FArchive &Ar, std::function<void(uint32, uint64, FArchive&)> iCallback );

    void ODYSSEYVECTOR_API Read( FOdysseyVectorScene* iScene, FArchive &Ar );
    void ODYSSEYVECTOR_API ReadObjectsDeclare( std::vector<FOdysseyVectorObject*>& vectorObjectArray, uint64 iChunkEnd, FArchive &Ar );
    void ODYSSEYVECTOR_API ReadObjectsDefine( std::vector<FOdysseyVectorObject*>& vectorObjectArray, uint64 iChunkEnd, FArchive &Ar );
    void ODYSSEYVECTOR_API ReadObjectPathCubic( FOdysseyVectorPathCubic& iCubicPath, uint64 iChunkEnd, FArchive &Ar );
    void ODYSSEYVECTOR_API ReadObjectGroupPaint( FOdysseyVectorGroupPaint& iPaintGroup, uint64 iChunkEnd, FArchive &Ar );
    void ODYSSEYVECTOR_API ReadObjectEllipse( FOdysseyVectorEllipse& iCircle, uint64 iChunkEnd, FArchive &Ar );
}
