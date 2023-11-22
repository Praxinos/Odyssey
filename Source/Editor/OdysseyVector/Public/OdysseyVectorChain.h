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

        uint32 HitMask( BLContext* iBLContext, std::vector<FOdysseyVectorObject*>& oNewPathArray );
        void Iterate( std::function<bool( FOdysseyVectorVertex*, FOdysseyVectorSegment*)> iCallback );

        friend class FOdysseyVectorPath;

    private :
        uint8 GetAlpha( int32 iX, int32 iY, BLImageData* iImageData );
        bool CheckContrast( uint8 iAlphaValue0, uint8 iAlphaValue1 );

        bool TraceLine( uint8 iLastAlphaValue
                      , int32 iX0
                      , int32 iY0
                      , double iT0
                      , int32 iX1
                      , int32 iY1
                      , double iT1
                      , BLImageData* iImageData
                      , FOdysseyVectorSegment* iSegment
                      , std::vector<FWayPoint>& oWayPointArray );

        void Trace( BLImageData* iImageData, std::vector<FWayPoint>& oWayPointarray );

    private :
        FOdysseyVectorPath* mPath;
        std::vector<FOdysseyVectorVertex*> mVertexArray;
        std::vector<FOdysseyVectorSegment*> mSegmentArray;
        double mLength;
        ::ULIS::FRectD mBBox;
};
