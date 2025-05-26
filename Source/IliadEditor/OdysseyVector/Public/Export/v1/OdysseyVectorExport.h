// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
class  FOdysseyVectorGroupPaint;

namespace FOdysseyVectorExportV1
{
    void ODYSSEYVECTOR_API Write( FOdysseyVectorGroupPaint* iScene, FArchive &Ar );

    uint32 GetObjectType( FOdysseyVectorObject& iObject );

    ////////////////////////////////////
    void WriteDefineObjects( std::vector<FOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar );
    void WriteDefineObjectEntry( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectBackgroundColor( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectForegroundColor( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectBackgroundBucket( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectForegroundBucket( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectID( FOdysseyVectorObject& iObject, FArchive &Ar );
    void WriteObjectParentID( FOdysseyVectorObject& iObject, FArchive &Ar );
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
    //void WriteBucketSpreading( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketPropagated( FOdysseyVectorBucket& iBucket, FArchive &Ar );
    void WriteBucketPaletteEntry( FOdysseyVectorBucket& iBucket, FArchive& Ar);

    ////////////////////////////////////
    void ODYSSEYVECTOR_API WritePath( FOdysseyVectorPath& iPath, FArchive &Ar );
    void WritePathCubicGeometrySegments( FOdysseyVectorPath& iPath, FArchive &Ar );
    void WritePathCubicGeometryVertices( FOdysseyVectorPath& iPath, FArchive &Ar );
    void WritePathCubicGeometry( FOdysseyVectorPath& iPath, FArchive &Ar );
    void WritePathJoint( FOdysseyVectorPath& iPath, FArchive &Ar );

    ////////////////////////////////////
    void ODYSSEYVECTOR_API WriteGroupPaint( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintBuckets( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintGapTolerance( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintGap( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintMonochromeColor( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintMonochrome( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintWireframeColor( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintWireframe( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
    void WriteGroupPaintPainted( FOdysseyVectorGroupPaint& iPaintGroup, FArchive &Ar );
}
