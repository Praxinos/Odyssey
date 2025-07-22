// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "Export/v2/OdysseyVectorExport.h"

class FOdysseyVectorObject;
class FOdysseyVectorGroup;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorTagInbetweener;
class FInbetweenerRoute;
class FInbetweenerBreakdown;
class FInbetweenerTrajectory;
class FOdysseyVectorBucket;
struct FOdysseyVectorBrush;
class FOdysseyVectorVertex;
class FOdysseyVectorPath;

class ODYSSEYVECTOR_API FOdysseyVectorImportV2
{
    public:
        ~FOdysseyVectorImportV2();
        FOdysseyVectorImportV2();

    void Reset();
    void PostLoadTextures();

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
    void ReadTagInbetweener( FOdysseyVectorTagInbetweener& iInbetweenerTag
                           , uint64 iChunkEnd
                           , FArchive &Ar );

    void ReadRoute( FInbetweenerRoute& iRoute
                  , uint64 iChunkEnd
                  , FArchive &Ar );

    void ReadTrajectory( FInbetweenerTrajectory& iTrajectory
                       , uint64 iChunkEnd
                       , FArchive &Ar );
    void ReadBreakdown( FInbetweenerBreakdown& iBreakdown
                      , uint64 iChunkEnd
                      , FArchive &Ar );

    private:
        FOdysseyVectorGroupPaint* mScene;
        std::vector<FOdysseyVectorObject*> mObjectArray;
        std::multimap<UTexture2D*,FOdysseyVectorBrush*> mBrushTextureMultiMap;
};
