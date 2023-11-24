// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>

class FOdysseyVectorVertex;
class FOdysseyVectorSegment;
class FOdysseyVectorPath;
class FOdysseyVectorObject;

enum eWayPointType : uint8
{
    OutsideErasureArea = 0,
    EntersErasureArea  = 1,
    LeavesErasureArea  = 2
};

// a waypoint is met at segment vertex or when a constrast is met
typedef struct _FWayPoint
{
    FOdysseyVectorSegment* segment;
    FOdysseyVectorVertex* vertex;
    eWayPointType type;
    double t;

    _FWayPoint( FOdysseyVectorVertex* iVertex
              , FOdysseyVectorSegment* iSegment
              , double iT
              , eWayPointType iWayPointType )
    {
        segment = iSegment;
        vertex = iVertex;
        type = iWayPointType;
        t = iT;
    }
} FWayPoint;

typedef struct _FWaySegment
{
    uint32 indexWayPoint0;
    uint32 indexWayPoint1;
    ::ULIS::FVec2D bezier[4];

    _FWaySegment( uint32 iIndexWayPoint0, uint32 iIndexWayPoint1, ::ULIS::FVec2D iBezier[4] )
    {
        // work with indexes because the wayPoint array is gonna be resized, so
        // we can't work with pointers
        indexWayPoint0 = iIndexWayPoint0;
        indexWayPoint1 = iIndexWayPoint1;

        memcpy( bezier, iBezier, sizeof( bezier ) ) ;
    }
} FWaySegment;

class FOdysseyVectorChain
{
    public:
        ~FOdysseyVectorChain();
         FOdysseyVectorChain( FOdysseyVectorPath* iPath
                            , FOdysseyVectorVertex* iInitiatorVertex );

        void Iterate( std::function<bool( FOdysseyVectorVertex*, FOdysseyVectorSegment*)> iCallback );

        friend class FOdysseyVectorPath;

    private :
        bool HitMask( BLImageData* iImageData
                    , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                    , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                    , std::vector<FWayPoint>& oWayPointArray
                    , std::vector<FWaySegment>& oWaySegmentArray );
        uint8 GetAlpha( int32 iX, int32 iY, BLImageData* iImageData );
        bool CheckContrast( uint8 iAlphaValue0, uint8 iAlphaValue1 );

        bool SegmentCreationPolicy( FWayPoint* iWayPoint0, FWayPoint* iWayPoint1 );

        uint32 TraceLine( int32 iX0
                        , int32 iY0
                        , double iT0
                        , int32 iX1
                        , int32 iY1
                        , double iT1
                        , BLImageData* iImageData
                        , FOdysseyVectorSegment* iSegment
                        , std::vector<FWayPoint>& oWayPointArray );

        bool Trace( BLImageData* iImageData
                  , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                  , std::vector<FWayPoint>& oWayPointArray
                  , std::vector<FWaySegment>& oWaySegmentArray );

    private :
        FOdysseyVectorPath* mPath;
        std::list<FOdysseyVectorVertex*> mVertexList;
        std::list<FOdysseyVectorSegment*> mSegmentList;
        double mLength;
        ::ULIS::FRectD mBBox;
};
