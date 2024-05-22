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
    bool smooth;

    _FTracerPoint()
    {
        coords.x = coords.y = radius = 0.0f;
        smooth = false;
    }

    _FTracerPoint( uint32 iID, double iX, double iY, double iRadius )
    {
        id = iID;
        coords.x = iX;
        coords.y = iY;
        radius = iRadius;
        smooth = false;
    }
} FTracerPoint, FTracerRecord;

typedef struct _FTracerEdge
{
    ::ULIS::FVec2D vector;
    ::ULIS::FVec2D p0;
    ::ULIS::FVec2D p1;
    double radius0;
    double radius1;
    double length;
    uint32 id;

    _FTracerEdge()
    {
    }

    _FTracerEdge( uint32 iID, double iX0, double iY0, double iX1, double iY1, double iRadius0, double iRadius1 )
    {
        id = iID;

        p0.x = iX0;
        p0.y = iY0;

        p1.x = iX1;
        p1.y = iY1;

        radius0 = iRadius0;
        radius1 = iRadius1;

        vector = p1 - p0;

        length = vector.Distance();

        if( length )
        {
            vector.Normalize();
        }
    }
} FTracerEdge;

typedef struct _FTracerBezier
{
    bool inited;
    double firstRecordRadius;
    double lastRecordRadius;
    uint32 firstRecordID;
    uint32 lastRecordID;
    uint32 firstEdgeID;
    uint32 lastEdgeID;
    ::ULIS::FVec2D pt[4];

    _FTracerBezier()
    {
        inited = false;
    }
} FTracerBezier;

class ODYSSEYVECTOR_API FOdysseyVectorPathTracer
{
    private:
        double mDotLimit;
        uint32 mPointID;
        double mSampleDistance;
        double mTracingWidth;
        BLImage* mBLImage;
        BLContext mBLContext;
        std::vector<FTracerPoint> mPointArray;
        std::vector<FTracerRecord> mRecordArray;
        std::vector<FTracerEdge> mEdgeArray;
        ::ULIS::FVec2D mSmoothVector;
        FTracerBezier mCandidateBezier;
        FTracerBezier mBestBezier;
        FTracerBezier mRawBezier;
        FOdysseyVectorPath* mCubicPath;
        uint32 mWidth, mHeight;
        uint8* mPixelData;

    public:
        ~FOdysseyVectorPathTracer();
        FOdysseyVectorPathTracer();
  
        FTracerBezier& GetBestBezier();
        FTracerBezier& GetRawBezier();
        void AttachPath( FOdysseyVectorPath* iCubicPath );
        FOdysseyVectorPath* GetPath();
        BLImage* GetBLImage();
        FOdysseyVectorSegment* Trace( FOdysseyVectorVertex* iStitchedVertex
                                    , double iWorldX
                                    , double iWorldY
                                    , double iRadius );
        bool MakeBezier( bool iForce );
        bool TestBezier( ::ULIS::FVec2D iBezier[4] );
        void Init( FOdysseyVectorGroupPaint* iScene );
        std::vector<FTracerPoint>& GetPointArray();
        std::vector<FTracerRecord>& GetRecordArray();
        std::vector<FTracerEdge>& GetEdgeArray();
        FOdysseyVectorSegment* Flush( FOdysseyVectorVertex* iPreviousVertex
                                    , FOdysseyVectorVertex* iEndVertex );
        FOdysseyVectorSegment* CommitSegment( FOdysseyVectorVertex* iPreviousVertex
                                            , FOdysseyVectorVertex* iEndVertex );
        FOdysseyVectorVertex* CommitVertex( bool iIsHandleAligned );
        void Reset();
        void ClearPointsTo( uint32 iPointID );
        void ClearTo( uint32 iRecordID, uint32 iEdgeID );
        void AdjustBezier( ::ULIS::FVec2D iBezier[4], double iEdgeChainLength );
        void AdjustBezierHandle( ::ULIS::FVec2D iBezier[4]
                               , ::ULIS::FVec2D& expectedPoint
                               , ::ULIS::FVec2D& sampledPoint
                               , uint32 iAt );
        ::ULIS::FVec2D GetSamplePointAtParameter( double iEdgeChainLength, double iAt );
        double GetEdgeChainLength();
        void TraceEdges( double iAlpha );
        void TraceEdge( FTracerEdge* iEdge, double iAlpha );
        void SetDotLimit( double iDotLimit );
        void SetTracingWidth( double iTracingWidth );
        ::ULIS::FRectD GetRedrawRect();
};
