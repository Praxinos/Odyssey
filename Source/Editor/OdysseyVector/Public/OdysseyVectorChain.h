// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>

class FOdysseyVectorVertex;
class FOdysseyVectorSegment;
class FOdysseyVectorPath;

// a waypoint is met at segment vertex or when a constrast is met
struct FWayPoint
{
    FOdysseyVectorSegment* segment;
    double t;
};

class FOdysseyVectorChain
{
    public:
        ~FOdysseyVectorChain();
         FOdysseyVectorChain( FOdysseyVectorPath* iPath
                            , FOdysseyVectorVertex* iInitiatorVertex );

        uint32 HitMask( BLContext* iBLContext );
        uint32 Iterate( std::function<bool( FOdysseyVectorVertex*
                                          , FOdysseyVectorSegment*)> iCallback );

        friend class FOdysseyVectorPath;

    private :
        uint32 TraceLine( uint8 iLastalphaValue
                        , int32
                        , int32
                        , double
                        , int32
                        , int32
                        , double
                        , BLImageData*
                        , std::vector<FWayPoint>& oWayPointArray );

    private :
        FOdysseyVectorPath* mPath;
        FOdysseyVectorVertex* mVertex;
        std::vector<FOdysseyVectorSegment*> mSegmentArray;
        double mLength;
        ::ULIS::FRectD mBBox;
};
