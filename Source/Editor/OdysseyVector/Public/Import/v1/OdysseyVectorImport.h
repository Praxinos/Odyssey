#pragma once

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVector.h"
#include "Export/v1/OdysseyVectorExport.h"

namespace FOdysseyVectorImportV1
{
    void ODYSSEYVECTOR_API ReadChunks( uint64 iChunkEnd, FArchive &Ar, std::function<void(uint32, uint64, FArchive&)> iCallback );

    void ODYSSEYVECTOR_API Read( FOdysseyVectorGroupPaint* iScene, FArchive &Ar, uint64 iChunkEnd );
    void ODYSSEYVECTOR_API ReadObjectsDeclare( std::vector<FOdysseyVectorObject*>& vectorObjectArray, uint64 iChunkEnd, FArchive &Ar );
    void ODYSSEYVECTOR_API ReadObjectsDefine( std::vector<FOdysseyVectorObject*>& vectorObjectArray, uint64 iChunkEnd, FArchive &Ar );
    void ODYSSEYVECTOR_API ReadPath( FOdysseyVectorPath& iPath, uint64 iChunkEnd, FArchive &Ar );
    void ODYSSEYVECTOR_API ReadGroupPaint( FOdysseyVectorGroupPaint& iPaintGroup, uint64 iChunkEnd, FArchive &Ar );
    void ODYSSEYVECTOR_API ReadBucket( FOdysseyVectorBucket& iBucket, uint64 iChunkEnd, FArchive &Ar );

    FOdysseyVectorObject* CreateObject( uint32 iObjectType );

    void ReadBucketEntry( FOdysseyVectorBucket& iBucket, uint64 iChunkEnd, FArchive &Ar );
    void ReadPathGeometryVertices( FOdysseyVectorPath& iPath
                                 , std::vector<FOdysseyVectorVertex*>& vertexArray
                                 , FArchive &Ar );
    void ReadPathGeometryCubicSegments( FOdysseyVectorPath& iPath
                                      , std::vector<FOdysseyVectorVertex*>& vertexArray
                                      , FArchive &Ar );
    void ReadObjectsDefineObjectBucket( FOdysseyVectorBucket& iBucket
                                      , uint64 iChunkEnd
                                      , FArchive &Ar );
    void ReadObjectsDefineObjectBucket( FOdysseyVectorBucket& iBucket
                                      , uint64 iChunkEnd
                                      , FArchive &Ar );
    void ReadObjectsDefineObjectTransform( FOdysseyVectorObject& iObject
                                         , uint64 iChunkEnd
                                         , FArchive &Ar );
}
