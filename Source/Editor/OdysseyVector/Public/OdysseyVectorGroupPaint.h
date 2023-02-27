#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyVectorBucket.h"
#include "OdysseyVectorLoop.h"

#include "OdysseyVectorGroupPaint.generated.h"

typedef struct _FCycleNode
{
    struct _FCycleNode* parent;
    std::list<FOdysseyVectorSection*>* bypassEdgeList;
    UOdysseyVectorVertex* vertex;
    FOdysseyVectorSection* section;
    uint32 depth;
} FCycleNode;

typedef struct _FCycleSearch
{
    FOdysseyVectorSection* startSection;
    FOdysseyVectorSection* endSection;
} FCycleSearch;

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorGroupPaint : public UOdysseyVectorGroup
{
    public:
        GENERATED_BODY()

    protected:
        uint32 MarchVertex( UOdysseyVectorVertexIntersection* iVertex );

        static const uint32 NOCYCLE  = 0;
        static const uint32 BLOCKED  = 1;
        static const uint32 HASCYCLE = 2;


        std::list<FOdysseyVectorBucket*> mBucketList;
        std::vector<FOdysseyVectorLoop*> mLoopArray;

    public:
        UPROPERTY(EditAnywhere, Category="General")
        bool Realtime; // relatime updates

    public:
        void ApplyBucket( FOdysseyVectorBucket* iBucket );
        void Colorize();

    public:
        ~UOdysseyVectorGroupPaint();
        UOdysseyVectorGroupPaint();
        void Init( std::string iName );

        void UpdateShape( uint32 iUpdateFlags );
        void DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags );
        bool PickShape( ::ULIS::FRectD& iRoi, uint32 iSelectionFlags );
        UOdysseyVectorObject* CopyShape();

        uint32 IntersectSegment( UOdysseyVectorSegmentCubic& iCubicSegment
                               , std::list<UOdysseyVectorSegment*>& cubicSegmenList
                               , std::vector<UOdysseyVectorVertexIntersection*>& iIntersectionVertexList );

        uint32 BuildGraph( std::vector<UOdysseyVectorVertexIntersection*>& iIntersectionVertexArray );

        /*FOdysseyVectorLoop* MakeCycle( uint64 iCycleID
                                     , std::vector<UOdysseyVectorVertex*>& iVertexArray
                                     , std::vector<FOdysseyVectorSection*>& iSectionArray );*/
        FOdysseyVectorLoop* March( UOdysseyVectorVertexIntersection* iNode
                                 , FOdysseyVectorSection* iStartSection
                                 , FOdysseyVectorSection* iEndSection );
/*
        FOdysseyVectorLoop* FindPath( UOdysseyVectorVertexIntersection* iVertex
                                    , FOdysseyVectorSection* iStartSection
                                    , FOdysseyVectorSection* iEndSection
                                    , std::vector<UOdysseyVectorVertex*>& oVertexArray
                                    , std::vector<UOdysseyVectorVertex*>& oSectionArray );
*/
        uint32 FindPath( UOdysseyVectorVertex* iVertex
                                  , FOdysseyVectorSection* iSection
                                  , std::vector<UOdysseyVectorVertex*>& iVertexArray
                                  , std::vector<FOdysseyVectorSection*>& iSectionArray
                                  , double iOrientation
                                  , uint32 );

        std::list<FOdysseyVectorBucket*>& GetBucketList();
        void MarchCycle( FOdysseyVectorLoop& iCycle );

        void FindCycles();
        virtual uint32 GetType();
        void SimplifyGraph();
        FOdysseyVectorBucket* Bucket( double iX, double iY, uint8 iR, uint8 iG, uint8 iB, uint8 iA );
        FOdysseyVectorBucket* PickBucket( double iX, double iY );
        FOdysseyVectorLoop* PickCycle( double iX, double iY );
        UOdysseyVectorHandleBucket* PickBucketHandle( double iX, double iY );
        void DrawBuckets( ::ULIS::FRectD& iRoi, uint64 iFlags );
        void OrderCycles();
        void ClearCycles();
        void AddBucket( FOdysseyVectorBucket* iBucket );
        void RemoveBucket( FOdysseyVectorBucket* iBucket );
};
