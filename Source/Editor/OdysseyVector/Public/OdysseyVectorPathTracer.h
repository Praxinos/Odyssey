#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorPath.h"

//#include "OdysseyVectorPathBuilder.generated.h"

typedef struct _FTracerPoint
{
    ::ULIS::FVec2D coords;
    double radius;
    uint32 id;

    _FTracerPoint()
    {
        coords.x = coords.y = radius = 0.0f;
    }

    _FTracerPoint( uint32 iID, double iX, double iY, double iRadius )
    {
        id = iID;
        coords.x = iX;
        coords.y = iY;
        radius = iRadius;
    }
} FTracerPoint, FTracerRecord, FTracerBezierPoint;

typedef struct _FTracerEdge
{
    ::ULIS::FVec2D vector;
    double length;
    uint32 id;

    _FTracerEdge()
    {
    }

    _FTracerEdge( uint32 iID, double iX0, double iY0, double iX1, double iY1 )
    {
        id = iID;

        vector.x = iX1 - iX0;
        vector.y = iY1 - iY0;

        length = vector.Distance();

        if( length )
        {
            vector.Normalize();
        }
    }
} FTracerEdge;

class ODYSSEYVECTOR_API FOdysseyVectorPathTracer
{
    private:
        double mCumulAngle;
        double mCumulAngleLimit;
        double mAngleLimit;
        uint32 mPointID;
        double mSampleDistance;
        BLImage* mBLImage;
        BLContext* mBLContext;
        std::vector<FTracerPoint> mPointArray;
        std::vector<FTracerRecord> mRecordArray;
        std::vector<FTracerEdge> mEdgeArray;
        FOdysseyVectorVertex* mLastVertex;
        FTracerBezierPoint mCandidateBezier[4];
        FOdysseyVectorPath* mCubicPath;
        //FOdysseyVectorSegmentCubic* mCubicSegment;
        uint32 mWidth, mHeight;
        uint8* mPixelData;

    public:
        ~FOdysseyVectorPathTracer();
        FOdysseyVectorPathTracer();

        void AttachPath( FOdysseyVectorPath* iCubicPath );
        FOdysseyVectorPath* GetPath();
        BLImage* GetBLImage();
        void Trace( double iWorldX, double iWorldY, double iRadius );
        void ClearEdgesUntil( std::vector<FTracerEdge>& iEdgeArray, uint32 iID );
        void ClearRecordsUntil( std::vector<FTracerRecord>& iRecordArray, uint32 iID );
        bool MakeBezier( bool iForce );
        bool TestBezier( FTracerBezierPoint iBezier[4] );
        double GetEdgeChainLength();
        void Init( FOdysseyVectorScene* iScene );
        std::vector<FTracerPoint>& GetPointArray();
        std::vector<FTracerRecord>& GetRecordArray();
        std::vector<FTracerEdge>& GetEdgeArray();
        void Flush();
        void CommitSegment();
};
