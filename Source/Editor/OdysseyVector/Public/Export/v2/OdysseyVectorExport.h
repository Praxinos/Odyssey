#pragma once

#include "CoreMinimal.h"
#include "Serialization/Archive.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVector.h"

class  FOdysseyVectorObject;
class  FOdysseyVectorBucket;
class  FOdysseyVectorPath;
struct FOdysseyVectorBrush;
class  FOdysseyVectorVertex;
class  FOdysseyVectorGroup;
class  FOdysseyVectorGroupPaint;
class  FOdysseyVectorTagInbetweener;
class  FInbetweenerTrajectory;
class  FInbetweenerGridARAP;
class  FInbetweenerBreakdown;
class  FInbetweenerRoute;

namespace FOdysseyVectorExportV2
{
    uint32 GetObjectType( FOdysseyVectorObject& iObject );

    void ODYSSEYVECTOR_API Write( FOdysseyVectorGroupPaint* iScene, FArchive &Ar );

    ////////////////////////////////////
    void ODYSSEYVECTOR_API WriteObject( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteDefineObjects( std::vector<FOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar );
    void WriteDefineObjectEntry( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteDefineObjectID( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectChunks( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectOpacity( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectExpansion( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectBackgroundColor( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectForegroundColor( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectBackgroundBucket( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectForegroundBucket( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectTags( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectID( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectParentID( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectName( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectTransform( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectTransformTranslation( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectTransformRotation( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectTransformScaling( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteDeclareObjects( std::vector<FOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar );
    void WriteDeclareObjectEntry( FOdysseyVectorObject& iObject, FArchive &Ar );

    ////////////////////////////////////
    void ODYSSEYVECTOR_API WriteBucket( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketColor( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketGradient( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketGradientStop( FColor& iStopColor, double iStopAt, FArchive &Ar );
    void WriteBucketRotation( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketPosition( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketSpreading( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketPropagated( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketPaletteEntryMark1( FOdysseyVectorBucket& iBucket, FArchive& Ar);
    void WriteBucketPaletteEntryMark2( FOdysseyVectorBucket& iBucket, FArchive& Ar);
    void WriteBucketColorMode( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketGradientRadialOffset( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketGradientRadialRadius( FOdysseyVectorBucket& iBucket, FArchive &Ar );

    ////////////////////////////////////
    void WriteBrush( const FOdysseyVectorBrush& iBrush, FArchive &Ar );
    void WriteBrushTexture( const FOdysseyVectorBrush& iBrush, FArchive &Ar );
    void WriteBrushExtendOverPath( const FOdysseyVectorBrush& iBrush, FArchive &Ar );
    void WriteBrushColorFromBrush( const FOdysseyVectorBrush& iBrush, FArchive &Ar );
    void WriteBrushRevert( const FOdysseyVectorBrush& iBrush, FArchive &Ar );

    ////////////////////////////////////
    void ODYSSEYVECTOR_API WritePath( FOdysseyVectorPath& iPath, FArchive &Ar );
    void WritePathGeometrySegments( FOdysseyVectorPath& iPath, FArchive &Ar );
    void WritePathGeometryVertices( FOdysseyVectorPath& iPath, FArchive &Ar );
    void WritePathGeometryVertex( FOdysseyVectorVertex& iVertex, FArchive &Ar );
    void WritePathGeometryVertexPosition( FOdysseyVectorVertex& iVertex, FArchive &Ar );
    void WritePathGeometryVertexHandleAlignment( FOdysseyVectorVertex& iVertex, FArchive &Ar );
    void WritePathGeometryVertexLock( FOdysseyVectorVertex& iVertex, FArchive &Ar );
    void WritePathGeometry( FOdysseyVectorPath& iPath, FArchive &Ar );
    void WritePathJoint( FOdysseyVectorPath& iPath, FArchive &Ar );
    void WritePathMiterLimit( FOdysseyVectorPath& iPath, FArchive &Ar );
    void WritePathBrush( FOdysseyVectorPath& iPath, FArchive &Ar );

    ////////////////////////////////////
    void ODYSSEYVECTOR_API WriteGroup( FOdysseyVectorGroup& iGroup, FArchive &Ar );
    void WriteGroupChunks( FOdysseyVectorGroup& iGroup, FArchive &Ar );

    ////////////////////////////////////
    void ODYSSEYVECTOR_API WriteGroupPaint( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintChunks( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintBuckets( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintGapTolerance( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintGap( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintMonochromeColor( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintMonochrome( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintWireframeColor( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintWireframe( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintPainted( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintIntersectsCanevas( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );

    void ODYSSEYVECTOR_API WriteTagInbetweener( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerColor( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerDrawingCount( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerMapAsPolyline( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerChart( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerGrid( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerDeformation( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerInterpolation( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerDimension( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerGridGeometry( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerGridGeometryMk2( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerTransformTranslation( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerTransformRotation( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerTransformScaling( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerTransform( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerArapRigidity( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerGridTrajectories( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerBreakdowns( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerBreakdownsLayout( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerBreakdownsGeometry( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );
    void WriteTagInbetweenerRoutes( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar );

    ////////////////////////////////////
    void ODYSSEYVECTOR_API WriteBreakdown( FInbetweenerBreakdown& iBreakdown, FArchive &Ar );
    void WriteBreakdownGridGeometry( FInbetweenerBreakdown& iBreakdown, FArchive &Ar );
    void WriteBreakdownTransformScaling( FInbetweenerBreakdown& iBreakdown, FArchive &Ar );
    void WriteBreakdownTransformRotation( FInbetweenerBreakdown& iBreakdown, FArchive &Ar );
    void WriteBreakdownTransformTranslation( FInbetweenerBreakdown& iBreakdown, FArchive &Ar );
    void WriteBreakdownTransform( FInbetweenerBreakdown& iBreakdown, FArchive &Ar );
    void WriteBreakdownReach( FInbetweenerBreakdown& iBreakdown, FArchive &Ar );
    void WriteBreakdownMaster( FInbetweenerBreakdown& iBreakdown, FArchive &Ar );
    void WriteBreakdownChart( FInbetweenerBreakdown& iBreakdown, FArchive &Ar );
    void WriteBreakdownChartSpacing( FInbetweenerBreakdown& iBreakdown, FArchive &Ar );
    void WriteBreakdownChartHUBBezier( FInbetweenerBreakdown& iBreakdown, FArchive &Ar );

    void ODYSSEYVECTOR_API WriteRoute( FInbetweenerRoute& iRoute, FArchive &Ar );
    void WriteRouteCoords( FInbetweenerRoute& iRoute, FArchive &Ar );
    void WriteRouteTrajectories( FInbetweenerRoute& iRoute, FArchive &Ar );


/*
    void ODYSSEYVECTOR_API WriteTrajectory( FInbetweenerTrajectory& iTrajectory
                                          , FArchive &Ar );
    void WriteTrajectoryCoords( FInbetweenerTrajectory& iTrajectory
                              , FArchive &Ar );
    void WriteTrajectoryGeometry( FInbetweenerTrajectory& iTrajectory
                                , FArchive &Ar );
    void WriteTrajectoryWaypoints( FInbetweenerTrajectory& iTrajectory
                                 , FArchive &Ar );
    void WriteTrajectoryWaypointsRatio( FInbetweenerTrajectory& iTrajectory
                                      , FArchive &Ar );
*/
}
