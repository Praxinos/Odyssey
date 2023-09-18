#pragma once

#include <Core/Core.h>
#include "OdysseyVector.h"
#include "Vector/OdysseyExportVectorV1.h"

namespace FOdysseyImportVectorV1
{
    void ODYSSEYIMPORT_API Read( FOdysseyVectorScene* iScene, FArchive &Ar, uint64 iChunkEnd );
    void ODYSSEYIMPORT_API ReadObjectsDeclare( std::vector<FOdysseyVectorObject*>& vectorObjectArray, uint64 iChunkEnd, FArchive &Ar );
    void ODYSSEYIMPORT_API ReadObjectsDefine( std::vector<FOdysseyVectorObject*>& vectorObjectArray, uint64 iChunkEnd, FArchive &Ar );
    void ODYSSEYIMPORT_API ReadPath( FOdysseyVectorPath& iPath, uint64 iChunkEnd, FArchive &Ar );
    void ODYSSEYIMPORT_API ReadGroupPaint( FOdysseyVectorGroupPaint& iPaintGroup, uint64 iChunkEnd, FArchive &Ar );
    void ODYSSEYIMPORT_API ReadBucket( FOdysseyVectorBucket& iBucket, uint64 iChunkEnd, FArchive &Ar );

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
