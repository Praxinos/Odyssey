// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorSegmentCubicGap.h"
#include "OdysseyVectorPath.h"

class FOdysseyVectorVertex;
class FOdysseyVectorVertexIntersection;
class FOdysseyVectorSection;
class FOdysseyVectorIntersection;
class FOdysseyVectorBucket;
class FOdysseyVectorCycle;

// small temporary structure will allow us to alloc the intersection vertices in one go.
// for X-Junction
struct FXIntersectionRecord
{
    FOdysseyVectorSegment* segment0;
    FOdysseyVectorSegment* segment1;
    double segment0T;
    double segment1T;
    double x;
    double y;

    FXIntersectionRecord( double iX
                        , double iY
                        , FOdysseyVectorSegment* iSegment0
                        , double iSegment0T
                        , FOdysseyVectorSegment* iSegment1
                        , double iSegment1T )
        : segment0 ( iSegment0 )
        , segment1 ( iSegment1 )
        , segment0T( iSegment0T )
        , segment1T( iSegment1T )
        , x( iX )
        , y( iY )
    {
        iSegment0->AddIntersectionSlot();
        iSegment1->AddIntersectionSlot();
    }
};

// small temporary structure will allow us to alloc the intersection vertices in one go.
// for T-Junction
struct FTIntersectionRecord
{
    FOdysseyVectorSegment* segment;
    double segmentT;
    double x;
    double y;
    FOdysseyVectorVertex* vertex;

    // for T-Junction Gaps
    FTIntersectionRecord( double iX
                        , double iY
                        , FOdysseyVectorSegment* iSegment
                        , double iSegmentT
                        , FOdysseyVectorVertex* iVertex )
        : segment( iSegment )
        , segmentT( iSegmentT )
        , x( iX )
        , y( iY )
        , vertex( iVertex )
    {
        iSegment->AddIntersectionSlot();
    }
};

class ODYSSEYVECTOR_API FOdysseyVectorGroupPaint : public FOdysseyVectorGroup
{
    private:
        static const uint32 mStaticClass =  0xa5a4b5bd; // value is crc32 FOdysseyVectorGroupPaint

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() override { return mStaticClass; };
        virtual bool HasBaseClass( uint32 iBaseClassID ) override;

        /**
         * @brief destructor
         */
        virtual ~FOdysseyVectorGroupPaint();

        /**
         * @brief constructor
         */
        FOdysseyVectorGroupPaint( const FString& iName );

        /**
         * @brief Update the shape's cached data e.g.
         * @param iUpdateFlags
         */
        virtual void UpdateShape( uint32 iUpdateFlags ) override;

        /**
         * @brief Update the shape. The matrix transform is done before the call to this function.
         * @param iRoi region of interest.
         * @param iFlags drawing flags.
         */
        virtual void DrawShape( BLContext* iBLContext
                              , const ::ULIS::FRectD& iInvalidationArea
                              , double iCombinedOpacity
                              , uint64 iFlags ) override;

        /**
         * @brief Pick the shape.
         * @param iRoi picking region.
         * @param iFlags picking flags.
             PICK_MATH_BASED : picking is according to the picking region.
             PICK_MASK_BASED : picking is according to the mask image.
         * @return true if the shape is picked, false otherwise.
         */
        virtual bool PickShape( const ::ULIS::FRectD& iRoi, uint32 iSelectionFlags ) override;

        /**
         * @brief Copy the shape to a new object.
         * @return a pointer to  copy of the object
         */
        virtual FOdysseyVectorObject* CopyShape() override;

        /**
         * @brief Copy buckets from this PaintGroup to a destination PaintGroup.
         * @param iDestination the destination PaintGroup.
         * @param iSwitchSpace
         * @return a pointer to  copy of the object
         */
        void CopyBuckets( FOdysseyVectorGroupPaint* iDestination, bool iSwitchSpace );

