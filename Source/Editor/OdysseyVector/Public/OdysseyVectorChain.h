// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>

class FOdysseyVectorVertex;
class FOdysseyVectorSegment;
class FOdysseyVectorSection;
class FOdysseyVectorPath;
class FOdysseyVectorObject;

// a waypoint is met at segment vertex or when a constrast is met
struct FWayPoint
{
    FOdysseyVectorVertexIntersection* intersectionVertex;
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
        intersectionVertex = nullptr;
        vertex = iVertex;
        flags = iWayPointFlags;
    }

    FWayPoint( FOdysseyVectorVertex* iVertex
             , FOdysseyVectorVertexIntersection* iIntersectionVertex
             , uint32 iWayPointFlags
             , double iT )
    {
        vertex = iVertex;
        intersectionVertex = iIntersectionVertex;
        flags = iWayPointFlags;
        t = iT;
    }
};

struct FWayFragment
{
    FOdysseyVectorSegment* segment;
    uint32 indexWayPoint0;
    uint32 indexWayPoint1;
    ::ULIS::FVec2D bezier[4];

    FWayFragment( FOdysseyVectorSegment* iSegment
                , std::vector<FWayPoint>& iWayPointArray
                , uint32 iIndexWayPoint0
                , uint32 iIndexWayPoint1 );
};

/** A class that contains an ordered and contiguous chain of segments and vertices
 *  (a FOdysseyVectorPath can have multiple sub-paths, i.e multiple chains).
*/
class FOdysseyVectorChain
{
    public:
        ~FOdysseyVectorChain();
         FOdysseyVectorChain( FOdysseyVectorPath* iPath
                            , FOdysseyVectorVertex* iInitiatorVertex );

        friend class FOdysseyVectorPath;
        friend class FOdysseyVectorGroupPaint;

        std::vector<FOdysseyVectorVertex*>& GetVertexArray();
        std::vector<FOdysseyVectorSegment*>& GetSegmentArray();

        void IterateSegments( std::function<bool( FOdysseyVectorVertex*, FOdysseyVectorSegment*)> iCallback );
        void IterateSections( std::function<bool( FOdysseyVectorVertex*, FOdysseyVectorSection*)> iCallback );


    private :
        bool EraseSections( BLImageData* iImageData
                          , std::vector<FWayPoint>& oWayPointArray
                          , std::vector<FWayFragment>& oWayFragmentArray );

        bool EraseSegments( BLImageData* iImageData
                          , std::vector<FWayPoint>& oWayPointArray
                          , std::vector<FWayFragment>& oWayFragmentArray );
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
                            , std::vector<FWayFragment>& oWayFragmentArray
                            , FOdysseyVectorSegment* iSegment
                            , bool iRevert );

        bool Trace( BLImageData* iImageData
                  , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                  , std::vector<FWayPoint>& oWayPointArray
                  , std::vector<FWayFragment>& oWayFragmentArray );
        bool PickSection( FOdysseyVectorSection* iSection
                        , const ::ULIS::FRectD& iMaskRect
                        , const uint8* iMaskPixelData );
        bool PickSections( std::vector<FOdysseyVectorSection*>& oPickedSectionArray );
        static void ExtendErasedSection( FOdysseyVectorVertex* iVertex
                                       , FOdysseyVectorSection* iFromSection );
        uint32 GetErasureFlags( FOdysseyVectorSection* iPrevSection
                              , FOdysseyVectorVertex* iVertex
                              , FOdysseyVectorSection* iNextSection );
        void GetSections( FOdysseyVectorVertex* iVertex
                        , FOdysseyVectorPath* iPath
                        , std::vector<FOdysseyVectorSection*>& oSectionArray );
        void GetSections( FOdysseyVectorVertexIntersection* iIntersectionVertex
                        , FOdysseyVectorSegment* iSegment
                        , std::vector<FOdysseyVectorSection*>& oSectionArray );
        FSectionLinkInfo* GetNextSectionLinkInfo( FOdysseyVectorSection* iLastSection
                                                , FOdysseyVectorVertex* iLastSectionVertex
                                                , uint32 iLastSectionVertexIndex );

    private :
        FOdysseyVectorPath* mPath;
        std::vector<FOdysseyVectorVertex*> mVertexArray;
        std::vector<FOdysseyVectorSegment*> mSegmentArray;
        double mLength;
        ::ULIS::FRectD mBBox;
};
