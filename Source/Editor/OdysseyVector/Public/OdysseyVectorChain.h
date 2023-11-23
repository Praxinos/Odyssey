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
struct FWayPoint
{
    FOdysseyVectorSegment* segment;
    FOdysseyVectorVertex* vertex;
    eWayPointType type;
    double t;

    FWayPoint( FOdysseyVectorVertex* iVertex, FOdysseyVectorSegment* iSegment, double iT, eWayPointType iWayPointType )
    {
        vertex = iVertex;
        segment = iSegment;
        t = iT;
        type = iWayPointType;
    }
};

class FOdysseyVectorChain
{
    public:
        ~FOdysseyVectorChain();
         FOdysseyVectorChain( FOdysseyVectorPath* iPath
                            , FOdysseyVectorVertex* iInitiatorVertex );

        bool HitMask( BLImageData* iImageData
                    , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                    , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                    , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                    , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray );

        void Iterate( std::function<bool( FOdysseyVectorVertex*, FOdysseyVectorSegment*)> iCallback );

        friend class FOdysseyVectorPath;

    private :
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
                  , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                  , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray );

    private :
        FOdysseyVectorPath* mPath;
        std::list<FOdysseyVectorVertex*> mVertexList;
        std::list<FOdysseyVectorSegment*> mSegmentList;
        double mLength;
        ::ULIS::FRectD mBBox;
};
