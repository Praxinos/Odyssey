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

    void ODYSSEYVECTOR_API Read( UOdysseyVectorScene* iScene, FArchive &Ar );
    void ODYSSEYVECTOR_API ReadObjectsDeclare( std::vector<UOdysseyVectorObject*>& vectorObjectArray, uint64 iChunkEnd, FArchive &Ar );
    void ODYSSEYVECTOR_API ReadObjectsDefine( std::vector<UOdysseyVectorObject*>& vectorObjectArray, uint64 iChunkEnd, FArchive &Ar );
    void ODYSSEYVECTOR_API ReadObjectPathCubic( UOdysseyVectorPathCubic& iCubicPath, uint64 iChunkEnd, FArchive &Ar );
    void ODYSSEYVECTOR_API ReadObjectGroupPaint( UOdysseyVectorGroupPaint& iPaintGroup, uint64 iChunkEnd, FArchive &Ar );
    void ODYSSEYVECTOR_API ReadObjectEllipse( UOdysseyVectorEllipse& iCircle, uint64 iChunkEnd, FArchive &Ar );
}