        std::list<FOdysseyVectorBucket*>& GetBucketList();
        std::list<FOdysseyVectorCycle*>& GetCycleList();
        void AddBucket( FOdysseyVectorBucket* iBucket );
        void RemoveBucket( FOdysseyVectorBucket* iBucket );
        void RemoveAllBuckets();
        void Colorize();
        void MergeCycles();
        double GetGapTolerance();
        void SetGapTolerance( double iGapTolerance );
        bool IsWireframe();
        void SetWireframe( bool iIsWireframe );
        FColor& GetWireframeColor();
        void GetWireframeColor( uint8 &oR, uint8 &oG, uint8& oB, uint8& oA );
        void SetWireframeColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA );

        void SelectBucket( FOdysseyVectorBucket* iSelectedBucket );
        void SelectAllBuckets();
        void UnselectBucket( FOdysseyVectorBucket* iSelectedBucket );
        void UnselectAllBuckets();
        std::list<FOdysseyVectorBucket*>& GetSelectedBucketList();

        bool GetBBoxFromSelectedVertices( ::ULIS::FRectD& oBBox, bool iWorld );
        void PickBucket( std::vector<FOdysseyVectorBucket*>& oPickedBucketArray );
        FOdysseyVectorCycle* PickCycle( double iWorldX, double iWorldY );
        FOdysseyVectorBucket* PickBucket( double iWorldX, double iWorldY );
        bool IsMonochrome();
        void SetMonochrome( bool iIsMonochrome );
        FColor& GetMonochromeColor();
        void GetMonochromeColor( uint8 &oR, uint8 &oG, uint8& oB, uint8& oA );
        void SetMonochromeColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA );

        bool IsPainted();
        void SetPainted( bool iPainted );

        void IntersectSegment( FOdysseyVectorSegment* iSegment0
                             , FOdysseyVectorSegment* iSegment1
                             , const ::ULIS::FVec2D& iSegment1MinInParentWithTolerance
                             , const ::ULIS::FVec2D& iSegment1MaxInParentWithTolerance
                             , std::vector<FXIntersectionRecord>& iIntersectionRecordArray );
        void IntersectVertex( FOdysseyVectorVertex* iVertex0
                            , const ::ULIS::FVec2D& iPoint0InParent
                            , FOdysseyVectorVertex* iVertex1
                            , const ::ULIS::FVec2D& iPoint1InParent );
        virtual void ApplyTransformations() override;
        virtual void ApplyMatrix( BLMatrix2D& iMatrix ) override;

        void GetChildrenPaths( std::vector<FOdysseyVectorPath*>& oPathArray );
        void PickSectionLessPaths( std::vector<FOdysseyVectorObject*>& oObjectArray );
        void SetRealtime( bool iRealtime );
        void SetIntersectsCanevas( bool iIntersectCanevas );
        bool IntersectsCanevas();

        bool IsRealtime();
        void SetWireframeColor( const FColor& iWireframeColor );
        void SetMonochromeColor( const FColor& iMonochromeColor );
        void EraseSections( std::vector<FOdysseyVectorObject*>& oAddedPathArray
                          , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                          , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                          , std::vector<FOdysseyVectorObject*>& oRemovedPathArray
                          , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                          , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                          , bool iSplit );

        void SetMultithreaded( bool iMultithreaded );
        bool IsMultithreaded();
        std::vector<FOdysseyVectorVertexIntersection>& GetIntersectionVertexArray();

        virtual void UpdateMatrix() override; // updates the canevas path

        void AlterContourWidth( double iValue, bool iAbsolute );
        bool PickSection( FOdysseyVectorSection* iSection
                        , const ::ULIS::FRectD& iMaskRect
                        , const uint8* iMaskPixelData );
        void PickErasedSections( std::vector<FOdysseyVectorSection*>& oErasedSectionArray );

    protected:
        /**
         * @brief Intersect a cubic segment. It creates the intersection vertices and the section (sub-segments).
         * @param iSegment the segment.
         * @param iSegmenList the other segments to intersect iCubicSegment with.
         * @return the number of intersections
         */
        void IntersectSegmentWithList( FOdysseyVectorSegment* iSegment
                                     , const std::list<FOdysseyVectorSegment*>& iSegmenList );

        /**
         * @brief Build the graph that allows to detect the cycles. It basically checks intersections
         */
        void BuildGraph( );

        void FindCycles();

        void SanitizeGraph();
        void SimplifyGraph();

        /**
         * @brief Recursive function that traverses the graph and find cycles.
         * @param iReturnSection
         * @param iVertex the vertex being explored, either intersection vertex or regular ones.
         * @param iSection the section that will take us to the next vertex.
         * @param oVertexArray an array that receives the explored vertices.
         * @param oSectionArray an array that receives the explored sections.
         * @param iOrientation orientation of the exploration. Means we explore CW or CCW.
         * @param iDepth current recursion depth.
         * @return iCubicSegment the segment.
         */
         // TODO: rename "Path" to something else, it is confusing with FOdysseyVectorPath
        uint32 FindPath( FOdysseyVectorSection* iReturnSection
                       , uint32 iSectionVertexIndex
                       , FOdysseyVectorSection* iSection
                       , std::vector<uint32>& oVertexIndexArray
                       , std::vector<FOdysseyVectorSection*>& oSectionArray
                       , double iOrientation
                       , uint32 iDepth );

        /**
         * @brief Sort found cycles to detect cycles that are within other cycles. This allows cycles with holes.
         * @return iCubicSegment the segment.
         */
        void OrderCycles();

        /**
         * @brief Delete all cycles, intersection vertices and sections. Create default
         * section for each segment. It also takes advantage of this process to retrieve the list of cubic segments.
         */
        void Clear();

        /**
         * @brief Find a cycle where the bucket passed as argument fits in and set it as the bucket for this cycle.
         * @param iBucket the bucket.
         */
        void ApplyBucket( FOdysseyVectorBucket* iBucket );

        uint32 Explore( FExplorationPair* iExplorationPair );

        void PropagateBuckets();

        void CreateVertexGapSegment( FOdysseyVectorVertex* iVertex );
        void CreateSegmentSections( FOdysseyVectorSegment* iSegment );
        void CreatePathSections( FOdysseyVectorPath* iPath
                               , BLMatrix2D* iConversionMatrix );

        FOdysseyVectorVertex* ReachVertexFromSection( FOdysseyVectorVertex* iVertex
                                                    , FOdysseyVectorSection* iFromSection
                                                    , FOdysseyVectorSegment* iOwnerSegment );
        bool PickSections( std::vector<FOdysseyVectorSection*>& oPickedSectionArray );

        void SetSegmentBBox( FOdysseyVectorSegment* iSegment
                           , BLMatrix2D& iConversionMatrix );
        bool IntersectGapSection( FOdysseyVectorSection* iGapSection
                                , FOdysseyVectorSegmentCubic* iSegment );
        void CreateNearIntersection( FOdysseyVectorVertex *iVertex );

        void MakeCanevasPath();

        void UpdatePathList();


    protected:
        static const uint32 NOCYCLE  = 0;
        static const uint32 BLOCKED  = 1;
        static const uint32 HASCYCLE = 2;
        // TODO: transform mPathList to mPathArray
        std::list<FOdysseyVectorPath*> mPathList;
        std::list<FOdysseyVectorBucket*> mSelectedBucketList;
        std::list<FOdysseyVectorBucket*> mBucketList;
        std::list<FOdysseyVectorCycle*> mCycleList;
        std::vector<FOdysseyVectorVertexIntersection> mIntersectionVertexArray;
        uint32 mPaintingCode;
        std::mutex mMutex;
        // short section are section with length = 0. We have to get rid of them to sanitize the graph
        std::vector<FOdysseyVectorSection*> mShortSectionArray;
        // temporarily store intersection info before creating them
        std::vector<FXIntersectionRecord> mXIntersectionRecordArray;
        std::vector<FTIntersectionRecord> mTIntersectionRecordArray;
        // allocated in one go after all intersection info have been gathered.
        std::vector<FOdysseyVectorSection> mSectionBuffer;
        std::vector<FOdysseyVectorSection> mGapSectionBuffer;
        std::vector<FOdysseyVectorSegmentCubicGap> mGapSegmentBuffer;
        uint32 mVertexID;
        bool bMultithreaded;
        bool bPainted;
        bool bMonochrome;
        FColor mMonochromeColor;
        bool bRealtime; // relatime updates
        double mGapTolerance;
        bool bWireframe;
        bool bIntersectsCanevas;
        FColor mWireframeColor;
        // the frame canevas path intersects with the canvas
        FOdysseyVectorPath mCanevasPath;
        FOdysseyVectorVertex mCanevasVertex[4];
        FOdysseyVectorSegmentCubic mCanevasSegment[4];
};
