// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>

class FOdysseyVectorVertex;
class FOdysseyVectorSegment;
class FOdysseyVectorSection;
class FOdysseyVectorPath;
class FOdysseyVectorObject;
class FOdysseyVectorVertexIntersection;
struct FSectionLinkInfo;
struct FWayFragment;

enum class eSegmentAdditionFlags : uint8
{
    None                  =        0  ,
    KeepOriginalSegment   = ( 1 << 0 ),
    RemoveOriginalSegment = ( 1 << 1 ),
    CreateDerivedSegment  = ( 1 << 2 ),
    CreateNewPath         = ( 1 << 3 )
};

// define bitwise op
ENUM_CLASS_FLAGS(eSegmentAdditionFlags)

enum class eVertexAdditionFlags : uint8
{
    None                 =        0  ,
    RemoveOriginalVertex = ( 1 << 0 ),
    CreateDerivedVertex  = ( 1 << 1 ),
    CreateBoundaryVertex = ( 1 << 2 )
};

// define bitwise op
ENUM_CLASS_FLAGS(eVertexAdditionFlags)

// a waypoint is met at segment vertex or when a constrast is met
struct FWayPoint
{
    FOdysseyVectorVertexIntersection* intersectionVertex;
    FOdysseyVectorVertex* vertex;
    uint32 flags;
    double t;
    std::vector<FWayFragment*> fragmentArray;

    // WayPoint flags
    static const uint32 Original           = ( 1 << 0 );
    static const uint32 OutsideErasureArea = ( 1 << 1 );
    static const uint32 BordersErasureArea = ( 1 << 2 );
    static const uint32 InsideErasureArea  = ( 1 << 3 );

    FWayPoint()
    {
        flags = 0;
    }

    FWayPoint( FOdysseyVectorVertex* iVertex, uint32 iWayPointFlags )
    {
        intersectionVertex = nullptr;
        vertex = iVertex;
        flags = iWayPointFlags;

        fragmentArray.reserve( 2 );
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

        fragmentArray.reserve( 2 );
    }
};

struct FMetaFragment
{
    FOdysseyVectorSegment* segment;
    uint32 wayPoint0Index;
    uint32 wayPoint1Index;
    bool erased;

    FMetaFragment( FOdysseyVectorSegment* iSegment
                 , uint32 iWayPoint0Index
                 , uint32 iWayPoint1Index
                 , bool iErased )
        : segment( iSegment )
        , wayPoint0Index( iWayPoint0Index )
        , wayPoint1Index( iWayPoint1Index )
        , erased( iErased )
    {
    }
};

struct FWayFragment
{
    FOdysseyVectorSegment* segment;
    FWayPoint* wayPoint0;
    FWayPoint* wayPoint1;
    ::ULIS::FVec2D bezier[4];
    bool erased;

    FWayFragment( FOdysseyVectorSegment* iSegment
                , FWayPoint* iWayPoint0
                , FWayPoint* iWayPoint1
                , bool iErased );

    FWayFragment* GetNext();
    FWayFragment* GetPrev();
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
        static void ExtendErasedSection( FOdysseyVectorPath* iPath, FSectionLinkInfo* iLastSectionInfo );

        void ParseWayPoints( std::vector<FWayPoint>& iWayPointArray
                           , std::vector<FWayFragment>& iWayFragmentArray
                           , std::vector<FOdysseyVectorObject*>& oAddedPathArray
                           , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                           , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                           , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                           , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                           , bool iSplit );

    private :
        bool EraseSections( std::vector<FWayPoint>& oWayPointArray
                          , std::vector<FWayFragment>& oWayFragmentArray );

        bool EraseSegments( BLImageData* iImageData
                          , std::vector<FWayPoint>& oWayPointArray
                          , std::vector<FWayFragment>& oWayFragmentArray );
        uint8 GetAlpha( int32 iX, int32 iY, BLImageData* iImageData );
        bool CheckContrast( uint8 iAlphaValue0, uint8 iAlphaValue1 );

        bool SegmentCreationPolicy( FWayPoint* iWayPoint0, FWayPoint* iWayPoint1 );

        FOdysseyVectorVertex* TraceLine( int32 iX0
                                       , int32 iY0
                                       , double iT0
                                       , double iRadius0
                                       , int32 iX1
                                       , int32 iY1
                                       , double iT1
                                       , double iRadius1
                                       , BLImageData* iImageData
                                       , std::vector<FWayPoint>& oWayPointBuffer
                                       , std::vector<FMetaFragment>& oMetaFragmentBuffer
                                       , FOdysseyVectorVertex* iChainVertex
                                       , FOdysseyVectorSegment* iSegment
                                       , bool iRevert );

        bool Trace( BLImageData* iImageData
                  , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                  , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                  , std::vector<FWayPoint>& oWayPointArray
                  , std::vector<FWayFragment>& oWayFragmentArray );
        bool PickSection( FOdysseyVectorSection* iSection, const BLImageData& iMaskData );
        //bool PickSections( std::vector<FOdysseyVectorSection*>& oPickedSectionArray );

        void GetSections( FOdysseyVectorVertex* iVertex
                        , FOdysseyVectorPath* iPath
                        , std::vector<FOdysseyVectorSection*>& oSectionArray );
        void GetSections( FOdysseyVectorVertexIntersection* iIntersectionVertex
                        , FOdysseyVectorSegment* iSegment
                        , std::vector<FOdysseyVectorSection*>& oSectionArray );
        static eVertexAdditionFlags VertexAdditionPolicy( FWayPoint* iWayPoint, bool iSplit );
        static eSegmentAdditionFlags SegmentAdditionPolicy( FWayFragment* iFragment, bool iSplit );
        static FWayFragment* GetStartFragment( FWayFragment* iFragment );
        void VertexToWaypoint( BLImageData* iImageData
                             , FOdysseyVectorVertex* iVertex
                             , std::vector<FWayPoint>& oWayPointArray );

    private :
        FOdysseyVectorPath* mPath;
        std::vector<FOdysseyVectorVertex*> mVertexArray;
        std::vector<FOdysseyVectorSegment*> mSegmentArray;
        double mLength;
        ::ULIS::FRectD mBBox;
};
