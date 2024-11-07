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

    void ParseObjectChunks( FOdysseyVectorObject& iObject
                          , uint32 iChunkID
                          , uint64 iChunkLen
                          , FArchive &Ar );
    void ParseGroupChunks( FOdysseyVectorGroup& iGroup
                         , uint32 iChunkID
                         , uint64 iChunkLen
                         , FArchive &Ar );
    void ParseGroupPaintChunks( FOdysseyVectorGroupPaint& iPaintGroup
                              , uint32 iChunkID
                              , uint64 iChunkLen
                              , FArchive &Ar );

    void Read( FOdysseyVectorGroupPaint* iScene, FArchive &Ar, uint64 iChunkEnd );
    void ReadObject( FOdysseyVectorObject& iObject, uint64 iChunkEnd, FArchive &Ar );
    void ReadObjectsDeclare( uint64 iChunkEnd, FArchive &Ar );
    void ReadObjectsDefine( uint64 iChunkEnd, FArchive &Ar );
    void ReadPath( FOdysseyVectorPath& iPath, uint64 iChunkEnd, FArchive &Ar );
    void ReadGroup( FOdysseyVectorGroup& iGroup, uint64 iChunkEnd, FArchive &Ar );
    void ReadGroupPaint( FOdysseyVectorGroupPaint& iPaintGroup, uint64 iChunkEnd, FArchive &Ar );
    void ReadBucket( FOdysseyVectorBucket& iBucket, uint64 iChunkEnd, FArchive &Ar );
    void ReadBrush( FOdysseyVectorBrush& iBrush, uint64 iChunkEnd, FArchive &Ar );

    FOdysseyVectorObject* CreateObject( uint32 iObjectType );

    void ReadBucketEntry( FOdysseyVectorBucket& iBucket, uint64 iChunkEnd, FArchive &Ar );
    void ReadPathGeometryVertices( FOdysseyVectorPath& iPath
                                 , std::vector<FOdysseyVectorVertex*>& vertexArray
                                 , FArchive &Ar );
    void ReadPathGeometryVertexPosition( FOdysseyVectorVertex& iVertex
                                       , FArchive &Ar );
    void ReadPathGeometryVertexHandleAlignment( FOdysseyVectorVertex& iVertex
                                              , FArchive &Ar );
    void ReadPathGeometryVertexLock( FOdysseyVectorVertex& iVertex
                                   , FArchive &Ar );
    void ReadPathGeometryCubicSegments( FOdysseyVectorPath& iPath
                                      , std::vector<FOdysseyVectorVertex*>& vertexArray
                                      , FArchive &Ar );
    void ReadObjectBucket( FOdysseyVectorBucket& iBucket, uint64 iChunkEnd, FArchive &Ar );
    void ReadObjectTransform( FOdysseyVectorObject& iObject, uint64 iChunkEnd, FArchive &Ar );
    void ReadScene( FOdysseyVectorGroupPaint& iScene, uint64 iChunkEnd, FArchive &Ar );

    private:
        std::vector<FOdysseyVectorObject*> mObjectArray;
        FOdysseyVectorGroupPaint* mScene;
};
