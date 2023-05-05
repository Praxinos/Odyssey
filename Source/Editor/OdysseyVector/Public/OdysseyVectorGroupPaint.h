#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorIntersection.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyVectorBucket.h"
#include "OdysseyVectorCycle.h"

#include "OdysseyVectorGroupPaint.generated.h"

USTRUCT()
struct FGroupPaintParam
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="General")
    bool Realtime; // relatime updates

    UPROPERTY(EditAnywhere, Category="General")
    double Tolerance;

    UPROPERTY(EditAnywhere, Category="General")
    bool Wireframe;

    UPROPERTY(EditAnywhere,Category="General")
    FColor WireframeColor;
};

class ODYSSEYVECTOR_API FOdysseyVectorGroupPaint : public FOdysseyVectorGroup
{
    private:
        static const uint32 mStaticClass =  0xa5a4b5bd; // value is crc32 FOdysseyVectorGroupPaint

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };
        bool HasBaseClass( uint32 iBaseClassID );

        /**
         * @brief destructor
         */
        virtual ~FOdysseyVectorGroupPaint();

        virtual void TransferChild( FOdysseyVectorObject* iFosterChild );

        /**
         * @brief constructor
         */
        FOdysseyVectorGroupPaint( std::string iName );

        /**
         * @brief function called after a child has its matrix updated.
         * @param iChild the child.
         */
        virtual void OnChildTransform( FOdysseyVectorObject* iChild ) override;

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
        virtual void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags ) override;

        virtual void Draw( ::ULIS::FRectD& iRoi, uint64 iFlags ) override;

        /**
         * @brief Pick the shape.
         * @param iRoi picking region.
         * @param iFlags picking flags.
             PICK_MATH_BASED : picking is according to the picking region.
             PICK_MASK_BASED : picking is according to the mask image.
         * @return true if the shape is picked, false otherwise.
         */
        virtual bool PickShape( ::ULIS::FRectD& iRoi, uint32 iSelectionFlags ) override;

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

        virtual uint32 GetType();

        FOdysseyVectorBucket* Bucket( double iX, double iY, uint8 iR, uint8 iG, uint8 iB, uint8 iA );
        FOdysseyVectorBucket* PickBucket( double iX, double iY );
        FOdysseyVectorCycle* PickCycle( double iX, double iY );
        FOdysseyVectorHandleBucket* PickBucketHandle( double iX, double iY );
        std::list<FOdysseyVectorBucket*>& GetBucketList();
        void AddBucket( FOdysseyVectorBucket* iBucket );
        void RemoveBucket( FOdysseyVectorBucket* iBucket );
        void Colorize();
        void DrawBuckets( FBucketDrawingFlags iDrawingFlags );
        virtual void Invalidate();
        void MergeCycles();
        double GetGapTolerance();
        void SetGapTolerance( double iGapTolerance );
        bool IsWireframe();
        void SetWireframe( bool iIsWireframe );
        void UpdateBBox();

    protected:
        /**
         * @brief Intersect a cubic segment. It creates the intersection vertices and the section (sub-segments).
         * @param iSegment the segment.
         * @param iSegmenList the other segments to intersect iCubicSegment with.
         * @param oIntersectionList list populated by the pointers to the intersection that will be created.
         * @return the number of intersections
         */
        uint32 IntersectSegment( FOdysseyVectorSegment* iSegment
                               , std::list<FOdysseyVectorSegment*>& iSegmenList
                               , std::vector<FOdysseyVectorIntersection*>& oIntersectionList );

        /**
         * @brief Build the graph that allows to detect the cycles. It basically checks intersections
         */
        void BuildGraph( );

        void FindCycles();

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
        uint32 FindPath( FOdysseyVectorSection* iReturnSection
                       , FOdysseyVectorVertex* iVertex
                       , FOdysseyVectorSection* iSection
                       , std::vector<FOdysseyVectorVertex*>& oVertexArray
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

        /**
         * @brief Find closed paths and create a cycle.
         */
        void CheckLoops();

        uint32 Explore( FExplorationPair* iExplorationPair );

        void PropagateBuckets();

    protected:
        static const uint32 NOCYCLE  = 0;
        static const uint32 BLOCKED  = 1;
        static const uint32 HASCYCLE = 2;
        std::list<FOdysseyVectorBucket*> mBucketList;
        std::vector<FOdysseyVectorCycle*> mCycleArray;
        std::vector<FOdysseyVectorIntersection*> mIntersectionArray;
        // need to remember them in order to clean properly
        std::vector<uint32> pathSectionCount;
        uint32 mPaintingCode;

        std::vector<FOdysseyVectorSection> mSectionBuffer;
        std::vector<FOdysseyVectorSegmentCubic> mGapSegmentBuffer;

    public:
        FGroupPaintParam mGroupPaintParam;
};
