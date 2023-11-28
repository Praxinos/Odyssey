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

// a waypoint is met at segment vertex or when a constrast is met
struct FWayPoint
{
    FOdysseyVectorVertex* vertex;
    uint32 flags;
    double t;

    // WayPoint flags
    static const uint32 Original           = ( 1 << 0 );
    static const uint32 OutsideErasureArea = ( 1 << 1 );
    static const uint32 EntersErasureArea  = ( 1 << 2 );
    static const uint32 InsideErasureArea  = ( 1 << 3 );
    static const uint32 LeavesErasureArea  = ( 1 << 4 );

    FWayPoint()
    {
        flags = 0;
    }

    FWayPoint( FOdysseyVectorVertex* iVertex, uint32 iWayPointFlags )
    {
        vertex = iVertex;
        flags = iWayPointFlags;
    }

    FWayPoint( FOdysseyVectorVertex* iVertex, uint32 iWayPointFlags, double iT )
    {
        vertex = iVertex;
        flags = iWayPointFlags;
        t = iT;
    }
};

struct FWaySegment
{
    FOdysseyVectorSegment* segment;
    uint32 indexWayPoint0;
    uint32 indexWayPoint1;
    ::ULIS::FVec2D bezier[4];
    bool revert;

    FWaySegment( FOdysseyVectorSegment* iSegment
               , std::vector<FWayPoint>& iWayPointArray
               , uint32 iIndexWayPoint0
               , uint32 iIndexWayPoint1 );
};

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

        FWayPoint* TraceLine( int32 iX0
                            , int32 iY0
                            , double iT0
                            , int32 iX1
                            , int32 iY1
                            , double iT1
                            , BLImageData* iImageData
                            , FWayPoint* lastWayPoint
                            , std::vector<FWayPoint>& oWayPointArray
                            , std::vector<FWaySegment>& oWaySegmentArray
                            , FOdysseyVectorSegment* iSegment
                            , bool iRevert );

        bool Trace( BLImageData* iImageData
                  , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                  , std::vector<FWayPoint>& oWayPointArray
                  , std::vector<FWaySegment>& oWaySegmentArray );

    private :
        FOdysseyVectorPath* mPath;
        std::vector<FOdysseyVectorVertex*> mVertexArray;
        std::vector<FOdysseyVectorSegment*> mSegmentArray;
        double mLength;
        ::ULIS::FRectD mBBox;
};
