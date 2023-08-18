#pragma once

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVector.h"
#include "Export/v2/OdysseyVectorExport.h"

class ODYSSEYVECTOR_API FOdysseyVectorImportV2
{
    public:
        ~FOdysseyVectorImportV2();
        FOdysseyVectorImportV2();

    void ReadChunks( uint64 iChunkEnd, FArchive &Ar, std::function<void(uint32, uint64, FArchive&)> iCallback );

    void Read( FOdysseyVectorScene* iScene, FArchive &Ar, uint64 iChunkEnd );
    void ReadObject( FOdysseyVectorObject& iObject, uint64 iChunkEnd, FArchive &Ar );
    void ReadObjectsDeclare( uint64 iChunkEnd, FArchive &Ar );
    void ReadObjectsDefine( uint64 iChunkEnd, FArchive &Ar );
    void ReadPath( FOdysseyVectorPath& iPath, uint64 iChunkEnd, FArchive &Ar );
    void ReadGroupPaint( FOdysseyVectorGroupPaint& iPaintGroup, uint64 iChunkEnd, FArchive &Ar );
    void ReadBucket( FOdysseyVectorBucket& iBucket, uint64 iChunkEnd, FArchive &Ar );

    FOdysseyVectorObject* CreateObject( uint32 iObjectType );

    void ReadBucketEntry( FOdysseyVectorBucket& iBucket, uint64 iChunkEnd, FArchive &Ar );
    void ReadPathGeometryVertices( FOdysseyVectorPath& iPath
                                 , std::vector<FOdysseyVectorVertex*>& vertexArray
                                 , FArchive &Ar );
    void ReadPathGeometryCubicSegments( FOdysseyVectorPath& iPath
                                      , std::vector<FOdysseyVectorVertex*>& vertexArray
                                      , FArchive &Ar );
    void ReadObjectBucket( FOdysseyVectorBucket& iBucket, uint64 iChunkEnd, FArchive &Ar );
    void ReadObjectTransform( FOdysseyVectorObject& iObject, uint64 iChunkEnd, FArchive &Ar );

    private:
        std::vector<FOdysseyVectorObject*> mObjectArray;
};
